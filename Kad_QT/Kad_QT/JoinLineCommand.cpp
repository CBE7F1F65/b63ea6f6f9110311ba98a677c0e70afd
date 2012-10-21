#include "StdAfx.h"
#include "JoinLineCommand.h"
#include "RenderTargetManager.h"


JoinLineCommand::JoinLineCommand(void)
{
}


JoinLineCommand::~JoinLineCommand(void)
{
}

void JoinLineCommand::OnProcessCommand()
{
	int step = OnNormalProcessCommand();
	int nowstep = pcommand->GetStep();
	UpdateLastStep();

	int ret = -1;

	if (step == CSI_INIT)
	{
		pcommand->StepTo(
			CSI_JOIN_WANTINDEX,
			CWP_INDEX);
	}
	else if (step == CSI_JOIN_WANTINDEX)
	{
		ret = pcommand->ProcessPending(
			CSP_JOIN_I_INDEX, COMMPARAMFLAG_I, CWP_INDEX,
			CSI_JOIN_WANTDELETEINDEX, CWP_INDEX
			);
	}
	else if (step == CSI_JOIN_WANTDELETEINDEX)
	{
		ret = pcommand->ProcessPending(
			CSP_JOIN_I_DELETEINDEX, COMMPARAMFLAG_I, CWP_INDEX,
			CSI_FINISH
			);
	}

	if (ret < 0)
	{
		// Routine
		if (step > CSI_INIT)
		{
			int iret = pgp->PickPoint(staticPickFilterFunc, staticPickFilterFunc);
			if (pgp->IsPickReady(iret))
			{
				if (!pcommand->IsInternalProcessing())
				{
					GObject * pObj = pgp->GetPickedObj();
					if (pObj)
					{
						if (pObj->isMidPoint())
						{
							pObj = pObj->getLine();
						}
						int index = pObj->getID();

						if (step == CSI_JOIN_WANTINDEX)
						{
							pcommand->SetParamI(CSP_JOIN_I_INDEX, index, CWP_INDEX);
							pcommand->StepTo(CSI_JOIN_WANTDELETEINDEX);
							pFirst = (GLine *)pObj;
						}
						else
						{
							pcommand->SetParamI(CSP_JOIN_I_DELETEINDEX, index, CWP_INDEX);
							pcommand->StepTo(CSI_FINISH);
						}
					}
				}
			}
		}
	}
}

void JoinLineCommand::OnDoneCommand()
{
	int index = pcommand->GetParamI(CSP_JOIN_I_INDEX);
	int deleteindex = pcommand->GetParamI(CSP_JOIN_I_DELETEINDEX);
	GObject * pObj = pgm->FindObjectByID(index);
	GObject * pDeleteObj = pgm->FindObjectByID(deleteindex);
	if (!pObj || !pDeleteObj || !pObj->isLine() || !pDeleteObj->isLine())
	{
		Terminal();
		return;
	}

	GLine * pLine = (GLine *)pObj;
	GLine * pDeleteLine = (GLine *)pDeleteObj;
	if (pLine->JoinLine(pDeleteLine))
	{
		PushRevertible(
			CCMake_C(COMM_I_COMMAND, 3),
			CCMake_CI(COMM_I_COMM_WORKINGLAYER, workinglayerID),
			CCMake_C(COMM_JOIN),
			CCMake_I(index),
			CCMake_I(deleteindex),
			NULL
			);		
	}
}

void JoinLineCommand::OnClearTemp()
{
	pFirst = NULL;
}

void JoinLineCommand::RenderToTarget()
{
	int nstep = pcommand->GetStep();
	if (nstep > CSI_JOIN_WANTINDEX && nstep <= CSI_JOIN_WANTDELETEINDEX && pFirst)
	{
		HTARGET tar = RenderTargetManager::getInstance().UpdateTarget(RTID_COMMAND);

		prh->BeginRenderTar(tar);
		pFirst->CallRender(LINECOLOR_HIGHLIGHT);
		prh->EndRenderTar();

		pcommand->SetRenderTarget(tar);
	}
}

bool JoinLineCommand::staticPickFilterFunc( GObject * pObj )
{
	return getInstance().PickFilterFunc(pObj);
}

bool JoinLineCommand::PickFilterFunc( GObject * pObj )
{
	if (pObj->isLine() || pObj->isMidPoint())
	{
		return true;
	}
	return false;
}