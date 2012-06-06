#include "StdAfx.h"
#include "NewLayerCommand.h"
#include "GObjectManager.h"


NewLayerCommand::NewLayerCommand(void)
{
}


NewLayerCommand::~NewLayerCommand(void)
{
}

void NewLayerCommand::OnProcessCommand()
{
	
	int step = OnNormalProcessCommand();
	UpdateLastStep();

	int ret = -1;

	if (step == CSI_INIT)
	{
		pcommand->StepTo(
			CSI_NEWLAYER_WANTNAME,
			CWP_NULL);
	}
	else if (step == CSI_NEWLAYER_WANTNAME)
	{
		ret = pcommand->ProcessPending(
			CSP_NEWLAYER_S_I_NAME_INDEX, COMMPARAMFLAG_S, CWP_NULL,
			CSI_NEWLAYER_WANTINDEX
			);
		if (ret<0)
		{
			pcommand->SetParamS(CSP_NEWLAYER_S_I_NAME_INDEX, GObjectManager::getInstance().GetDefaultLayerName());
			pcommand->StepTo(CSI_NEWLAYER_WANTINDEX);
		}
	}
	else if (step == CSI_NEWLAYER_WANTINDEX)
	{
		ret = pcommand->ProcessPending(
			CSP_NEWLAYER_S_I_NAME_INDEX, COMMPARAMFLAG_I, CWP_NULL,
			CSI_FINISH
			);
		if (ret<0)
		{
			pcommand->SetParamI(CSP_NEWLAYER_S_I_NAME_INDEX, -1);
			pcommand->StepTo(CSI_FINISH);
		}
	}
}

void NewLayerCommand::OnDoneCommand()
{
	const char * layername = pcommand->GetParamS(CSP_NEWLAYER_S_I_NAME_INDEX);
	int layerIndex = pcommand->GetParamI(CSP_NEWLAYER_S_I_NAME_INDEX);
	GLayer * pLayer = NULL;

	if (comm == COMM_NEWLAYER)
	{
		pLayer = GObjectManager::getInstance().NewLayer(workingLayer, layername, layerIndex);
	}
	else
	{
		pLayer = GObjectManager::getInstance().NewSubLayer(workingLayer, layername, layerIndex);
	}

	if (layerIndex < 0)
	{
		layerIndex = pLayer->layerID;
	}
	PushRevertable(
		CCMake_C(COMM_I_ADDNODE, 2),
// 		CCMake_D((int)pLayer),
// 		CCMake_D((int)(pLayer->getParent())),
		CCMake_I(pLayer->getID()),
		CCMake_I(pLayer->getParent()->getID()),
		CCMake_C(COMM_I_COMMAND, 3, 0),
		CCMake_C(COMM_I_COMM_WORKINGLAYER, workingLayerID),
		CCMake_C(comm),
		CCMake_S(layername),
		CCMake_I(layerIndex),
		NULL);
}
