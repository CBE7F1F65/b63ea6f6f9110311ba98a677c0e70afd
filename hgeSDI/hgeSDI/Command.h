#pragma once

#include "MainDependency.h"
#include "CommandDefines.h"

struct CommandParam
{
	DWORD useflag;

	float x;
	float y;
	float fval;
	int ival;
	int flag;
	char sval[M_STRMAX];
};

struct CommandStepInfo
{
	int command;
	int step;
	int wantnext;
	int wantnextindex;
	int wantprompt;
	CommandParam param[COMMPARAMMAX];
};

struct CommandStrInfo
{
	char str[COMMANDSTRINGMAX];
	char shortstr[COMMANDSHORTSTRMAX];
};

struct WantPromptInfo
{
	char str[M_STRMAX];
};

struct CommittedCommand
{
	char substr[M_STRMAX];
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
	int FinishCommand();
	int TerminalCommand();

	int CommitCommand(const char * str);
	int _FindNextSubStr(const char * str, char * substr, int maxnsubstr, int ibegin=0);
	int FindCommandByStr(const char * str);

	void LogCreate();
	void LogFinish();
	void LogTerminal();
	void LogParam(int index, int useflag);
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
	int SetParamX(int index, float x);
	int SetParamY(int index, float y);
	int SetParamF(int index, float fval);
	int SetParamI(int index, int ival);
	int SetParamS(int index, const char * sval);
	int SetParamG(int index, int flag);

	bool GetParamXY(int index, float * x, float * y);
	float GetParamF(int index);
	int GetParamI(int index);
	char * GetParamS(int index);
	int GetParamG(int index);
	bool CheckParamSet(int index, int useflag);

	list<CommittedCommand> inputcommandlist;
	inline bool IsInternalProcessing()
	{
		return !inputcommandlist.empty();
	};

	inline int GetStep() 
	{
		return ccomm.step;
	};

	int StepTo(int step, int wantnextindex=0, int wantnext=0, int wantnextprompt=0, bool pushback=true);
	int StepBack();

	inline const char * GetWantPromptStr(int wantprompt=-1)
	{
		if (wantprompt < 0 || wantprompt >= COMMANDWANTPROMPTMAX)
		{
			wantprompt = ccomm.wantprompt;
		}
		return wpinfo[wantprompt].str;
	}
	inline const char * GetCommandStr(int command=-1)
	{
		if (command < 0 || command >= COMMANDINDEXMAX)
		{
			command = ccomm.command;
		}
		return scinfo[command].str;
	};
	inline const char * GetCommandShortStr(int command = -1)
	{

		if (command < 0 || command >= COMMANDINDEXMAX)
		{
			command = ccomm.command;
		}
		return scinfo[command].shortstr;
	};

	HTARGET tarcommand;
	float tarx;
	float tary;
	void SetRenderTarget(HTARGET tar, float x=0, float y=0);
};