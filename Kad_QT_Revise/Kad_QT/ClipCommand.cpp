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
						float fProportion = pgp->CalculateProportion();
						if (pObj->isMidPoint())
						{
							pObj = pObj->getLine();
						}
						if (pObj->isPoint())
						{
							GPoint * pPoint = (GPoint *)pObj;
							pObj = pPoint->getClingInfo()->GetClingTo();
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
	if (!pObj || !pObj->isLine())
	{
		Terminal();
		return;
	}
	float fProporation = pcommand->GetParamF(CSP_CLIP_I_F_INDEX_PROPORTION);
	if (fProporation <= 0 || fProporation >= 1)
	{
		Terminal();
		return;
	}

	GLine * pLine = (GLine *)pObj;
	GLine * pClonedLine = pLine->Clip(fProporation);
	if (pClonedLine)
	{
		PushRevertible(
			CCMake_C(COMM_I_COMMAND, 3),
			CCMake_CI(COMM_I_COMM_WORKINGLAYER, workinglayerID),
			CCMake_C(COMM_CLIP),
			CCMake_I(index),
			CCMake_F(fProporation),
			NULL
			);		
	}
}
/*
void ClipCommand::OnProcessUnDoCommand( RevertibleCommand * rc )
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
*/
bool ClipCommand::staticPickFilterFunc( GObject * pObj )
{
	return getInstance().PickFilterFunc(pObj);
}

bool ClipCommand::PickFilterFunc( GObject * pObj )
{
	if (pObj->isPoint() && pObj->canAttach())
	{
		GPoint * pPoint = (GPoint *)pObj;
		if (pPoint->getClingInfo()->GetClingTo())
		{
			return false;
		}
	}
	if (pObj->isLine() || pObj->isMidPoint())
	{
		return true;
	}
	return false;
}