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
			CSP_NEWLAYER_S_NAME, COMMPARAMFLAG_S, CWP_NULL,
			CSI_FINISH
			);

		if (ret<0)
		{
			pcommand->SetParamS(CSP_NEWLAYER_S_NAME, GObjectManager::getInstance().GetDefaultLayerName());
			pcommand->StepTo(CSI_FINISH);
		}
	}
}

void NewLayerCommand::OnDoneCommand()
{
	const char * layername = pcommand->GetParamS(CSP_NEWLAYER_S_NAME);
	GLayer * pLayer = GObjectManager::getInstance().NewLayer(workingLayer, layername);
	PushRevertable(
		CCMake_C(COMM_I_ADDNODE, 2),
		CCMake_D((int)pLayer),
		CCMake_D((int)(pLayer->getParent())),
		CCMake_C(COMM_I_COMMAND, 2, 0),
		CCMake_C(COMM_NEWLAYER),
		CCMake_S(layername),
		NULL);
}

void NewSubLayerCommand::OnProcessCommand()
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
			CSP_NEWLAYER_S_NAME, COMMPARAMFLAG_S, CWP_NULL,
			CSI_FINISH
			);

		if (ret<0)
		{
			pcommand->SetParamS(CSP_NEWLAYER_S_NAME, GObjectManager::getInstance().GetDefaultLayerName());
			pcommand->StepTo(CSI_FINISH);
		}
	}
}

void NewSubLayerCommand::OnDoneCommand()
{
	const char * layername = pcommand->GetParamS(CSP_NEWLAYER_S_NAME);
	GLayer * pLayer = GObjectManager::getInstance().NewSubLayer(workingLayer, layername);
	PushRevertable(
		CCMake_C(COMM_I_ADDNODE, 2),
		CCMake_D((int)pLayer),
		CCMake_D((int)(pLayer->getParent())),
		CCMake_C(COMM_I_COMMAND, 2, 0),
		CCMake_C(COMM_NEWSUBLAYER),
		CCMake_S(layername),
		NULL);
}

NewSubLayerCommand::NewSubLayerCommand()
{

}

NewSubLayerCommand::~NewSubLayerCommand()
{

}