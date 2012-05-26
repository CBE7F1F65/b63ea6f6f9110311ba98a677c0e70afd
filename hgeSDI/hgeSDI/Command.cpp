#include "stdafx.h"
#include "Command.h"

#include "Main.h"
#include "RenderHelper.h"

#define COMMANDLOGSTR_CREATECOMMAND		"OnCommand: "
#define COMMANDLOGSTR_PROCESSCOMMAND	"Processing: "
#define COMMANDLOGSTR_FINISHCOMMAND		"DoneCommand: "
#define COMMANDLOGSTR_TERMINALCOMMAND	"TerminalCommand: "
#define COMMANDLOGSTR_ERROR				"Error: "
#define COMMANDLOGSTR_PARAM				"SetParam: "
#define COMMANDLOGSTR_WANTNEXT			"Please Input: "


Command::Command()
{
//	ZeroMemory(&ccomm, sizeof(ccomm));
	ccomm.ClearSet();
	tarcommand = 0;
	Init();
}

Command::~Command()
{

}

void Command::Init()
{
	InitCommandStrInfo();
	InitWantPromptInfo();
	TerminalInternalProcess();
	ClearCurrentCommand();
	SetRenderTarget(0);
}

void Command::LogCreate()
{
	if (ccomm.command < _COMM_NOLOGCOMMANDBEGIN || ccomm.command > _COMM_NOLOGCOMMANDEND)
	{
		char strlog[M_STRMAX];
		sprintf_s(strlog, M_STRMAX, "%s%s", COMMANDLOGSTR_CREATECOMMAND, GetCommandStr());
		MainInterface::getInstance().CallAppendCommandLogText(strlog);
	}
}

void Command::LogFinish()
{
	if (ccomm.command < _COMM_NOLOGCOMMANDBEGIN || ccomm.command > _COMM_NOLOGCOMMANDEND)
	{
		char strlog[M_STRMAX];
		sprintf_s(strlog, M_STRMAX, "%s%s", COMMANDLOGSTR_FINISHCOMMAND, GetCommandStr());
		MainInterface::getInstance().CallAppendCommandLogText(strlog);
	}
}

void Command::LogTerminal()
{
	if (ccomm.command < _COMM_NOLOGCOMMANDBEGIN || ccomm.command > _COMM_NOLOGCOMMANDEND)
	{
		char strlog[M_STRMAX];
		sprintf_s(strlog, M_STRMAX, "%s%s", COMMANDLOGSTR_TERMINALCOMMAND, GetCommandStr());
		MainInterface::getInstance().CallAppendCommandLogText(strlog);
	}
}

void Command::_LogParam( int index, int useflag, int cwp/*=-1 */ )
{
	if (ccomm.command < _COMM_NOLOGCOMMANDBEGIN || ccomm.command > _COMM_NOLOGCOMMANDEND)
	{
		char strlog[M_STRMAX*4];
		char paramstr[M_STRMAX];
		switch (useflag)
		{
		case COMMPARAMFLAG_X:
			sprintf_s(paramstr, M_STRMAX, "%f", ccomm.params[index].x);
			break;
		case COMMPARAMFLAG_Y:
			sprintf_s(paramstr, M_STRMAX, "%f", ccomm.params[index].y);
			break;
		case COMMPARAMFLAG_F:
			sprintf_s(paramstr, M_STRMAX, "%f", ccomm.params[index].fval);
			break;
		case COMMPARAMFLAG_I:
			sprintf_s(paramstr, M_STRMAX, "%d", ccomm.params[index].ival);
			break;
		case COMMPARAMFLAG_G:
			sprintf_s(paramstr, M_STRMAX, "%f", ccomm.params[index].flag);
			break;
		case COMMPARAMFLAG_S:
			ccomm.params[index].sval = paramstr;
//			strcpy_s(paramstr, M_STRMAX, ccomm.params[index].sval);
			break;
			
		}
		sprintf_s(strlog, M_STRMAX*4, "(%s) %s%s%s", GetCommandStr(), COMMANDLOGSTR_PARAM, GetWantPromptStr(cwp), paramstr);
		MainInterface::getInstance().CallAppendCommandLogText(strlog);
	}
}

