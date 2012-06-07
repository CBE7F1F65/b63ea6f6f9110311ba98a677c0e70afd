#pragma once

#include "MainDependency.h"
#include "CommandDefines.h"

#include "GObject.h"
#include "StringManager.h"

class CommandParam
{
public:
	CommandParam()
	{
		ClearSet();
	};
	~CommandParam(){};

	void ClearSet()
	{
		useflag = 0;
		x = 0;
		y = 0;
		ival = 0;
		fval = 0;
		flag = 0;
		sval = "";
	};

	DWORD useflag;

	float x;
	float y;
	float fval;
	int ival;
	int flag;
	string sval;
};

class CommandStepInfo
{
public:
	CommandStepInfo()
	{
		ClearSet();
	};
	~CommandStepInfo(){};

	void ClearSet()
	{
		command = 0;
		step = 0;
		wantprompt = 0;
		params.clear();
		enabledsubcommand.clear();
	};
	void EnableSubCommand(int subcommand)
	{
		enabledsubcommand.push_back(subcommand);
	};
	void ClearEnabledSubCommand()
	{
		enabledsubcommand.clear();
	};

	int command;
	int step;
	int wantprompt;
	vector<CommandParam> params;
	list<int>enabledsubcommand;
};

#define	COMMITTEDCOMMANDTYPE_ERROR			0x00

#define COMMITTEDCOMMANDTYPE_SUBCOMMAND		0x01
#define COMMITTEDCOMMANDTYPE_COMMAND		0x02
#define COMMITTEDCOMMANDTYPE_COMMANDSHORT	0x04
#define F_COMMITTEDCOMMANDTYPE_COMMAND			(COMMITTEDCOMMANDTYPE_SUBCOMMAND|COMMITTEDCOMMANDTYPE_COMMAND|COMMITTEDCOMMANDTYPE_COMMANDSHORT)

#define COMMITTEDCOMMANDTYPE_FLOAT			0x10
#define COMMITTEDCOMMANDTYPE_INT			0x20
#define F_COMMITTEDCOMMANDTYPE_NUMBER			(COMMITTEDCOMMANDTYPE_FLOAT|COMMITTEDCOMMANDTYPE_INT)

#define COMMITTEDCOMMANDTYPE_STRING			0x40
#define F_COMMITTEDCOMMANDTYPE_STRING			0xff

class CommittedCommand
{
public:
	CommittedCommand()
	{
		ClearSet();
	};
	~CommittedCommand(){};

	void ClearSet()
	{
		type = 0;
		fval = 0;
		ival = 0;
		csub = 0;
		sval = "";
	};

	int type;
	float fval;
	int ival;
	int csub;
	string sval;
};

class RevertableCommand
{
public:
	RevertableCommand(){};
	~RevertableCommand(){};

	void PushCommand(CommittedCommand * cc)
	{
		if (cc)
		{
			commandlist.push_back(*cc);
		}
	};

	list<CommittedCommand> commandlist;
};

#define CUNDOREDO_NULL		0x00
#define CUNDOREDO_UNDOING	0x01
#define CUNDOREDO_REDOING	0x02

class Command
{
public:
	static Command& getInstance()
	{
		static Command instance;
		// Guaranteed to be destroyed. Instantiated on first use.
		return instance;
	}

private:
	Command();
	~Command();
	// Don't forget to declare these two.
	Command(Command const&);
	// Don't Implement
	void operator=(Command const&);
	// Don't implement

public:

	void Init();
	void Render();

	void OnInit();
private:
	int CreateCommand(int comm);
public:
	void ProcessCommand();
	void ProcessUnDoCommandCommit(RevertableCommand * rc);
	void ProcessUnDoCommandParam(int command, RevertableCommand * rc);
	int ProcessCommittedCommand();
	int ProcessPending(int index, int useflag, int fillprompt, int step, int wantprompt=0, bool pushback=true);
	void CommitFrontCommand(CommittedCommand &cc);
	void ClearReDo();

	inline bool IsCCTypeCommand(int type)
	{
		return type == COMMITTEDCOMMANDTYPE_COMMAND;
	};
	inline bool IsNormalCommand(int comm)
	{
		return comm > _COMM_NORMALCOMMANDBEGIN && comm < _COMM_NORMALCOMMANDEND;
	};
	inline bool IsInternalCommand(int comm)
	{
		return comm > _COMM_INTERNALBEGIN && comm < _COMM_INTERNALEND;
	};
	inline bool IsInternalCommand_Command(int comm)
	{
		return (comm == COMM_I_COMMAND || comm == COMM_I_COMMAND_AUTO);
	};
	inline bool IsInternalCommand_CommandAuto(int comm)
	{
		return comm == COMM_I_COMMAND_AUTO;
	};
	inline bool IsCommandNoLog(int comm)
	{
		return comm > _COMM_NOLOGCOMMANDBEGIN && comm < _COMM_NOLOGCOMMANDEND;
	};
	inline bool IsCommandPush(int comm)
	{
		return comm > _COMM_PUSHCOMMANDBEGIN && comm < _COMM_PUSHCOMMANDEND;
	};
	inline bool IsCommandInstant(int comm)
	{
		return comm > _COMM_INSTANTCOMMANDBEGIN && comm < _COMM_INSTANTCOMMANDEND;
	};

	void FinishPendingSubCommand();

	int FinishCommand();
	int TerminalCommand();

	int CommitCommand(const char * str);
	int CreateCommandCommit(int command);

	int _FindNextSubStr(const char * str, CommittedCommand * substr, int maxnsubstr, int ibegin=0);
	int FindCommandByStr(const char * str, bool * isshort=0);
	int FindSubCommandByStr(const char * str);

