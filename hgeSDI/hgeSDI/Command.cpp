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
	ZeroMemory(&ccomm, sizeof(ccomm));
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

void Command::LogParam( int index, int useflag )
{
	if (ccomm.command < _COMM_NOLOGCOMMANDBEGIN || ccomm.command > _COMM_NOLOGCOMMANDEND)
	{
		char strlog[M_STRMAX*4];
		char paramstr[M_STRMAX];
		switch (useflag)
		{
		case COMMPARAMFLAG_X:
			sprintf_s(paramstr, M_STRMAX, "%f", ccomm.param[index].x);
			break;
		case COMMPARAMFLAG_Y:
			sprintf_s(paramstr, M_STRMAX, "%f", ccomm.param[index].y);
			break;
		case COMMPARAMFLAG_F:
			sprintf_s(paramstr, M_STRMAX, "%f", ccomm.param[index].fval);
			break;
		case COMMPARAMFLAG_I:
			sprintf_s(paramstr, M_STRMAX, "%d", ccomm.param[index].ival);
			break;
		case COMMPARAMFLAG_G:
			sprintf_s(paramstr, M_STRMAX, "%f", ccomm.param[index].flag);
			break;
		case COMMPARAMFLAG_S:
			strcpy_s(paramstr, M_STRMAX, ccomm.param[index].sval);
			break;
			
		}
		sprintf_s(strlog, M_STRMAX*4, "(%s) %s%s%s", GetCommandStr(), COMMANDLOGSTR_PARAM, GetWantPromptStr(), paramstr);
		MainInterface::getInstance().CallAppendCommandLogText(strlog);
	}
}

void Command::LogWantNext()
{
	if (!ccomm.wantnext || IsInternalProcessing())
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
	inputcommandlist.clear();
	if (ccomm.command && callterminal)
	{
		StepTo(CSI_TERMINAL);
		ProcessCommand();
		StepBack();
	}
	ZeroMemory(&ccomm, sizeof(ccomm));
	return 0;
}

int Command::SetCurrentCommand( CommandStepInfo * info )
{
	if (!info)
	{
		return -1;
	}
	memcpy(&ccomm, info, sizeof(ccomm));
	return 0;
}
/*
int Command::SetParamXY( int index, float x, float y )
{
	if (index < 0 || index >= COMMPARAMMAX)
	{
		return -1;
	}
	ccomm.param[index].x = x;
	ccomm.param[index].y = y;
	ccomm.param[index].useflag |= COMMPARAMFLAG_XY;
	LogParam(index, COMMPARAMFLAG_XY);
	return ccomm.command;
}
*/
int Command::SetParamX( int index, float x )
{
	if (index < 0 || index >= COMMPARAMMAX)
	{
		return -1;
	}
	ccomm.param[index].x = x;
	ccomm.param[index].useflag |= COMMPARAMFLAG_X;
	LogParam(index, COMMPARAMFLAG_X);
	return ccomm.command;
}

int Command::SetParamY( int index, float y )
{
	if (index < 0 || index >= COMMPARAMMAX)
	{
		return -1;
	}
	ccomm.param[index].y = y;
	ccomm.param[index].useflag |= COMMPARAMFLAG_Y;
	LogParam(index, COMMPARAMFLAG_Y);
	return ccomm.command;
}

int Command::SetParamF( int index, float fval )
{
	if (index < 0 || index >= COMMPARAMMAX)
	{
		return -1;
	}
	ccomm.param[index].fval = fval;
	ccomm.param[index].useflag |= COMMPARAMFLAG_F;
	LogParam(index, COMMPARAMFLAG_F);
	return ccomm.command;
}

int Command::SetParamI( int index, int ival )
{
	if (index < 0 || index >= COMMPARAMMAX)
	{
		return -1;
	}
	ccomm.param[index].ival = ival;
	ccomm.param[index].useflag |= COMMPARAMFLAG_I;
	LogParam(index, COMMPARAMFLAG_I);
	return ccomm.command;
}

int Command::SetParamS( int index, const char * sval )
{
	if (index < 0 || index >= COMMPARAMMAX)
	{
		return -1;
	}
	strcpy_s(ccomm.param[index].sval, M_STRMAX, sval);
	ccomm.param[index].useflag |= COMMPARAMFLAG_S;
	LogParam(index, COMMPARAMFLAG_S);
	return ccomm.command;
}

int Command::SetParamG( int index, int flag )
{
	if (index < 0 || index >= COMMPARAMMAX)
	{
		return -1;
	}
	ccomm.param[index].flag = flag;
	ccomm.param[index].useflag |= COMMPARAMFLAG_G;
	LogParam(index, COMMPARAMFLAG_G);
	return ccomm.command;
}

bool Command::GetParamXY( int index, float * x, float * y )
{
	if (index < 0 || index >= COMMPARAMMAX)
	{
		return false;
	}
	if (x)
	{
		*x = ccomm.param[index].x;
	}
	if (y)
	{
		*y = ccomm.param[index].y;
	}
	return true;
}

float Command::GetParamF( int index )
{
	if (index < 0 || index >= COMMPARAMMAX)
	{
		return 0;
	}
	return ccomm.param[index].fval;
}

int Command::GetParamI( int index )
{
	if (index < 0 || index >= COMMPARAMMAX)
	{
		return 0;
	}
	return ccomm.param[index].ival;
}

char * Command::GetParamS( int index )
{
	if (index < 0 || index >= COMMPARAMMAX)
	{
		return NULL;
	}
	return ccomm.param[index].sval;
}

int Command::GetParamG( int index )
{
	if (index < 0 || index >= COMMPARAMMAX)
	{
		return 0;
	}
	return ccomm.param[index].flag;
}