void Command::LogWantNext()
{
	if (!ccomm.wantprompt || IsInternalProcessing())
	{
		return;
	}
	if (ccomm.command < _COMM_NOLOGCOMMANDBEGIN || ccomm.command > _COMM_NOLOGCOMMANDEND)
	{
		char strlog[M_STRMAX];
		sprintf_s(strlog, M_STRMAX, "%s: %s%s", GetCommandStr(), COMMANDLOGSTR_WANTNEXT, GetWantPromptStr());
		MainInterface::getInstance().CallAppendCommandLogText(strlog);
	}
}

void Command::LogError( const char * str )
{
	char strlog[M_STRMAX*4];
	if (ccomm.command && ccomm.wantprompt)
	{
		sprintf_s(strlog, M_STRMAX*4, "(%s) %s%s%s", GetCommandStr(), COMMANDLOGSTR_ERROR, GetWantPromptStr(), str);
	}
	else
	{
		sprintf_s(strlog, M_STRMAX*4, "%s%s", COMMANDLOGSTR_ERROR, str);
	}
	MainInterface::getInstance().CallAppendCommandLogText(strlog);
}

int Command::CreateCommand(int comm)
{
	if (comm > _COMM_PUSHCOMMANDBEGIN && comm < _COMM_PUSHCOMMANDEND)
	{
		PushCommand();
	}

	if (comm > _COMM_INSTANTCOMMANDBEGIN && comm < _COMM_INSTANTCOMMANDEND)
	{
	}
	else
	{
		ClearCurrentCommand(true);
		ccomm.command = comm;
	}
	LogCreate();
	return ccomm.command;
}

int Command::FinishCommand()
{
	LogFinish();

	for (list<CommandStepInfo>::iterator it = histcomm.begin(); it!=histcomm.end();)
	{
		if (it->command == ccomm.command)
		{
			it = histcomm.erase(it);
		}
		else
		{
			++it;
		}
	}
	if (ccomm.command > _COMM_PUSHCOMMANDBEGIN && ccomm.command < _COMM_PUSHCOMMANDEND)
	{
		return PullCommand();
	}
	else
	{
		return ClearCurrentCommand();
	}
}

int Command::TerminalCommand()
{
	LogTerminal();

	for (list<CommandStepInfo>::iterator it = histcomm.begin(); it!=histcomm.end();)
	{
		if (it->command == ccomm.command)
		{
			it = histcomm.erase(it);
		}
		else
		{
			++it;
		}
	}
	if (ccomm.command > _COMM_PUSHCOMMANDBEGIN && ccomm.command < _COMM_PUSHCOMMANDEND)
	{
		return PullCommand();
	}
	else
	{
		return ClearCurrentCommand();
	}
}

int Command::PushCommand()
{
	if (ccomm.command)
	{
		StepTo(CSI_PAUSE);
		ProcessCommand();
		StepBack();
	}
	pushedcomm.push_back(ccomm);
	return ClearCurrentCommand();
}

int Command::PullCommand()
{
	if (!pushedcomm.size())
	{
		return 0;
	}
	SetCurrentCommand(&(pushedcomm.back()));
	pushedcomm.pop_back();
	if (ccomm.command)
	{
		StepTo(CSI_RESUME);
		ProcessCommand();
		StepBack();
	}
	return ccomm.command;
}

int Command::ClearCurrentCommand(bool callterminal /*=false*/)
{
//	inputcommandlist.clear();
	pendingparam.ClearSet();
	if (ccomm.command && callterminal)
	{
		StepTo(CSI_TERMINAL);
		ProcessCommand();
		StepBack();
	}
	ccomm.ClearSet();
//	ZeroMemory(&ccomm, sizeof(ccomm));
	return 0;
}

