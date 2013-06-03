#include "StdAfx.h"
#include "DemergeCommand.h"


DemergeCommand::DemergeCommand(void)
{
}


DemergeCommand::~DemergeCommand(void)
{
}

void DemergeCommand::OnProcessCommand()
{
	int step = OnNormalProcessCommand();
	int nowstep = pcommand->GetStep();
	UpdateLastStep();

	int ret = -1;

	if (step == CSI_INIT)
	{
		pcommand->StepTo(
			CSI_DEMERGE_WANTFROMINDEX,
			CWP_INDEX);
	}
	else if (step == CSI_DEMERGE_WANTFROMINDEX)
	{
		ret = pcommand->ProcessPending(
			CSP_DEMERGE_I_FROMINDEX, COMMPARAMFLAG_I, CWP_INDEX,
			CSI_DEMERGE_WANTTOINDEX
			);
	}
	else if (step == CSI_DEMERGE_WANTTOINDEX)
	{
		ret = pcommand->ProcessPending(
			CSP_DEMERGE_I_TOINDEX, COMMPARAMFLAG_I, CWP_INDEX,
			CSI_FINISH
			);
	}
}

void DemergeCommand::OnDoneCommand()
{
	int fromindex = pcommand->GetParamI(CSP_DEMERGE_I_FROMINDEX);
	int toindex = pcommand->GetParamI(CSP_DEMERGE_I_TOINDEX);

	GObject * pFromObj = pgm->FindObjectByID(fromindex);
	GObject * pToObj = pgm->FindObjectByID(toindex);
	if (!pFromObj || !pToObj)
	{
		Terminal();
		return;
	}
	if (!pFromObj->isPoint() || !pToObj->isPoint())
	{
		Terminal();
		return;
	}
	GPoint * pFromPoint = (GPoint *)pFromObj;
	GPoint * pToPoint = (GPoint *)pToObj;

	if (!pFromPoint->isMergeWith(pToPoint))
	{
		Terminal();
		return;
	}
	
	pFromPoint->DemergeFrom(pToPoint);

	PushRevertible(
		CCMake_C(COMM_I_COMMAND, 3),
		CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
		CCMake_C(COMM_DEMERGE),
		CCMake_I(fromindex),
		CCMake_I(toindex),
		NULL
		);
}
/*
void SeparateCommand::OnProcessUnDoCommand( RevertibleCommand * rc )
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

	pFromPoint->MergeWith(pToPoint);
}
*/