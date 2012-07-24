#include "stdafx.h"
#include "Command.h"

#include "MainInterface.h"
#include "RenderHelper.h"
#include <sstream>
#include <iomanip>
#include "CommandTemplate.h"
#include "SnapshotManager.h"

Command::Command()
{
//	ZeroMemory(&ccomm, sizeof(ccomm));
	ccomm.ClearSet();
	tarCommand = 0;
	undoredoflag = 0;
	undostepmax = INID_UNDOMAX;
	Init();
}

Command::~Command()
{
}

void Command::Init()
{
	TerminalInternalProcess();
	ClearCurrentCommand();
	SetRenderTarget(0);
}

void Command::LogCreate()
{
	if (!IsCommandNoLog(GetCurrentCommand()))
	{
		string strlog;
//		sprintf_s(strlog, M_STRMAX, "%s: %s"
		strlog = StringManager::getInstance().GetCLCreateCommandName();
		strlog += ": ";
		strlog += GetCommandStr();
		MainInterface::getInstance().CallAppendCommandLogText(strlog.c_str());
	}
}

void Command::LogFinish()
{
	if (!IsCommandNoLog(GetCurrentCommand()))
	{
		string strlog;
//		sprintf_s(strlog, M_STRMAX, "%s: %s"
		strlog = StringManager::getInstance().GetCLFinishCommandName();
		strlog += ": ";
		strlog += GetCommandStr();
		MainInterface::getInstance().CallAppendCommandLogText(strlog.c_str());
	}
}

void Command::LogTerminal()
{
	if (!IsCommandNoLog(GetCurrentCommand()))
	{
		string strlog;
//		sprintf_s(strlog, M_STRMAX, "%s: %s"
		strlog = StringManager::getInstance().GetCLTerminalCommandName();
		strlog += ": ";
		strlog += GetCommandStr();
		MainInterface::getInstance().CallAppendCommandLogText(strlog.c_str());
	}
}

void Command::_LogParam( int index, int useflag, int cwp/*=-1 */ )
{
	if (!IsCommandNoLog(GetCurrentCommand()))
	{
//		char strlog[M_STRMAX*4];
//		char paramstr[M_STRMAX];
		string strlog;
		stringstream ss;

		switch (useflag)
		{
		case COMMPARAMFLAG_X:
			ss << fixed << ccomm.params[index].x;
//			sprintf_s(paramstr, M_STRMAX, "%f", ccomm.params[index].x);
			break;
		case COMMPARAMFLAG_Y:
			ss << fixed << ccomm.params[index].y;
//			sprintf_s(paramstr, M_STRMAX, "%f", ccomm.params[index].y);
			break;
		case COMMPARAMFLAG_F:
			ss << fixed << ccomm.params[index].fval;
//			sprintf_s(paramstr, M_STRMAX, "%f", ccomm.params[index].fval);
			break;
		case COMMPARAMFLAG_I:
			ss << ccomm.params[index].ival;
//			sprintf_s(paramstr, M_STRMAX, "%d", ccomm.params[index].ival);
			break;
		case COMMPARAMFLAG_G:
			ss << ccomm.params[index].flag;
//			sprintf_s(paramstr, M_STRMAX, "%d", ccomm.params[index].flag);
			break;
		case COMMPARAMFLAG_S:
			ss << ccomm.params[index].sval;
			break;
		}

//		sprintf_s(strlog, M_STRMAX*4, "(%s) %s: %s: %s",
		strlog = "(";
		strlog += GetCommandStr();
		strlog += ") ";
		strlog += StringManager::getInstance().GetCLSetParameterName();
		strlog += ": ";
		strlog += GetWantPromptStr(cwp);
		strlog += ": ";
		strlog += ss.str();
		MainInterface::getInstance().CallAppendCommandLogText(strlog.c_str());
	}
}