int Command::SetCurrentCommand( CommandStepInfo * info )
{
	if (!info)
	{
		return -1;
	}
	ccomm = *info;
//	memcpy(&ccomm, info, sizeof(ccomm));
	return 0;
}
/*
int Command::SetParamXY( int index, float x, float y )
{
	if (index < 0 || index >= COMMPARAMMAX)
	{
		return -1;
	}
	ccomm.params[index].x = x;
	ccomm.params[index].y = y;
	ccomm.params[index].useflag |= COMMPARAMFLAG_XY;
	LogParam(index, COMMPARAMFLAG_XY);
	return ccomm.command;
}
*/
int Command::SetParamX( int index, float x, int cwp/*=-1*/ )
{
	if (index < 0)
	{
		return -1;
	}
	GrowParam(index);
	ccomm.params[index].x = x;
	ccomm.params[index].useflag |= COMMPARAMFLAG_X;
	_LogParam(index, COMMPARAMFLAG_X, cwp);
	return ccomm.command;
}

int Command::SetParamY( int index, float y, int cwp/*=-1*/ )
{
	if (index < 0)
	{
		return -1;
	}
	GrowParam(index);
	ccomm.params[index].y = y;
	ccomm.params[index].useflag |= COMMPARAMFLAG_Y;
	_LogParam(index, COMMPARAMFLAG_Y, cwp);
	return ccomm.command;
}

int Command::SetParamF( int index, float fval, int cwp/*=-1*/ )
{
	if (index < 0)
	{
		return -1;
	}
	GrowParam(index);
	ccomm.params[index].fval = fval;
	ccomm.params[index].useflag |= COMMPARAMFLAG_F;
	_LogParam(index, COMMPARAMFLAG_F, cwp);
	return ccomm.command;
}

int Command::SetParamI( int index, int ival, int cwp/*=-1*/ )
{
	if (index < 0)
	{
		return -1;
	}
	GrowParam(index);
	ccomm.params[index].ival = ival;
	ccomm.params[index].useflag |= COMMPARAMFLAG_I;
	_LogParam(index, COMMPARAMFLAG_I, cwp);
	return ccomm.command;
}

int Command::SetParamS( int index, const char * sval, int cwp/*=-1*/ )
{
	if (index < 0)
	{
		return -1;
	}
	GrowParam(index);
	ccomm.params[index].sval = sval;
//	strcpy_s(ccomm.params[index].sval, M_STRMAX, sval);
	ccomm.params[index].useflag |= COMMPARAMFLAG_S;
	_LogParam(index, COMMPARAMFLAG_S, cwp);
	return ccomm.command;
}

int Command::SetParamG( int index, int flag, int cwp/*=-1*/ )
{
	if (index < 0)
	{
		return -1;
	}
	GrowParam(index);
	ccomm.params[index].flag = flag;
	ccomm.params[index].useflag |= COMMPARAMFLAG_G;
	_LogParam(index, COMMPARAMFLAG_G, cwp);
	return ccomm.command;
}

bool Command::GetParamXY( int index, float * x, float * y )
{
	if (index < 0)
	{
		return false;
	}
	GrowParam(index);
	if (x)
	{
		*x = ccomm.params[index].x;
	}
	if (y)
	{
		*y = ccomm.params[index].y;
	}
	return true;
}

float Command::GetParamF( int index )
{
	if (index < 0)
	{
		return -1;
	}
	GrowParam(index);
	return ccomm.params[index].fval;
}

int Command::GetParamI( int index )
{
	if (index < 0)
	{
		return -1;
	}
	GrowParam(index);
	return ccomm.params[index].ival;
}

const char * Command::GetParamS( int index )
{
	if (index < 0)
	{
		return "";
	}
	GrowParam(index);
	return ccomm.params[index].sval.c_str();
}

int Command::GetParamG( int index )
{
	if (index < 0)
	{
		return -1;
	}
	GrowParam(index);
	return ccomm.params[index].flag;
}

