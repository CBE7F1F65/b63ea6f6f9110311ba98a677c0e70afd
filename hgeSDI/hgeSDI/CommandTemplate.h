#pragma once

#include "GUICoordinate.h"
#include "Main.h"
#include "RenderHelper.h"
#include "Command.h"
#include "GLayer.h"

class CommandTemplate
{
public:
	CommandTemplate(void);
	~CommandTemplate(void);

	virtual void OnProcessCommand()=0;
	virtual void InstantProcessCommand();
	virtual void OnDoneCommand()=0;
	virtual void CallDoneCommand();

	virtual int OnNormalProcessCommand(int cursorindex=-1);
	virtual void DispatchNormalSubCommand(int subcommand);
	virtual void UpdateLastStep();
	virtual bool IsStepped();

	virtual void PushRevertable(CommittedCommand * first, ...);
	virtual void CommitFrontCommand(CommittedCommand * first, ...);

	virtual void ProtectPendingFinishCommand();

	virtual void RenderToTarget();
	virtual void ReleaseTarget();

	virtual CommittedCommand * CCMake_C(int command, int paramcount=0, int undocount=0);
	virtual CommittedCommand * CCMake_F(float fval);
	virtual CommittedCommand * CCMake_I(int ival);
	virtual CommittedCommand * CCMake_D(int ival){return CCMake_I(ival);};
	virtual CommittedCommand * CCMake_S(const char * sval);
	virtual CommittedCommand * CCMake_Sub(int subcommand);
	virtual void DeleteCC();

	virtual bool DoUnDo(int undostep=1);
	virtual bool DoReDo(int redostep=1);

	GUICoordinate * pguic;
	MainInterface * pmain;
	Command * pcommand;
	list<CommittedCommand*> madecctodelete;

	GLayer * workingLayer;

	CommandStepInfo laststep;
};