	void EnableSubCommand(bool bdisplay, int first, ...);
	void ClearEnabledSubCommand();

	void LogCreate();
	void LogFinish();
	void LogTerminal();
	void _LogParam(int index, int useflag, int cwp=-1);
	void LogWantNext();
	void LogError(const char * str);
	void LogDisplaySubCommandBegin();
	void LogDisplaySubCommand(int subcommand);
	void LogDisplaySubCommandEnd();
	void LogFinishSubCommand(int subcommand);
	void LogUnDo();
	void LogReDo();

	bool DoUnDo(int undostep=1);
	bool DoReDo(int redostep=1);

	bool DoUnDoCommandCommit(RevertableCommand * rc);
	bool DoUnDoCommandParam(int command, RevertableCommand * rc);
	bool DoReDoCommandSingle(RevertableCommand * rc);

	bool DoUnDoAddNode(int objid, int objparentid);
	bool DoReDoAddNode(int objid, int objparentid);
	bool DoUnDoDeleteNode(int objparentid, int objafterid);
	bool DoReDoDeleteNode(int objparentid, int objafterid);
	bool DoUnDoReparentNode(GObject * obj, GObject * oparent, GObject * aparent);
	bool DoReDoReparentNode(GObject * obj, GObject * oparent, GObject * aparent);

	bool IsUnDoReDoing(){return undoredoflag!=0;};
	bool canReDoDone();
	int undoredoflag;
	int undostepmax;

	int PushCommand();
	int PullCommand();

	void PushRevertable(RevertableCommand * rc);
	list<RevertableCommand> undolist;
	list<RevertableCommand> redolist;

protected:
	int SetCurrentCommand(CommandStepInfo * info);
	int ClearCurrentCommand(bool callterminal=false);
public:

	CommandStepInfo ccomm;
	list<CommandStepInfo> histcomm;
	list<CommandStepInfo> pushedcomm;

//	int SetParamXY(int index, float x, float y);

	void GrowParam(int index);

	int SetParamX(int index, float x, int cwp=-1);
	int SetParamY(int index, float y, int cwp=-1);
	int SetParamF(int index, float fval, int cwp=-1);
	int SetParamI(int index, int ival, int cwp=-1);
	int SetParamS(int index, const char * sval, int cwp=-1);
	int SetParamG(int index, int flag, int cwp=-1);

	bool GetParamXY(int index, float * x, float * y);
	float GetParamF(int index);
	int GetParamI(int index);
	const char * GetParamS(int index);
	int GetParamG(int index);
	bool CheckParamSet(int index, int useflag);

	int GetIvalFromRC( RevertableCommand * rc, int csp);
	float GetFvalFromRC( RevertableCommand * rc, int csp );
	const char * GetSvalFromRC( RevertableCommand * rc, int csp );
	int GetCSubFromRC( RevertableCommand * rc, int csp );

	list<CommittedCommand> inputcommandlist;
	CommittedCommand pendingparam;
	inline bool IsInternalProcessing()
	{
		return !inputcommandlist.empty();
	};
	void TerminalInternalProcess();

	inline int GetStep()
	{
		return ccomm.step;
	};

	int StepTo(int step, int wantnextprompt=0, bool pushback=true);
	int StepBack();

	inline const char * GetCommandStr(int command=-1)
	{
		if (command < 0)
		{
			command = ccomm.command;
		}
		if (command >= COMMANDINDEXMAX)
		{
			command = COMM_NULL;
		}
		return StringManager::getInstance().GetCommandStrName(command);//scinfo[command].str.c_str();
	};
	inline const char * GetCommandShortStr(int command = -1)
	{
		if (command < 0)
		{
			command = ccomm.command;
		}
		if (command >= COMMANDINDEXMAX)
		{
			command = COMM_NULL;
		}
		return StringManager::getInstance().GetCommandShortStrName(command);//scinfo[command].shortstr.c_str();
	};
	inline const char * GetCommandDescriptionStr(int command = -1)
	{
		if (command < 0)
		{
			command = ccomm.command;
		}
		if (command >= COMMANDINDEXMAX)
		{
			command = COMM_NULL;
		}
		return StringManager::getInstance().GetCommandDescriptionName(command);
	}
	inline const char * GetCommandCommentStr(int command = -1)
	{
		if (command < 0)
		{
			command = ccomm.command;
		}
		if (command >= COMMANDINDEXMAX)
		{
			command = COMM_NULL;
		}
		return StringManager::getInstance().GetCommandCommentName(command);
	}
	inline const char * GetWantPromptStr(int cwp=-1)
	{
		if (cwp < 0 || cwp >= COMMANDWANTPROMPTMAX)
		{
			cwp = ccomm.wantprompt;
		}
		return StringManager::getInstance().GetWantPromptName(cwp);//wpinfo[wantprompt].str.c_str();
	}
	inline const char * GetSubCommandStr(int ssc)
	{
		if (ssc < 0 || ssc >= COMMANDSUBINDEXMAX)
		{
			ssc = 0;
		}
		return StringManager::getInstance().GetSubCommandStrName(ssc);//subcinfo[ssc].str.c_str();
	};
	inline const char * GetSubCommandPromptStr(int ssc)
	{
		if (ssc < 0 || ssc >= COMMANDSUBINDEXMAX)
		{
			ssc = 0;
		}
		return StringManager::getInstance().GetSubCommandPromptStrName(ssc);//subcinfo[ssc].promptstr.c_str();
	};

	HTARGET tarcommand;
	float tarx;
	float tary;
	void SetRenderTarget(HTARGET tar, float x=0, float y=0);
};