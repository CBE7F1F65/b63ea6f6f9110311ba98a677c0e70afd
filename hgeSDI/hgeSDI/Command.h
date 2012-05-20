#pragma once

#include "CommandParamIndex.h"

enum{
	COMM_NULL = 0,

	_COMM_PUSHCOMMANDBEGIN,

	COMM_PAN,		// enter pan mode
	COMM_ZOOMIN,	// rect zoom
	COMM_DOZOOM,	// mouse wheel

	_COMM_PUSHCOMMANDEND,

	_COMM_INSTANTCOMMANDBEGIN,


	_COMM_INSTANTCOMMANDEND,

	COMM_LINE,

	COMM_FORCEDWORD = 0x7fffffff,
};


#define COMMPARAMFLAG_XY	0x0001
#define COMMPARAMFLAG_F		0x0002
#define COMMPARAMFLAG_I		0x0004
#define COMMPARAMFLAG_S		0x0008

struct CommandParam
{
	float x;
	float y;
	float fval;
	int ival;
	char sval[M_STRMAX];

	DWORD useflag;
};

struct CommandStepInfo
{
	int command;
	int step;
	int savedstep;
	CommandParam param[COMMPARAMMAX];
};

class Command
{
public:
	Command();
	~Command();

	static Command * getInstance();

	int CreateCommand(int comm);
	int ProcessCommand();
	int FinishCommand();

	int PushCommand();
	int PullCommand();

protected:
	int SetCurrentCommand(CommandStepInfo * info);
	int ClearCurrentCommand(bool callterminal=false);
public:

	int SetParamXY(int index, float x, float y);
	int SetParamF(int index, float fval);
	int SetParamI(int index, int ival);
	int SetParamS(int index, const char * sval);

	bool GetParamXY(int index, float * x, float * y);
	float GetParamF(int index);
	float GetParamI(int index);
	char * GetParamS(int index);

	inline int GetStep() {return ccomm.step;};
	inline int StepTo(int step) {ccomm.savedstep = ccomm.step; ccomm.step=step; return ccomm.step;};
	inline int StepBack() {return StepTo(ccomm.savedstep);};

	CommandStepInfo ccomm;
	list<CommandStepInfo> pushedcomm;
};