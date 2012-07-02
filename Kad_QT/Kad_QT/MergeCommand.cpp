#include "StdAfx.h"
#include "MergeCommand.h"


MergeCommand::MergeCommand(void)
{
}


MergeCommand::~MergeCommand(void)
{
}

void MergeCommand::OnProcessCommand()
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
			CSI_MERGE_WANTFROMINDEX,
			CWP_INDEX);
	}
	else if (step == CSI_MERGE_WANTFROMINDEX)
	{
		ret = pcommand->ProcessPending(
			CSP_MERGE_I_FROMINDEX, COMMPARAMFLAG_I, CWP_INDEX,
			CSI_MERGE_WANTTOINDEX
			);
	}
	else if (step == CSI_MERGE_WANTTOINDEX)
	{
		ret = pcommand->ProcessPending(
			CSP_MERGE_I_TOINDEX, COMMPARAMFLAG_I, CWP_INDEX,
			CSI_FINISH
			);
	}

	if (ret > 0)
	{
		DispatchNormalSubCommand(ret);
		pcommand->FinishPendingSubCommand();
	}
}

void MergeCommand::OnDoneCommand()
{
	int fromindex = pcommand->GetParamI(CSP_MERGE_I_FROMINDEX);
	int toindex = pcommand->GetParamI(CSP_MERGE_I_TOINDEX);

	GObject * pFromObj = pgm->FindObjectByID(fromindex);
	GObject * pToObj = pgm->FindObjectByID(toindex);
	if (!pFromObj || !pToObj)
	{
		return;
	}
	if (!pFromObj->isPoint() || !pToObj->isPoint())
	{
		return;
	}
	GPoint * pFromPoint = (GPoint *)pFromObj;
	GPoint * pToPoint = (GPoint *)pToObj;
	
	if (pFromPoint->isMergeWith(pToPoint))
	{
		return;
	}
	pFromPoint->MergeWith(pToPoint);

	PushRevertable(
		CCMake_C(COMM_I_COMMAND, 3, 1),
		CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
		CCMake_C(COMM_MERGE),
		CCMake_I(fromindex),
		CCMake_I(toindex),
		CCMake_C(COMM_I_UNDO_PARAM, 2),
		CCMake_I(fromindex),
		CCMake_I(toindex),
		NULL
		);
}

void MergeCommand::OnProcessUnDoCommand( RevertableCommand * rc )
{
	ASSERT(rc);
	list<CommittedCommand>::iterator it=rc->commandlist.begin();
	int fromindex = it->ival;
	++it;
	int toindex = it->ival;


	GObject * pFromObj = pgm->FindObjectByID(fromindex);
	GObject * pToObj = pgm->FindObjectByID(toindex);
	ASSERT(pFromObj);
	ASSERT(pFromObj->isPoint());
	ASSERT(pToObj);
	ASSERT(pToObj->isPoint());

	GPoint * pFromPoint = (GPoint *)pFromObj;
	GPoint * pToPoint = (GPoint *)pToObj;

	pFromPoint->SeperateFrom(pToPoint);
}