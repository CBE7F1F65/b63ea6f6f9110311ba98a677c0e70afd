#pragma once

#include "GObjectPicker.h"
#include "CommandTemplate.h"

class CommandState;
class CommandSubStateTemplate;
class CommandMap;

class FillCommandParamInfo
{
public:
	FillCommandParamInfo(){};
	FillCommandParamInfo(int _step, int _index, int _useflag, int _fillprompt, int _tostep, int _wantprompt=0, bool _pushback=true)
	{
		step = _step;
		index = _index;
		useflag = _useflag;
		fillprompt = _fillprompt;
		tostep = _tostep;
		wantprompt = _wantprompt;
		pushback = _pushback;
	};
	~FillCommandParamInfo(){};

	int step;
	int index;
	int useflag;
	int fillprompt;
	int tostep;
	int wantprompt;
	bool pushback;
};


class CommandSubStateTemplate
{
public:
	CommandSubStateTemplate();
	~CommandSubStateTemplate();

	void SetSubCommandIndex(int index=-1);
	void SetToFCP(FillCommandParamInfo * tofcpinfo){tofcp=*tofcpinfo;};

	void SetState(CommandState * _pState){pState=_pState;};

	virtual void Clear();
	virtual bool RequirePickPoint(){return false;};
	virtual bool SetPickParam( int iret, bool bReady ){return false;};

	virtual bool ExcuteSubState() = 0;

	bool CallExcuteSubState( int nowsubstate );

	CommandState * getState(){return pState;};
	CommandMap * getMap();
	CommandTemplate * getCommandTemplate();
	int subindex;
	CommandState *pState;
	FillCommandParamInfo tofcp;
};

class CommandState
{
public:
	CommandState(FillCommandParamInfo * fcpinfo);
	~CommandState();

	void AddSubState(CommandSubStateTemplate * substate);
	void Clear();
	void SetPickFilterCallback(PickFilterCallback pfcb);
	bool ExecuteState(int step);
	void SetMap(CommandMap * _pMap){pMap=_pMap;};

	bool EnableSubCommand(int ssc);
	bool DisableSubCommand(int ssc);

	CommandTemplate * getCommandTemplate();
	CommandMap * getMap(){return pMap;};
	void FlushSubCommand(int step);

	list<CommandSubStateTemplate *> substatelist;
	list<int> subcommandlist;
	FillCommandParamInfo fcp;
	bool bPickPoint;
	PickFilterCallback pfcb;
	CommandMap * pMap;
	int nowsubstate;

};

class CommandMap
{
public:
	CommandMap(void);
	~CommandMap(void);

	void AddState(CommandState * state);
	void Clear();

	CommandTemplate * getCommandTemplate(){return pct;};
	void SetCommandTemplate(CommandTemplate * pct);
	bool ExecuteMap(int step, int nowstep);

	list<CommandState *> statelist;
	CommandTemplate * pct;
	int laststep;
};

