#include "StdAfx.h"
#include "BindHandleCommand.h"


BindHandleCommand::BindHandleCommand(void)
{
}


BindHandleCommand::~BindHandleCommand(void)
{
}

void BindHandleCommand::OnProcessCommand()
{
	int step = OnNormalProcessCommand();
	int nowstep = pcommand->GetStep();
	if (IsStepped())
	{
		if (nowstep > CSI_INIT)
		{
			pcommand->EnableSubCommand(
				(laststep.step==CSI_RESUME)?false:true,
				SSC_TERMINAL,
				SSC_NULL);
		}
	}
	UpdateLastStep();

	int ret = -1;

	if (step == CSI_INIT)
	{
		pcommand->StepTo(
            CSI_BINDHANDLE_WANTFROMINDEX,
			CWP_INDEX);
	}
	else if (step == CSI_BINDHANDLE_WANTFROMINDEX)
	{
		ret = pcommand->ProcessPending(
			CSP_BINDHANDLE_I_FROMINDEX, COMMPARAMFLAG_I, CWP_INDEX,
			CSI_BINDHANDLE_WANTTOINDEX
			);
	}
	else if (step == CSI_BINDHANDLE_WANTTOINDEX)
	{
		ret = pcommand->ProcessPending(
			CSP_BINDHANDLE_I_TOINDEX, COMMPARAMFLAG_I, CWP_INDEX,
			CSI_FINISH
			);
	}

	if (ret > 0)
	{
		DispatchNormalSubCommand(ret);
		pcommand->FinishPendingSubCommand();
	}
}

void BindHandleCommand::OnDoneCommand()
{
	int fromindex = pcommand->GetParamI(CSP_BINDHANDLE_I_FROMINDEX);
	int toindex = pcommand->GetParamI(CSP_BINDHANDLE_I_TOINDEX);

	GObject * pFromObj = pgm->FindObjectByID(fromindex);
	GObject * pToObj = pgm->FindObjectByID(toindex);
    if (!pFromObj)
	{
		return;
	}
    if (!pFromObj->isHandlePoint() || (pToObj && !pToObj->isHandlePoint()))
	{
		return;
	}
	GHandlePoint * pFromHandlePoint = (GHandlePoint *)pFromObj;
	GHandlePoint * pToHandlePoint = (GHandlePoint *)pToObj;

    if (pFromHandlePoint->isBindWith(pToHandlePoint))
	{
		return;
	}
    pFromHandlePoint->BindWith(pToHandlePoint);

	PushRevertable(
		CCMake_C(COMM_I_COMMAND, 3, 1),
		CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
		CCMake_C(COMM_BINDHANDLE),
		CCMake_I(fromindex),
		CCMake_I(toindex),
		CCMake_C(COMM_I_UNDO_PARAM, 2),
		CCMake_I(fromindex),
		CCMake_I(toindex),
		NULL
		);
}

void BindHandleCommand::OnProcessUnDoCommand( RevertableCommand * rc )
{
	ASSERT(rc);
	list<CommittedCommand>::iterator it=rc->commandlist.begin();
	int fromindex = it->ival;
	++it;
	int toindex = it->ival;


	GObject * pFromObj = pgm->FindObjectByID(fromindex);
	GObject * pToObj = pgm->FindObjectByID(toindex);
	ASSERT(pFromObj);
	ASSERT(pFromObj->isHandlePoint());
	ASSERT(pToObj);
	ASSERT(pToObj->isHandlePoint());

	GHandlePoint * pFromHandlePoint = (GHandlePoint *)pFromObj;
	GHandlePoint * pToHandlePoint = (GHandlePoint *)pToObj;

    pFromHandlePoint->BindWith();
    pToHandlePoint->BindWith();
}
