#include "StdAfx.h"
#include "ClipCommand.h"
#include "RenderTargetManager.h"


ClipCommand::ClipCommand(void)
{
}


ClipCommand::~ClipCommand(void)
{
}

void ClipCommand::OnProcessCommand()
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
			CSI_CLIP_WANTINDEX,
			CWP_INDEX);
	}
	else if (step == CSI_CLIP_WANTINDEX)
	{
		ret = pcommand->ProcessPending(
			CSP_CLIP_I_F_INDEX_PROPORTION, COMMPARAMFLAG_I, CWP_INDEX,
			CSI_CLIP_WANTPROPORTION, CWP_PROPORTION
			);
	}
	else if (step == CSI_CLIP_WANTPROPORTION)
	{
		ret = pcommand->ProcessPending(
			CSP_CLIP_I_F_INDEX_PROPORTION, COMMPARAMFLAG_F, CWP_PROPORTION,
			CSI_FINISH
//			CSI_FINISHCONTINUE
			);
	}

	if (ret > 0)
	{
		DispatchNormalSubCommand(ret);
		pcommand->FinishPendingSubCommand();
	}
	else if (ret < 0)
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
						float fProportion = pgp->CalculateProportion();
						if (pObj->isMidPoint())
						{
							pObj = pObj->getLine();
						}
						int index = pObj->getID();

						pcommand->SetParamI(CSP_CLIP_I_F_INDEX_PROPORTION, index, CWP_INDEX);
						pcommand->SetParamF(CSP_CLIP_I_F_INDEX_PROPORTION, fProportion, CWP_PROPORTION);
						pcommand->StepTo(CSI_FINISH);
//						pcommand->StepTo(CSI_FINISHCONTINUE);
					}
				}
			}
		}
		/*
		else if (step == CSI_FINISHCONTINUE)
		{
			ProtectPendingFinishCommand();

			CommitFrontCommand(
				CCMake_C(COMM_CLIP),
				NULL
				);
		}
		*/
	}
}

void ClipCommand::OnDoneCommand()
{
	int index = pcommand->GetParamI(CSP_CLIP_I_F_INDEX_PROPORTION);
	GObject * pObj = pgm->FindObjectByID(index);
	if (!pObj->isLine())
	{
		pcommand->TerminalCommand();
		return;
	}
	float fProporation = pcommand->GetParamF(CSP_CLIP_I_F_INDEX_PROPORTION);
	if (fProporation <= 0 || fProporation >= 1)
	{
		pcommand->TerminalCommand();
		return;
	}

	GLine * pLine = (GLine *)pObj;
	PointF2D ptOBH = pLine->GetBeginPoint()->GetHandle()->GetPointF2D();
	PointF2D ptOEH = pLine->GetEndPoint()->GetHandle()->GetPointF2D();
	GLine * pClonedLine = pLine->Clip(fProporation);
	if (pClonedLine)
	{
		PushRevertable(
			CCMake_C(COMM_I_COMMAND, 3, 1),
			CCMake_CI(COMM_I_COMM_WORKINGLAYER, workinglayerID),
			CCMake_C(COMM_CLIP),
			CCMake_I(index),
			CCMake_F(fProporation),
			CCMake_C(COMM_I_UNDO_PARAM, 6),
			CCMake_I(pLine->getID()),
			CCMake_I(pClonedLine->getID()),
			CCMake_F(ptOBH.x),
			CCMake_F(ptOBH.y),
			CCMake_F(ptOEH.x),
			CCMake_F(ptOEH.y),
			NULL
			);		
	}
}

void ClipCommand::OnProcessUnDoCommand( RevertableCommand * rc )
{
	ASSERT(rc);
	list<CommittedCommand>::iterator it=rc->commandlist.begin();
	int lineindex = it->ival;
	++it;
	int clonedlineindex = it->ival;
	++it;
	float fobhx = it->fval;
	++it;
	float fobhy = it->fval;
	++it;
	float foehx = it->fval;
	++it;
	float foehy = it->fval;

	GLine * pLine = (GLine *)pgm->FindObjectByID(lineindex);
	GLine * pClonedLine = (GLine *)pgm->FindObjectByID(clonedlineindex);
	pLine->Combine(pClonedLine);
	pLine->GetBeginPoint()->SetHandlePosition(fobhx, fobhy);
	pLine->GetEndPoint()->SetHandlePosition(foehx, foehy);
}

bool ClipCommand::staticPickFilterFunc( GObject * pObj )
{
	return getInstance().PickFilterFunc(pObj);
}

bool ClipCommand::PickFilterFunc( GObject * pObj )
{
	if (pObj->isLine() || pObj->isMidPoint())
	{
		return true;
	}
	return false;
}