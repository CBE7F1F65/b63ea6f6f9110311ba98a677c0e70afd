#pragma once

#include "MainDependency.h"
#include "CommandDefines.h"

#include "GObject.h"

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

class CommandStrInfo
{
public:
	CommandStrInfo()
	{
		ClearSet();
	};
	~CommandStrInfo(){};

	void ClearSet()
	{
		str = "";
		shortstr = "";
	};

	string str;
	string shortstr;
};

class SubCommandStrInfo{

public:
	SubCommandStrInfo()
	{
		ClearSet();
	};
	~SubCommandStrInfo(){};

	void ClearSet()
	{
		promptstr = "";
		str = "";
	};

	string promptstr;
	string str;
};

class WantPromptInfo
{
public:
	WantPromptInfo()
	{
		ClearSet();
	};
	~WantPromptInfo(){};

	void ClearSet()
	{
		str = "";
	};

	string str;
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
	void InitCommandStrInfo();
	void InitWantPromptInfo();
	void InitSubCommandStrInfo();
	void Render();

	int CreateCommand(int comm);
	void ProcessCommand();
	void ProcessUnDoCommand(RevertableCommand * rc);
	int ProcessCommittedCommand();
	int ProcessPending(int index, int useflag, int fillprompt, int step, int wantprompt=0, bool pushback=true);
	void CommitFrontCommand(CommittedCommand &cc);

	void FinishPendingSubCommand();

	int FinishCommand();
	int TerminalCommand();

	int CommitCommand(const char * str);
	int _FindNextSubStr(const char * str, CommittedCommand * substr, int maxnsubstr, int ibegin=0);
	int FindCommandByStr(const char * str, bool * isshort=0);
	int FindSubCommandByStr(const char * str);

	void EnableSubCommand(int first, ...);
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

	bool DoUnDo(int undostep=1);
	bool DoReDo(int redostep=1);

	bool DoUnDoCommandSingle(RevertableCommand * rc);
	bool DoReDoCommandSingle(RevertableCommand * rc);

	bool DoUnDoAddNode(GObject * obj, GObject * parent);
	bool DoReDoAddNode(GObject * obj, GObject * parent);
	bool DoUnDoDeleteNode(GObject * obj, GObject * parent);
	bool DoReDoDeleteNode(GObject * obj, GObject * parent);
	bool DoUnDoReparentNode(GObject * obj, GObject * oparent, GObject * aparent);
	bool DoReDoReparentNode(GObject * obj, GObject * oparent, GObject * aparent);

	bool IsUnDoReDoing(){return undoredoflag!=0;};
	int undoredoflag;

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
	CommandStrInfo scinfo[COMMANDINDEXMAX];
	WantPromptInfo wpinfo[COMMANDWANTPROMPTMAX];
	SubCommandStrInfo subcinfo[COMMANDSUBINDEXMAX];

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

	inline const char * GetWantPromptStr(int wantprompt=-1)
	{
		if (wantprompt < 0 || wantprompt >= COMMANDWANTPROMPTMAX)
		{
			wantprompt = ccomm.wantprompt;
		}
		return wpinfo[wantprompt].str.c_str();
	}
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
		return scinfo[command].str.c_str();
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
		return scinfo[command].shortstr.c_str();
	};
	inline const char * GetSubCommandStr(int subcommand)
	{
		if (subcommand < 0 || subcommand >= COMMANDSUBINDEXMAX)
		{
			subcommand = 0;
		}
		return subcinfo[subcommand].str.c_str();
	};
	inline const char * GetSubCommandPromptStr(int subcommand)
	{
		if (subcommand < 0 || subcommand >= COMMANDSUBINDEXMAX)
		{
			subcommand = 0;
		}
		return subcinfo[subcommand].promptstr.c_str();
	};

	HTARGET tarcommand;
	float tarx;
	float tary;
	void SetRenderTarget(HTARGET tar, float x=0, float y=0);
};