#pragma once

#include "GUICoordinate.h"
#include "MainInterface.h"
#include "RenderHelper.h"
#include "Command.h"
#include "GLayer.h"
#include "GObjectManager.h"
#include "GObjectPicker.h"
#include "RenderHelper.h"

enum{
	PUSHREVERTABLESTATE_BEGIN,
	PUSHREVERTABLESTATE_CONTINUE,
	PUSHREVERTABLESTATE_END,
};


class CommandTemplate
{
public:
	CommandTemplate(void);
	virtual ~CommandTemplate(void);

	void CallProcessCommand();
	virtual void OnProcessCommand()=0;
	void InstantProcessCommand();
	virtual void OnDoneCommand()=0;
	void CallDoneCommand();
	virtual void OnProcessUnDoCommand(RevertableCommand * rc);
	void CallProcessUnDoCommand(int comm, RevertableCommand * rc);

	virtual int OnNormalProcessCommand(int cursorindex=-1);
	void DispatchNormalSubCommand(int subcommand);
	void UpdateLastStep();
	bool IsStepped();

	void PushRevertable(CommittedCommand * first, ...);

	RevertableCommand rcbatch;
	void PushRevertableBatch(int revertstate, CommittedCommand * first, ...);

	void CommitFrontCommand(CommittedCommand * first, ...);

	void ProtectPendingFinishCommand();

	virtual void RenderToTarget();
	void ReleaseTarget();

	CommittedCommand * CCMake_C(int command, int paramcount=0, int undocount=0);
	CommittedCommand * CCMake_CI(int command, int ival);
	CommittedCommand * CCMake_F(float fval);
	CommittedCommand * CCMake_I(int ival);
	CommittedCommand * CCMake_D(int ival){return CCMake_I(ival);};
	CommittedCommand * CCMake_S(const char * sval);
	CommittedCommand * CCMake_Sub(int subcommand);
	void DeleteCC();

	bool DoUnDo(int undostep=1);
	bool DoReDo(int redostep=1);

	GUICoordinate * pguic;
	MainInterface * pmain;
	Command * pcommand;
	GObjectManager * pgm;
	GObjectPicker * pgp;
	RenderHelper * prh;
	list<CommittedCommand*> madecctodelete;

	int comm;


	CommandStepInfo laststep;

	static CommandTemplate * GetTemplateByCommand(int comm);

	int workinglayerID;
	static void CallOnUnDo();
};