bool Command::CheckParamSet( int index, int useflag )
{
	if (index < 0 || index >= COMMPARAMMAX)
	{
		return false;
	}
	return (ccomm.param[index].useflag & useflag);

}

int Command::ProcessCommittedCommand()
{
	if (inputcommandlist.empty())
	{
		return ccomm.command;
	}

	list<CommittedCommand>::iterator it = inputcommandlist.begin();
	if (!ccomm.command)
	{
		CommitCommand(it->substr);
	}
	if (ccomm.step == CSI_INIT)
	{
		return ccomm.command;
	}

	float fval = 0;
	float ival = 0;
	switch (ccomm.wantnext)
	{
	case COMMPARAMFLAG_X:
	case COMMPARAMFLAG_Y:
	case COMMPARAMFLAG_F:
		sscanf_s(it->substr, "%f", &fval);
		break;

	case COMMPARAMFLAG_I:
	case COMMPARAMFLAG_G:
		sscanf_s(it->substr, "%d", &ival);
		break;

	case COMMPARAMFLAG_S:
		break;
	}
	switch (ccomm.wantnext)
	{
	case COMMPARAMFLAG_X:
		SetParamX(ccomm.wantnextindex, fval);
		break;
	case COMMPARAMFLAG_Y:
		SetParamY(ccomm.wantnextindex, fval);
		break;
	case COMMPARAMFLAG_F:
		SetParamF(ccomm.wantnextindex, fval);
		break;
	case COMMPARAMFLAG_I:
		SetParamI(ccomm.wantnextindex, ival);
		break;
	case COMMPARAMFLAG_G:
		SetParamG(ccomm.wantnextindex, ival);
		break;
	case COMMPARAMFLAG_S:
		SetParamS(ccomm.wantnextindex, it->substr);
		break;
	default:
		LogError(it->substr);
		break;
	}

	inputcommandlist.pop_front();
	return ccomm.command;
}

int Command::CommitCommand( const char * str )
{
	bool notfinished = false;
	CommittedCommand _ic;
	int inext = _FindNextSubStr(str, _ic.substr, M_STRMAX);

	if (!ccomm.command)
	{
		if (!CreateCommand(FindCommandByStr(_ic.substr)))
		{
			LogError(_ic.substr);
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
	/*
	if (!ccomm.wantnext)
	{
		return ccomm.command;
	}
	*/
	while (true)
	{
		/*
		float fval = 0;
		float ival = 0;
		switch (ccomm.wantnext)
		{
		case COMMPARAMFLAG_X:
		case COMMPARAMFLAG_Y:
		case COMMPARAMFLAG_F:
			sscanf_s(commandsubstr, "%f", &fval);
			break;

		case COMMPARAMFLAG_I:
		case COMMPARAMFLAG_G:
			sscanf_s(commandsubstr, "%d", &ival);
			break;

		case COMMPARAMFLAG_S:
			break;
		}
		switch (ccomm.wantnext)
		{
		case COMMPARAMFLAG_X:
			SetParamX(ccomm.wantnextindex, fval);
			break;
		case COMMPARAMFLAG_Y:
			SetParamY(ccomm.wantnextindex, fval);
			break;
		case COMMPARAMFLAG_F:
			SetParamF(ccomm.wantnextindex, fval);
			break;
		case COMMPARAMFLAG_I:
			SetParamI(ccomm.wantnextindex, ival);
			break;
		case COMMPARAMFLAG_G:
			SetParamG(ccomm.wantnextindex, ival);
			break;
		case COMMPARAMFLAG_S:
			SetParamS(ccomm.wantnextindex, commandsubstr);
			break;
		default:
			LogError(commandsubstr);
			break;
		}
		*/
		inputcommandlist.push_back(_ic);
		if (!inext)
		{
			break;
		}
		inext = _FindNextSubStr(str, _ic.substr, M_STRMAX, inext);
	}
	return ccomm.command;
}

bool _CheckCharToBreak(char ch)
{
	if (ch >= 'a' && ch <= 'z' || ch == '_' || ch >= 'A' && ch <= 'Z' || ch >= '0' && ch <= '9' || ch >= 0x80)
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

int Command::_FindNextSubStr( const char * str, char * substr, int maxnsubstr, int ibegin/*=0*/ )
{
	if (!substr)
	{
		return 0;
	}
	strcpy_s(substr, maxnsubstr, "");
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
	strncpy_s(substr, maxnsubstr, &str[ibegin], nchars);
	int nnowsubstrlen = strlen(substr);
	if (nnowsubstrlen < maxnsubstr)
	{
		substr[nchars] = 0;
	}
	substr[maxnsubstr-1] = 0;

	for (int j=0; j<strlen(substr); j++)
	{
		substr[j] = tolower(substr[j]);
	}

	if (i == strlen(str))
	{
		return 0;
	}
	return i;
}

int Command::StepTo( int step, int wantnextindex/*=0*/, int wantnext/*=0*/, int wantnextprompt/*=0*/, bool pushback/*=true*/ )
{
	if (pushback)
	{
		histcomm.push_back(ccomm);
	}
	ccomm.step=step;
	ccomm.wantnextindex = wantnextindex;
	ccomm.wantnext = wantnext;
	ccomm.wantprompt = wantnextprompt;
	if (pushback)
	{
		LogWantNext();
	}
	return ccomm.step;
}

int Command::FindCommandByStr( const char * str )
{
	for (int i=0; i<COMMANDINDEXMAX; i++)
	{
		if (!strcmp(str, scinfo[i].str) || !strcmp(str, scinfo[i].shortstr))
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
				StepTo(it->step, it->wantnextindex, it->wantnext, it->wantprompt, false);
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