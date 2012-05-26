#pragma once

#include "MainDependency.h"
#include "CommandDefines.h"

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
//	char sval[M_STRMAX];
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
	};

	int command;
	int step;
	int wantprompt;
	vector<CommandParam> params;
//	CommandParam param[COMMPARAMMAX];
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
//	char str[COMMANDSTRINGMAX];
//	char shortstr[COMMANDSHORTSTRMAX];
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
//	char str[M_STRMAX];
};

enum{
	COMMITTEDCOMMANDTYPE_ERROR = 0,
	COMMITTEDCOMMANDTYPE_COMMAND,
	COMMITTEDCOMMANDTYPE_FLOAT,
	COMMITTEDCOMMANDTYPE_INT,
	COMMITTEDCOMMANDTYPE_STRING,
};

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
		sval = "";
	};

	int type;
	float fval;
	int ival;
	string sval;
};

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
	void Render();

	int CreateCommand(int comm);
	void ProcessCommand();
	int ProcessCommittedCommand();
	bool ProcessPending(int index, int useflag, int fillprompt, int step, int wantprompt=0, bool pushback=true);
	void _CommitFrontCommand(CommittedCommand &cc);
	void CommitFrontCommandF(float fval);
	void CommitFrontCommandI(int ival);
	void CommitFrontCommandS(const char * sval);
	void CommitFrontCommandC(int command);

	int FinishCommand();
	int TerminalCommand();

	int CommitCommand(const char * str);
	int _FindNextSubStr(const char * str, CommittedCommand * substr, int maxnsubstr, int ibegin=0);
	int FindCommandByStr(const char * str);

	void LogCreate();
	void LogFinish();
	void LogTerminal();
	void _LogParam(int index, int useflag, int cwp=-1);
	void LogWantNext();
	void LogError(const char * str);

	int PushCommand();
	int PullCommand();

protected:
	int SetCurrentCommand(CommandStepInfo * info);
	int ClearCurrentCommand(bool callterminal=false);
public:

	CommandStepInfo ccomm;
	list<CommandStepInfo> histcomm;
	list<CommandStepInfo> pushedcomm;
	CommandStrInfo scinfo[COMMANDINDEXMAX];
	WantPromptInfo wpinfo[COMMANDWANTPROMPTMAX];

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
		if (command < 0 || command >= COMMANDINDEXMAX)
		{
			command = ccomm.command;
		}
		return scinfo[command].str.c_str();
	};
	inline const char * GetCommandShortStr(int command = -1)
	{

		if (command < 0 || command >= COMMANDINDEXMAX)
		{
			command = ccomm.command;
		}
		return scinfo[command].shortstr.c_str();
	};

	HTARGET tarcommand;
	float tarx;
	float tary;
	void SetRenderTarget(HTARGET tar, float x=0, float y=0);
};