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

	GHandlePoint * pOldFromToHandlePoint = pFromHandlePoint->getBindWith();
	GHandlePoint * pOldToToHandlePoint = NULL;

	float oldtox = 0;
	float oldtoy = 0;
	if (pToHandlePoint)
	{
		pOldToToHandlePoint = pToHandlePoint->getBindWith();
		oldtox = pToHandlePoint->getX();
		oldtoy = pToHandlePoint->getY();
	}	

	int oldfromtoindex = -1;
	int oldtotoindex = -1;
	if (pOldFromToHandlePoint)
	{
		oldfromtoindex = pOldFromToHandlePoint->getID();
	}
	if (pOldToToHandlePoint)
	{
		oldtotoindex = pOldToToHandlePoint->getID();
	}
	
    pFromHandlePoint->BindWith(pToHandlePoint);

	PushRevertable(
		CCMake_C(COMM_I_COMMAND, 3),
		CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
		CCMake_C(COMM_BINDHANDLE),
		CCMake_I(fromindex),
		CCMake_I(toindex),
		NULL
		);
}
/*
void BindHandleCommand::OnProcessUnDoCommand( RevertableCommand * rc )
{
	ASSERT(rc);
	list<CommittedCommand>::iterator it=rc->commandlist.begin();
	int fromindex = it->ival;
	++it;
	int oldfromtoindex = it->ival;
	++it;
	int toindex = it->ival;
	++it;
	int oldtotoindex = it->ival;
	++it;
	float oldtox = it->fval;
	++it;
	float oldtoy = it->fval;

	GHandlePoint * pFromHandlePoint = (GHandlePoint *)pgm->FindObjectByID(fromindex);
	GHandlePoint * pToHandlePoint = (GHandlePoint *)pgm->FindObjectByID(toindex);
	GHandlePoint * pOldFromHandlePoint = (GHandlePoint *)pgm->FindObjectByID(oldfromtoindex);
	GHandlePoint * pOldToToHandlePoint = (GHandlePoint *)pgm->FindObjectByID(oldtotoindex);

	if (pToHandlePoint)
	{
		pToHandlePoint->BindWith();
		pToHandlePoint->SetPosition(oldtox, oldtoy);
		if (pOldToToHandlePoint)
		{
			pToHandlePoint->BindWith(pOldToToHandlePoint);
		}
	}
	pFromHandlePoint->BindWith(pOldFromHandlePoint);
}
*/