void Command::LogWantNext()
{
	if (!ccomm.wantprompt || IsInternalProcessing())
	{
		return;
	}
	if (!IsCommandNoLog(GetCurrentCommand()))
	{
		string strlog;
//		sprintf_s(strlog, M_STRMAX, "%s: %s: %s: ",
		strlog = GetCommandStr();
		strlog += ": ";
		strlog += StringManager::getInstance().GetCLNextPromptName();
		strlog += ": ";
		strlog += GetWantPromptStr();
		MainInterface::getInstance().CallAppendCommandLogText(strlog.c_str());

		if (!ccomm.enabledsubcommand.empty())
		{
			LogDisplaySubCommandBegin();
			for (list<int>::iterator it=ccomm.enabledsubcommand.begin(); it!=ccomm.enabledsubcommand.end(); ++it)
			{
				LogDisplaySubCommand(*it);
			}
			LogDisplaySubCommandEnd();
		}
	}
}

void Command::LogDisplaySubCommandBegin()
{
	MainInterface::getInstance().CallAppendCommandLogText(" [/", false);
}

void Command::LogDisplaySubCommand( int subcommand )
{
	string strlog;
	strlog = GetSubCommandPromptStr(subcommand);
	strlog += "/";
	MainInterface::getInstance().CallAppendCommandLogText(strlog.c_str(), false);
}

void Command::LogDisplaySubCommandEnd()
{
	MainInterface::getInstance().CallAppendCommandLogText("]", false);
}

void Command::LogFinishSubCommand( int subcommand )
{
	string strlog;
	strlog = StringManager::getInstance().GetCLFinishSubCommandName();
	strlog += ": ";
	strlog += GetSubCommandPromptStr(subcommand);
	MainInterface::getInstance().CallAppendCommandLogText(strlog.c_str());
//	char strlog[M_STRMAX];
//	sprintf_s(strlog, M_STRMAX, "%s: %s", StringManager::getInstance().GetCLFinishSubCommandName(), GetSubCommandPromptStr(subcommand));
//	MainInterface::getInstance().CallAppendCommandLogText(strlog);
}

void Command::LogError( const char * str )
{
	string strlog;
//	char strlog[M_STRMAX*4];
	if (GetCurrentCommand() && ccomm.wantprompt)
	{
		strlog = "(";
		strlog += GetCommandStr();
		strlog += ") ";
		strlog += StringManager::getInstance().GetCLErrorName();
		strlog += ": ";
		strlog += GetWantPromptStr();
		strlog += ": ";
		strlog += str;
//		sprintf_s(strlog, M_STRMAX*4, "(%s) %s: %s: %s", GetCommandStr(), StringManager::getInstance().GetCLErrorName(), GetWantPromptStr(), str);
	}
	else
	{
		strlog = StringManager::getInstance().GetCLErrorName();
		strlog += ": ";
		strlog += str;
//		sprintf_s(strlog, M_STRMAX*4, "%s: %s", StringManager::getInstance().GetCLErrorName(), str);
	}
	MainInterface::getInstance().CallAppendCommandLogText(strlog.c_str());
//	MainInterface::getInstance().CallAppendCommandLogText(strlog);
}

void Command::LogUnDo()
{
	string strlog;
	strlog = StringManager::getInstance().GetCLUnDoName();
	MainInterface::getInstance().CallAppendCommandLogText(strlog.c_str());
}

void Command::LogReDo()
{
	string strlog;
	strlog = StringManager::getInstance().GetCLReDoName();
	MainInterface::getInstance().CallAppendCommandLogText(strlog.c_str());
}

int Command::CreateCommand( int comm, int iparam/*=1*/ )
{
	if (comm == COMM_UNDO)
	{
		DoUnDo(iparam);
		return GetCurrentCommand();
	}
	else if (comm == COMM_REDO)
	{
		DoReDo(iparam);
		return GetCurrentCommand();
	}
	if (IsCommandPush(comm))
	{
		PushCommand();
	}
	/*
	if (IsCommandInstant(comm))
	{
	}
	else
	{
	*/
		ClearCurrentCommand(true);
		ccomm.command = comm;
//	}
	LogCreate();
	return GetCurrentCommand();
}

