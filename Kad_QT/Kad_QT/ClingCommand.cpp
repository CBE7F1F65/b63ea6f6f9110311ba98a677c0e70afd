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
			CSP_CLING_I_F_TOINDEX_PROPORTION, COMMPARAMFLAG_I, CWP_INDEX,
			CSI_CLING_WANTPROPORTION, CWP_PROPORTION
			);
	}
	else if (step == CSI_CLING_WANTPROPORTION)
	{
		ret = pcommand->ProcessPending(
			CSP_CLING_I_F_TOINDEX_PROPORTION, COMMPARAMFLAG_F, CWP_PROPORTION,
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
	int toindex = pcommand->GetParamI(CSP_CLING_I_F_TOINDEX_PROPORTION);
	float fProportion = pcommand->GetParamF(CSP_CLING_I_F_TOINDEX_PROPORTION);

	GObject * pFromObj = pgm->FindObjectByID(fromindex);
	GObject * pToObj = pgm->FindObjectByID(toindex);
	if (!pFromObj)
	{
		pcommand->TerminalCommand();
		return;
	}
	if (!pFromObj->isPoint() ||(pToObj && !pToObj->isLine()))
	{
		pcommand->TerminalCommand();
		return;
	}

	GPoint * pFromPoint = (GPoint *)pFromObj;
//	GLine * pToLine = (GLine *)pToObj;

	GLine * pOClingto = pFromPoint->getClingTo();
	int oClingToIndex = -1;
	float oClingToProportion = 0;
	if (pOClingto)
	{
		oClingToIndex = pOClingto->getID();
		oClingToProportion = pFromPoint->getClingProportion();
	}

	if (pFromPoint->isClingTo(pToObj) && fabsf(pFromPoint->getClingProportion() - fProportion) < M_FLOATEPS)
	{
		pcommand->TerminalCommand();
		return;
	}
	
	if (pFromPoint->ClingTo(pToObj, fProportion))
	{

		PushRevertable(
			CCMake_C(COMM_I_COMMAND, 4, 1),
			CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
			CCMake_C(COMM_CLING),
			CCMake_I(fromindex),
			CCMake_I(toindex),
			CCMake_F(fProportion),
			CCMake_C(COMM_I_UNDO_PARAM, 3),
			CCMake_I(fromindex),
			CCMake_I(oClingToIndex),
			CCMake_F(oClingToProportion),
			NULL
			);
	}
}

void ClingCommand::OnProcessUnDoCommand( RevertableCommand * rc ) 
{
	ASSERT(rc);
	list<CommittedCommand>::iterator it=rc->commandlist.begin();
	int fromindex = it->ival;
	++it;
	int oClingToIndex = it->ival;
	++it;
	float oClingToProportion = it->fval;


	GObject * pObj = pgm->FindObjectByID(fromindex);
	ASSERT(pObj);
	ASSERT(pObj->isPoint());
	GPoint * pPoint = (GPoint *)pObj;
	GObject * pOClingToObj = NULL;
	if (oClingToIndex >= 0)
	{
		pOClingToObj = pgm->FindObjectByID(oClingToIndex);
	}
	pPoint->DeclingToOther();
	if (pOClingToObj)
	{
		pPoint->ClingTo(pOClingToObj, oClingToProportion);
	}
}