bool Command::CheckParamSet( int index, int useflag )
{
	if (index < 0)
	{
		return false;
	}
	GrowParam(index);
	return (ccomm.params[index].useflag & useflag);

}

int Command::CommitCommand( const char * str )
{
	bool notfinished = false;
	CommittedCommand _cc;
	int inext = 0;//_FindNextSubStr(str, &_ic, M_STRMAX);
	/*
	if (!ccomm.command)
	{
		if (!CreateCommand(FindCommandByStr(_ic.substr.c_str())))
		{
			LogError(_ic.substr.c_str());
			return ccomm.command;
		}
		
		if (inext)
		{
			inext = _FindNextSubStr(str, _ic.substr, M_STRMAX, inext);
		}
		else
		{
			return ccomm.command;
		}
	}
	*/
	while (true)
	{
		inext = _FindNextSubStr(str, &_cc, M_STRMAX, inext);
		if (!_cc.type)
		{
			break;
		}
		inputcommandlist.push_back(_cc);
		if (!inext)
		{
			break;
		}
	}
	return ccomm.command;
}

bool _CheckCharToBreak(char ch)
{
	if (ch >= 'a' && ch <= 'z' || ch == '_' || ch >= 'A' && ch <= 'Z' || ch >= '0' && ch <= '9' || ch == '-' || ch == '+' || ch == '.' || ch >= 0x80)
	{
		return false;
	}
	return true;
}

bool _CheckCharQuote(char ch)
{
	if (ch == '"' || ch == '\'')
	{
		return true;
	}
	return false;
}

int Command::_FindNextSubStr( const char * str, CommittedCommand * cc, int maxnsubstr, int ibegin/*=0*/ )
{
	/*
	if (!substr)
	{
		return 0;
	}
	*/
//	strcpy_s(substr, maxnsubstr, "");
	if (!cc)
	{
		return 0;
	}
	cc->ClearSet();

	string substr = "";
	int nchars = 0;
	int i=ibegin;
	for (; i<strlen(str); i++)
	{
		if (!_CheckCharToBreak(str[i]))
		{
			break;
		}
	}
	if (i == strlen(str))
	{
		return 0;
	}

	bool checkquote = false;
	if (_CheckCharQuote(str[i]))
	{
		checkquote = true;
		i++;
	}
	ibegin = i;
	for (; i<strlen(str); i++)
	{
		nchars++;
		if (checkquote?_CheckCharQuote(str[i]):_CheckCharToBreak(str[i]))
		{
			break;
		}
	}

	if (i != strlen(str))
	{
		nchars--;
	}
	substr = str;
	substr = substr.substr(ibegin, nchars);

	if (!substr.length())
	{
		return 0;
	}
	cc->sval = substr;

	char ch[3];
	for (int i=0; i<3; i++)
	{
		if (substr.length() > i)
		{
			ch[i] = substr.c_str()[i];
		}
		else
		{
			ch[i] = 0;
		}
	}
	if (isdigit(ch[0]) || 
		(ch[0]=='.' || ch[0]=='+' || ch[0]=='-') && isdigit(ch[1]) ||
		(ch[0]=='+' || ch[0]=='-') && ch[1]=='.' && isdigit(ch[2]))
	{
		for (int i=0; i<substr.length(); i++)
		{
			if (substr.c_str()[i] == '.')
			{
				cc->type = COMMITTEDCOMMANDTYPE_FLOAT;
				break;
			}
		}
		if (!cc->type)
		{
			cc->type = COMMITTEDCOMMANDTYPE_INT;
		}
	}
	if (!cc->type)
	{
		if (_CheckCharQuote(ch[0]))
		{
			cc->type = COMMITTEDCOMMANDTYPE_STRING;
		}
	}
	if (!cc->type)
	{
		cc->ival = FindCommandByStr(substr.c_str());
		if (cc->ival)
		{
			cc->type = COMMITTEDCOMMANDTYPE_COMMAND;
		}
	}
	if (!cc->type)
	{
		cc->type = COMMITTEDCOMMANDTYPE_STRING;
	}

	switch (cc->type)
	{
	case COMMITTEDCOMMANDTYPE_ERROR:
	case COMMITTEDCOMMANDTYPE_COMMAND:
		break;
	case COMMITTEDCOMMANDTYPE_FLOAT:
		sscanf_s(substr.c_str(), "%f", &(cc->fval));
		cc->ival = cc->fval;
		break;
	case COMMITTEDCOMMANDTYPE_INT:
		sscanf_s(substr.c_str(), "%d", &(cc->ival));
		cc->fval = cc->ival;
		break;
	}


	if (i >= strlen(str))
	{
		return 0;
	}
	return i;
}

