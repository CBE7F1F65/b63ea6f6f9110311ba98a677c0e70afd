#include "StdAfx.h"
#include "SetWorkingLayerCommand.h"
#include "GObjectManager.h"


SetWorkingLayerCommand::SetWorkingLayerCommand(void)
{
}


SetWorkingLayerCommand::~SetWorkingLayerCommand(void)
{
}

void SetWorkingLayerCommand::OnProcessCommand()
{

	int step = OnNormalProcessCommand();
	UpdateLastStep();

	int ret = -1;

	if (step == CSI_INIT)
	{
		pcommand->StepTo(
			CSI_SETWORKINGLAYER_WANTNEWINDEX,
			CWP_NULL);
	}
	else if (step == CSI_SETWORKINGLAYER_WANTNEWINDEX)
	{
		ret = pcommand->ProcessPending(
			CSP_SETWORKINGLAYER_I_NEWINDEX, COMMPARAMFLAG_I, CWP_NULL,
			CSI_SETWORKINGLAYER_WANTLASTINDEX
			);
		if (ret<0)
		{
			pcommand->SetParamI(CSP_SETWORKINGLAYER_I_NEWINDEX, pgm->GetActiveLayer()->getID());
			pcommand->StepTo(CSI_SETWORKINGLAYER_WANTLASTINDEX);
		}
	}
	else if (step == CSI_SETWORKINGLAYER_WANTLASTINDEX)
	{
		ret = pcommand->ProcessPending(
			CSP_SETWORKINGLAYER_I_LASTINDEX, COMMPARAMFLAG_I, CWP_NULL,
			CSI_FINISH
			);
		if (ret<0)
		{
			pcommand->SetParamI(CSP_SETWORKINGLAYER_I_LASTINDEX, pgm->GetLastActiveLayer()->getID());
			pcommand->StepTo(CSI_FINISH);
		}
	}

}

void SetWorkingLayerCommand::OnDoneCommand()
{
	int lastobjid = pcommand->GetParamI(CSP_SETWORKINGLAYER_I_LASTINDEX);
	int newobjid = pcommand->GetParamI(CSP_SETWORKINGLAYER_I_NEWINDEX);
//	GObject * lastObj = GObjectManager::getInstance().FindObjectByID(lastobjid);
	GObject * newObj = pgm->FindObjectByID(newobjid);
	
	pgm->SetActiveLayer_Internal(newObj);

	PushRevertable(
		CCMake_C(COMM_I_COMMAND, 2, 1),
		CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
		CCMake_C(COMM_SETWORKINGLAYER),
		CCMake_I(newobjid),
		// Remain
		CCMake_C(COMM_I_UNDO_PARAM, 1),
		CCMake_I(lastobjid),
		NULL
		);
}
// Remain
void SetWorkingLayerCommand::OnProcessUnDoCommand( RevertableCommand * rc )
{
	int lastobjid = pcommand->GetIvalFromRC(rc, CSPUNDO_SETWORKINGLAYER_I_LASTINDEX);
	GObject * lastObj = pgm->FindObjectByID(lastobjid);
	pgm->SetActiveLayer_Internal(lastObj);
}