int Command::FinishCommand()
{
	LogFinish();

	for (list<CommandStepInfo>::iterator it = histcomm.begin(); it!=histcomm.end();)
	{
		if (it->command == GetCurrentCommand())
		{
			it = histcomm.erase(it);
		}
		else
		{
			++it;
		}
	}
	if (IsCommandPush(GetCurrentCommand()))
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
	CommandTemplate::GetTemplateByCommand(GetCurrentCommand())->CallTerminalCommand();
	LogTerminal();

	for (list<CommandStepInfo>::iterator it = histcomm.begin(); it!=histcomm.end();)
	{
		if (it->command == GetCurrentCommand())
		{
			it = histcomm.erase(it);
		}
		else
		{
			++it;
		}
	}
	if (IsCommandPush(GetCurrentCommand()))
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
	if (GetCurrentCommand())
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
	if (GetCurrentCommand())
	{
		StepTo(CSI_RESUME);
		ProcessCommand();
		StepBack();
	}
	return GetCurrentCommand();
}

int Command::ClearCurrentCommand(bool callterminal /*=false*/)
{
//	inputcommandlist.clear();
	pendingparam.ClearSet();
	if (GetCurrentCommand() && callterminal)
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
	return GetCurrentCommand();
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
	return GetCurrentCommand();
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
	return GetCurrentCommand();
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
	return GetCurrentCommand();
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
	return GetCurrentCommand();
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
	return GetCurrentCommand();
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
	return GetCurrentCommand();
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

int Command::GetIvalFromRC( RevertableCommand * rc, int csp )
{
	if (rc)
	{
		if ((int)(rc->GetSize()) >= csp)
		{
			list<CommittedCommand>::reverse_iterator it = rc->commandlist.rbegin();
			for (int i=0; i<csp; i++)
			{
				++it;
			}
			return it->ival;
		}
	}
	ASSERT(false);
	return 0;
}

float Command::GetFvalFromRC( RevertableCommand * rc, int csp )
{
	if (rc)
	{
		if ((int)(rc->GetSize()) >= csp)
		{
			list<CommittedCommand>::reverse_iterator it = rc->commandlist.rbegin();
			for (int i=0; i<csp; i++)
			{
				++it;
			}
			return it->fval;
		}
	}
	ASSERT(false);
	return 0;

}

const char * Command::GetSvalFromRC( RevertableCommand * rc, int csp )
{
	if (rc)
	{
		if ((int)(rc->GetSize()) >= csp)
		{
			list<CommittedCommand>::reverse_iterator it = rc->commandlist.rbegin();
			for (int i=0; i<csp; i++)
			{
				++it;
			}
			return it->sval.c_str();
		}
	}
	ASSERT(false);
	return "";

}

int Command::GetCSubFromRC( RevertableCommand * rc, int csp )
{
	if (rc)
	{
		if ((int)(rc->GetSize()) >= csp)
		{
			list<CommittedCommand>::reverse_iterator it = rc->commandlist.rbegin();
			for (int i=0; i<csp; i++)
			{
				++it;
			}
			return it->csub;
		}
	}
	ASSERT(false);
	return 0;

}

int Command::CommitCommand( const char * str )
{
	bool notfinished = false;
	CommittedCommand _cc;
	int inext = 0;
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
	return GetCurrentCommand();
}

int Command::CreateCommandCommit( int command, int iparam/*=1*/ )
{
	/*
	CommittedCommand _cc;
	_cc.type = COMMITTEDCOMMANDTYPE_COMMAND;
	_cc.ival = command;
	_cc.sval = StringManager::getInstance().GetCommandStrName(command);
	inputcommandlist.push_back(_cc);
	*/
	return CreateCommand(command, iparam);
//	return GetCurrentCommand();
}

bool _CheckCharToBreak(char ch)
{
	if (ch >= 'a' && ch <= 'z' || ch == '\"' || ch == '\'' || ch == '_' || ch >= 'A' && ch <= 'Z' || ch >= '0' && ch <= '9' || ch == '-' || ch == '+' || ch == '.' || ch == ';' || ch < 0)
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
	for (; i<(int)strlen(str); i++)
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
	for (; i<(int)strlen(str); i++)
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
		if ((int)substr.length() > i)
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
		for (int i=0; i<(int)substr.length(); i++)
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
		if (checkquote)
		{
			cc->type = COMMITTEDCOMMANDTYPE_STRING;
		}
	}
	if (!cc->type)
	{
		bool isshort;
		cc->ival = FindCommandByStr(substr.c_str(), &isshort);
		if (cc->ival)
		{
			cc->type |= COMMITTEDCOMMANDTYPE_COMMAND;
			if (isshort)
			{
				cc->type |= COMMITTEDCOMMANDTYPE_COMMANDSHORT;
			}
		}
		cc->csub = FindSubCommandByStr(substr.c_str());
		if (cc->csub)
		{
			cc->type |= COMMITTEDCOMMANDTYPE_SUBCOMMAND;
		}
	}
	if (!cc->type)
	{
		cc->type = COMMITTEDCOMMANDTYPE_STRING;
	}

	switch (cc->type)
	{
	case COMMITTEDCOMMANDTYPE_ERROR:
	/*
	case COMMITTEDCOMMANDTYPE_COMMAND:
	case COMMITTEDCOMMANDTYPE_SUBCOMMAND:
	case COMMITTEDCOMMANDTYPE_COMMAND|COMMITTEDCOMMANDTYPE_SUBCOMMAND:
	*/
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

	if (i >= (int)strlen(str))
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
	ccomm.enabledsubcommand.clear();
	if (pushback)
	{
		LogWantNext();
	}
	return ccomm.step;
}

int Command::FindCommandByStr( const char * str, bool * isshort/*=0*/ )
{
	string upperstr = str;
	transform(upperstr.begin(), upperstr.end(), upperstr.begin(), toupper);

	StringManager * psm = &StringManager::getInstance();

	for (int i=0; i<_COMM_ENDINDEX; i++)
	{
		if (!upperstr.compare(psm->GetCommandStrName(i))/*!scinfo[i].str.compare(upperstr)*/)
		{
			if (isshort)
			{
				*isshort = false;
			}
			return i;
		}
		if (!upperstr.compare(psm->GetCommandShortStrName(i))/*!scinfo[i].shortstr.compare(upperstr)*/)
		{
			if (isshort)
			{
				*isshort = true;
			}
			return i;
		}
	}
	return 0;
}

int Command::FindSubCommandByStr( const char * str )
{
	if (!GetCurrentCommand() || ccomm.enabledsubcommand.empty())
	{
		return 0;
	}

	StringManager * psm = &StringManager::getInstance();

	string upperstr = str;
	transform(upperstr.begin(), upperstr.end(), upperstr.begin(), toupper);

	for (list<int>::iterator it=ccomm.enabledsubcommand.begin(); it!=ccomm.enabledsubcommand.end(); ++it)
	{
		if (!upperstr.compare(psm->GetSubCommandStrName((*it)))/*!subcinfo[(*it)].str.compare(upperstr)*/)
		{
			return *it;
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
			if (it->command == GetCurrentCommand())
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
	RenderHelper::getInstance().TargetQuadRender_S(tarCommand, tarx, tary, 0xffffffff);
}

void Command::SetRenderTarget( HTARGET tar, float x/*=0*/, float y/*=0*/ )
{
	tarCommand = tar;
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

void Command::CommitFrontCommand( CommittedCommand &cc )
{
	inputcommandlist.push_front(cc);
}

void Command::TerminalInternalProcess()
{
	inputcommandlist.clear();
}

void Command::EnableSubCommand( bool bdisplay, int first, ... )
{
	if (!GetCurrentCommand())
	{
		return;
	}

	if (!first)
	{
		return;
	}

	list<int> sublist;

	va_list ap;
	va_start(ap, first);
	int vai = first;

	while (vai)
	{
		sublist.push_back(vai);
		vai = (int)va_arg(ap, int);
	}
	va_end(ap);

	EnableSubCommand(bdisplay, &sublist);
}

void Command::EnableSubCommand( bool bdisplay, list<int> * sublist )
{
	if (!GetCurrentCommand())
	{
		return;
	}

	ASSERT(sublist);
	if (sublist->empty())
	{
		return;
	}
	
	if (pendingparam.type)
	{
		bdisplay = false;
	}
	if (ccomm.enabledsubcommand.empty())
	{
		if (bdisplay)
		{
			LogDisplaySubCommandBegin();
		}
	}
	for (list<int>::iterator it=sublist->begin(); it!=sublist->end(); ++it)
	{
		ccomm.EnableSubCommand(*it);
		if (bdisplay)
		{
			LogDisplaySubCommand(*it);
		}
	}

	if (bdisplay)
	{
		LogDisplaySubCommandEnd();
	}

}
void Command::ClearEnabledSubCommand()
{
	if (GetCurrentCommand())
	{
		ccomm.ClearEnabledSubCommand();
	}
//	enabledsubcommand.clear();
}

void Command::FinishPendingSubCommand()
{
	if (pendingparam.type)
	{
		LogFinishSubCommand(pendingparam.csub);
		pendingparam.ClearSet();
		LogWantNext();
	}
}

void Command::DoPushRevertable()
{
	RevertableCommand * rc = &rcbuffer;
	if (!rc->GetSize())
	{
		return;
	}
	if (rc)
	{
		/*
		list<RevertableCommand> rclist;
		RevertableCommand rv;
		rclist.push_front(rv);
		for (list<CommittedCommand>::iterator it=rc->commandlist.begin(); it!=rc->commandlist.end(); ++it)
		{

			if (IsCCTypeCommand(it->type) && IsInternalCommand_CommandEndMark(it->ival))
			{
				RevertableCommand rvt;
				rclist.push_front(rvt);
			}
			else
			{
				rclist.front().PushCommand(&(*it));
			}
		}
		if (rclist.front().commandlist.empty())
		{
			rclist.pop_front();
		}
		RevertableCommand rvundo;
		for (list<RevertableCommand>::iterator it=rclist.begin(); it!=rclist.end(); ++it)
		{
			for (list<CommittedCommand>::iterator jt=it->commandlist.begin(); jt!=it->commandlist.end(); ++jt)
			{
				rvundo.PushCommand(&(*jt));
			}
		}
		undolist.push_back(rvundo);
		*/
		undolist.push_back(*rc);
		while ((int)undolist.size() > undostepmax)
		{
			undolist.pop_front();
			SnapshotManager::getInstance().OnDeleteUnDo(undostepmax);
			MainInterface::getInstance().OnClearPreviousHistory();
		}
	}
	if (!IsUnDoReDoing())
	{
		ClearReDo();

		string strcomm;
		bool pushed=false;
		int firstcomm = 0;
		int firstusefulcomm = 0;
		bool multicomm = false;

		for (list<CommittedCommand>::iterator it=rc->commandlist.begin(); it!= rc->commandlist.end(); ++it)
		{
			if (IsCCTypeCommand(it->type) && IsInternalCommand_Command(it->ival))
			{
				pushed = true;
				int pcount = CI_GETPCOUNT(it->csub);
				int ucount = CI_GETUCOUNT(it->csub);
				++it;	// working layer
				if (pcount)
				{
					int comm=0;
					for (int i=0; i<pcount; i++)
					{
						++it;
						strcomm += it->sval;
						if (!i)
						{
							comm = it->ival;
							strcomm += "(";
						}
						else if (i < pcount-1)
						{
							strcomm += ", ";
						}
					}
					strcomm += "); ";
					if (!firstcomm)
					{
						firstcomm = comm;
					}
					if (comm != COMM_SETWORKINGLAYER)
					{
						if (!firstusefulcomm)
						{
							firstusefulcomm = comm;
						}
						else
						{
							multicomm = true;
						}
					}
				}
			}
		}
		if (pushed)
		{
			SnapshotManager::getInstance().OnPushRevertable();
			int comm = firstusefulcomm?firstusefulcomm:firstcomm;
			string descstr = GetCommandDescriptionStr(comm);
			if (multicomm)
			{
				descstr += "...";
			}
			MainInterface::getInstance().OnPushRevertable(descstr.c_str(), strcomm.c_str(), comm);
		}
	}
	if (undoredoflag == CUNDOREDO_REDOING)
	{
		ExitReDo();
	}
	rcbuffer.Clear();
}

void Command::PushRevertable( RevertableCommand * rc )
{
	if (rc)
	{
		for (list<CommittedCommand>::iterator it=rc->commandlist.begin(); it!=rc->commandlist.end(); ++it)
		{
			rcbuffer.PushCommand(&(*it));
		}
		rcbuffer.PushEndMark();
	}

}

void Command::ClearReDo()
{
	int ntodelete = redolist.size();
	if (ntodelete)
	{
		redolist.clear();
		SnapshotManager::getInstance().OnClearReDo(ntodelete);
		MainInterface::getInstance().OnClearReDo(ntodelete);
	}
}

void Command::ClearUnDo()
{
	int ntodelete = undolist.size();
	if (ntodelete)
	{
		undolist.clear();
		SnapshotManager::getInstance().OnClearUnDo(ntodelete);
		MainInterface::getInstance().OnClearUnDo(ntodelete);
	}
//	CreateCommandCommit(COMM_INITIAL);
}

void Command::OnInit()
{
	HGE * hge = MainInterface::getInstance().hge;
	undostepmax = hge->Ini_GetInt(INIS_PERFORMANCE, ININ_UNDOMAX, INID_UNDOMAX);
	if (undostepmax <= 0)
	{
		undostepmax = INID_UNDOMAX;
	}

	CreateCommand(COMM_INITIAL);
}

bool Command::canCommandDone()
{
	if (!GetCurrentCommand() || ccomm.step == CSI_FINISH || ccomm.step == CSI_FINISHCONTINUE || ccomm.step == CSI_TERMINAL)
	{
		return true;
	}
	return false;
}

int Command::CreateUnDoCommandCommit( int step/*=1*/ )
{
	ASSERT(step>=0);
// 	for (int i=0; i<step; i++)
// 	{
// 		CreateCommandCommit(COMM_UNDO);
// 	}
// 	CommittedCommand cc;
// 	cc.ival = step;
// 	stringstream ss;
// 	ss<<step;
// 	cc.sval = ss.str();
// 	CommitFrontCommand(cc);
	CreateCommandCommit(COMM_UNDO, step);
	return GetCurrentCommand();
}

int Command::CreateReDoCommandCommit( int step/*=1*/ )
{
	ASSERT(step>=0);
// 	for (int i=0; i<step; i++)
// 	{
// 		CreateCommandCommit(COMM_REDO);
// 	}
// 	CommittedCommand cc;
// 	cc.ival = step;
// 	stringstream ss;
// 	ss<<step;
// 	cc.sval = ss.str();
// 	CommitFrontCommand(cc);
	CreateCommandCommit(COMM_REDO, step);
	return GetCurrentCommand();
}

int Command::GetCurrentCommand()
{
	return ccomm.command;
}

bool CommittedCommand::TranslateObjToID()
{
	if (type != COMMITTEDCOMMANDTYPE_OBJ)
	{
		return false;
	}
	type = COMMITTEDCOMMANDTYPE_INT;
	GObject * pObj = (GObject *)ival;
	if (pObj)
	{
		ival = pObj->getID();
	}
	else
	{
		ival = -1;
	}
	fval = ival;
	stringstream ss;
	ss << ival;
	sval = ss.str();
	return true;
}