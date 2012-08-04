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
			CSI_CLING_WANTCLINGTYPE, CWP_TYPE
			);
	}
	else if (step == CSI_CLING_WANTCLINGTYPE)
	{
		ret = pcommand->ProcessPending(
			CSP_CLING_I_F_TYPE_VALUE, COMMPARAMFLAG_I, CWP_TYPE,
			CSI_CLING_WANTVAL, CWP_VALUE
			);
	}
	else if (step == CSI_CLING_WANTVAL)
	{
		ret = pcommand->ProcessPending(
			CSP_CLING_I_F_TYPE_VALUE, COMMPARAMFLAG_F, CWP_VALUE,
			CSI_FINISH
			);
	}
}

void ClingCommand::OnDoneCommand()
{
	int fromindex = pcommand->GetParamI(CSP_CLING_I_FROMINDEX);
	int toindex = pcommand->GetParamI(CSP_CLING_I_TOINDEX);
	int nType = pcommand->GetParamI(CSP_CLING_I_F_TYPE_VALUE);
	float fValue = pcommand->GetParamF(CSP_CLING_I_F_TYPE_VALUE);

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
	GLine * pToLine = (GLine *)pToObj;
	
	GClingInfo * pcli = pFromPoint->getClingInfo();
	if (pFromPoint->isClingTo(pToObj) && pcli->GetClingType() == nType && fabsf(pcli->GetClingVal()-fValue)<M_FLOATEXTREMEEPS)
	{
		pcommand->TerminalCommand();
		return;
	}
	
	if (pFromPoint->ClingTo(pToLine, fValue, nType))
	{
		PushRevertable(
			CCMake_C(COMM_I_COMMAND, 5),
			CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
			CCMake_C(COMM_CLING),
			CCMake_I(fromindex),
			CCMake_I(toindex),
			CCMake_I(nType),
			CCMake_F(fValue),
			NULL
			);
	}
}
/*
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
*/