int Command::StepTo( int step, int wantnextprompt/*=0*/, bool pushback/*=true*/ )
{
	if (pushback)
	{
		histcomm.push_back(ccomm);
	}
	ccomm.step=step;
	ccomm.wantprompt = wantnextprompt;
	if (pushback)
	{
		LogWantNext();
	}
	return ccomm.step;
}

int Command::FindCommandByStr( const char * str )
{
	string lowerstr = str;
	transform(lowerstr.begin(), lowerstr.end(), lowerstr.begin(), tolower);

	for (int i=0; i<COMMANDINDEXMAX; i++)
	{
//		if (!strcmp(str, scinfo[i].str) || !strcmp(str, scinfo[i].shortstr))
		if (!scinfo[i].str.compare(lowerstr) || !(scinfo[i].shortstr.compare(lowerstr)))
		{
			return i;
		}
	}
	return 0;
}

int Command::StepBack()
{
	if (!histcomm.empty())
	{
		for (list<CommandStepInfo>::reverse_iterator it = histcomm.rbegin(); it!= histcomm.rend(); ++it)
		{
			if (it->command == ccomm.command)
			{
				StepTo(it->step, it->wantprompt, false);
				histcomm.erase(--(it.base()));
				break;
			}
		}
	}
	return ccomm.step;
}

void Command::Render()
{
	RenderHelper::TargetQuadRender_S(tarcommand, tarx, tary, 0xffffffff);
}

void Command::SetRenderTarget( HTARGET tar, float x/*=0*/, float y/*=0*/ )
{
	tarcommand = tar;
	tarx = x;
	tary = y;
}

void Command::GrowParam( int index )
{
	int size = ccomm.params.size();
	if (size < index+1)
	{
		for (int i=size; i<index+1; i++)
		{
			CommandParam _cp;
			ccomm.params.push_back(_cp);
		}
	}
}

void Command::_CommitFrontCommand( CommittedCommand &cc )
{
	inputcommandlist.push_front(cc);
}

void Command::CommitFrontCommandF( float fval )
{
	CommittedCommand cc;
	cc.type = COMMITTEDCOMMANDTYPE_FLOAT;
	cc.fval = fval;
	cc.ival = fval;
	_CommitFrontCommand(cc);
}

void Command::CommitFrontCommandI( int ival )
{
	CommittedCommand cc;
	cc.type = COMMITTEDCOMMANDTYPE_INT;
	cc.fval = ival;
	cc.ival = ival;
	_CommitFrontCommand(cc);

}

void Command::CommitFrontCommandS( const char * sval )
{
	CommittedCommand cc;
	cc.type = COMMITTEDCOMMANDTYPE_STRING;
	cc.sval = sval;
	_CommitFrontCommand(cc);
}

void Command::CommitFrontCommandC( int command )
{
	CommittedCommand cc;
	cc.type = COMMITTEDCOMMANDTYPE_COMMAND;
	cc.ival = command;
	cc.sval = GetCommandStr(command);
	_CommitFrontCommand(cc);
}

void Command::TerminalInternalProcess()
{
	inputcommandlist.clear();
}