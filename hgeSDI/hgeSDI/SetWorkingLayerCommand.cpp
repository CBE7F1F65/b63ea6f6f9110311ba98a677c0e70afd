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
			CSI_SETWORKINGLAYER_WANTLASTINDEX,
			CWP_NULL);
	}
	else if (step == CSI_SETWORKINGLAYER_WANTLASTINDEX)
	{
		ret = pcommand->ProcessPending(
			CSP_SETWORKINGLAYER_I_LASTINDEX, COMMPARAMFLAG_I, CWP_NULL,
			CSI_SETWORKINGLAYER_WANTNEWINDEX
			);
		if (ret<0)
		{
			pcommand->SetParamI(CSP_SETWORKINGLAYER_I_LASTINDEX, lastWorkingLayer->getID());
			pcommand->StepTo(CSI_SETWORKINGLAYER_WANTNEWINDEX);
		}
	}
	else if (step == CSI_SETWORKINGLAYER_WANTNEWINDEX)
	{
		ret = pcommand->ProcessPending(
			CSP_SETWORKINGLAYER_I_NEWINDEX, COMMPARAMFLAG_I, CWP_NULL,
			CSI_FINISH
			);
		if (ret<0)
		{
			pcommand->SetParamI(CSP_SETWORKINGLAYER_I_LASTINDEX, workingLayer->getID());
			pcommand->StepTo(CSI_FINISH);
		}
	}

}

void SetWorkingLayerCommand::OnDoneCommand()
{
	int lastobjid = pcommand->GetParamI(CSP_SETWORKINGLAYER_I_LASTINDEX);
	int newobjid = pcommand->GetParamI(CSP_SETWORKINGLAYER_I_NEWINDEX);
//	GObject * lastObj = GObjectManager::getInstance().FindObjectByID(lastobjid);
	GObject * newObj = GObjectManager::getInstance().FindObjectByID(newobjid);
	GObjectManager::getInstance().SetActiveLayer(newObj);

	PushRevertable(
		CCMake_C(COMM_I_COMMAND, 3, 1),
		CCMake_C(COMM_I_COMM_WORKINGLAYER, workingLayer->getID()),
		CCMake_C(COMM_SETWORKINGLAYER),
		CCMake_I(lastobjid),
		CCMake_I(newobjid),
		CCMake_C(COMM_I_UNDO_PARAM, 1),
		CCMake_I(lastobjid),
		NULL
		);
}

void SetWorkingLayerCommand::OnProcessUnDoCommand( RevertableCommand * rc )
{
	int lastobjid = pcommand->GetIvalFromRC(rc, CSP_SETWORKINGLAYER_I_LASTINDEX);
	GObject * lastObj = GObjectManager::getInstance().FindObjectByID(lastobjid);
	GObjectManager::getInstance().SetActiveLayer(lastObj);
}