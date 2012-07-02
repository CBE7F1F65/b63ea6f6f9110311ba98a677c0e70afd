#include "StdAfx.h"
#include "ClingCommand.h"


ClingCommand::ClingCommand(void)
{
}


ClingCommand::~ClingCommand(void)
{
}

void ClingCommand::OnProcessCommand()
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
			CSI_CLING_WANTFROMINDEX,
			CWP_INDEX);
	}
	else if (step == CSI_CLING_WANTFROMINDEX)
	{
		ret = pcommand->ProcessPending(
			CSP_CLING_I_FROMINDEX, COMMPARAMFLAG_I, CWP_INDEX,
			CSI_CLING_WANTTOINDEX
			);
	}
	else if (step == CSI_CLING_WANTTOINDEX)
	{
		ret = pcommand->ProcessPending(
			CSP_CLING_I_TOINDEX, COMMPARAMFLAG_I, CWP_INDEX,
			CSI_FINISH
			);
	}

	if (ret > 0)
	{
		DispatchNormalSubCommand(ret);
		pcommand->FinishPendingSubCommand();
	}
}

void ClingCommand::OnDoneCommand()
{
	int fromindex = pcommand->GetParamI(CSP_CLING_I_FROMINDEX);
	int toindex = pcommand->GetParamI(CSP_CLING_I_TOINDEX);

	GObject * pFromObj = pgm->FindObjectByID(fromindex);
	GObject * pToObj = pgm->FindObjectByID(toindex);
	if (!pFromObj)
	{
		return;
	}
	if (!pFromObj->isPoint() ||(pToObj && !pToObj->isLine()))
	{
		return;
	}

	GPoint * pFromPoint = (GPoint *)pFromObj;
//	GLine * pToLine = (GLine *)pToObj;

	GObject * pOClingto = pFromPoint->getClingTo();
	int oClingToIndex = -1;
	if (pOClingto)
	{
		oClingToIndex = pOClingto->getID();
	}

	if (pFromPoint->isClingTo(pToObj))
	{
		return;
	}
	pFromPoint->ClingTo(pToObj);

	PushRevertable(
		CCMake_C(COMM_I_COMMAND, 3, 1),
		CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
		CCMake_C(COMM_CLING),
		CCMake_I(fromindex),
		CCMake_I(toindex),
		CCMake_C(COMM_I_UNDO_PARAM, 2),
		CCMake_I(fromindex),
		CCMake_I(oClingToIndex),
		NULL
		);
}

void ClingCommand::OnProcessUnDoCommand( RevertableCommand * rc ) 
{
	ASSERT(rc);
	list<CommittedCommand>::iterator it=rc->commandlist.begin();
	int fromindex = it->ival;
	++it;
	int oClingToIndex = it->ival;


	GObject * pObj = pgm->FindObjectByID(fromindex);
	ASSERT(pObj);
	ASSERT(pObj->isPoint());
	GPoint * pPoint = (GPoint *)pObj;
	GObject * pOClingToObj = NULL;
	if (oClingToIndex >= 0)
	{
		pgm->FindObjectByID(oClingToIndex);
	}
	pPoint->DeclingToOther();
	if (pOClingToObj)
	{
		pPoint->ClingTo(pOClingToObj);
	}
}