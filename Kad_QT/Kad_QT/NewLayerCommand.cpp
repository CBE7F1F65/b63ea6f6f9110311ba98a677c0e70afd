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
			CWP_NAME);
	}
	else if (step == CSI_NEWLAYER_WANTNAME)
	{
		ret = pcommand->ProcessPending(
			CSP_NEWLAYER_S_I_NAME_INDEX, COMMPARAMFLAG_S, CWP_INDEX,
			CSI_NEWLAYER_WANTINDEX
			);
		if (ret<0)
		{
			pcommand->SetParamS(CSP_NEWLAYER_S_I_NAME_INDEX, pgm->GetDefaultLayerName(), CWP_NAME);
			pcommand->StepTo(CSI_NEWLAYER_WANTINDEX, CWP_INDEX);
		}
	}
	else if (step == CSI_NEWLAYER_WANTINDEX)
	{
		ret = pcommand->ProcessPending(
			CSP_NEWLAYER_S_I_NAME_INDEX, COMMPARAMFLAG_I, CWP_INDEX,
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
		pLayer = pgm->NewLayer(pgm->GetActiveLayer(), layername, layerIndex);
	}
	else
	{
		pLayer = pgm->NewSubLayer(pgm->GetActiveLayer(), layername, layerIndex);
	}

	if (layerIndex < 0)
	{
		layerIndex = pLayer->getLayerID();
	}
	PushRevertible(
		CCMake_C(COMM_I_COMMAND, 3),
		CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
		CCMake_C(comm),
		CCMake_S(layername),
		CCMake_I(layerIndex),
		NULL);
}
