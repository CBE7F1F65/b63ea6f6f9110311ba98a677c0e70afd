#include "StdAfx.h"
#include "ExtendCommand.h"
#include "RenderTargetManager.h"


ExtendCommand::ExtendCommand(void)
{
	pTempLineLeft = NULL;
}


ExtendCommand::~ExtendCommand(void)
{
}

void ExtendCommand::OnProcessCommand()
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
			CSI_EXTEND_WANTINDEX,
			CWP_INDEX);
	}
	else if (step == CSI_EXTEND_WANTINDEX)
	{
		ret = pcommand->ProcessPending(
			CSP_EXTEND_I_F_INDEX_BEGINOFFSET, COMMPARAMFLAG_I, CWP_INDEX,
			CSI_EXTEND_WANTBEGINOFFSET, CWP_BEGINOFFSET
			);
	}
	else if (step == CSI_EXTEND_WANTBEGINOFFSET)
	{
		ret = pcommand->ProcessPending(
			CSP_EXTEND_I_F_INDEX_BEGINOFFSET, COMMPARAMFLAG_F, CWP_BEGINOFFSET,
			CSI_EXTEND_WANTENDOFFSET, CWP_ENDOFFSET
			);
	}
	else if (step == CSI_EXTEND_WANTENDOFFSET)
	{
		ret = pcommand->ProcessPending(
			CSP_EXTEND_F_ENDOFFSET, COMMPARAMFLAG_F, CWP_ENDOFFSET,
			CSI_FINISH
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
			int iret = pgp->PickPoint();
			if (pgp->IsPickReady(iret))
			{
				if (!pcommand->IsInternalProcessing())
				{
					GObject * pObj = pgp->GetPickedObj();
					if (step == CSI_EXTEND_WANTINDEX)
					{
						if (pObj)
						{
							if (pObj->isMidPoint())
							{
								pObj = pObj->getLine();
							}
							if (pObj->isLine())
							{
								int index = pObj->getID();

								pTempLineLeft = (GBezierLine *)pObj->CreateNewClone(&tBaseNode);
								pTempLineLeft->Extend(1.0f, 1.0f);
								pTempLineRight = (GBezierLine *)pTempLineLeft->Clip(2.0f/3.0f);
								pTempLineLeft->Clip(0.5f)->RemoveFromParent(true);
								pTempLineLeft->SwapBeginEnd();

								pTempLineLeft->SetLineRenderStyle(RHLINESTYLE_DOTTEDLINE);
								pTempLineRight->SetLineRenderStyle(RHLINESTYLE_DOTTEDLINE);

								pgp->SetLockLockLine(pTempLineLeft);

								pcommand->SetParamI(CSP_EXTEND_I_F_INDEX_BEGINOFFSET, index, CWP_INDEX);
								pcommand->StepTo(CSI_EXTEND_WANTBEGINOFFSET, CWP_BEGINOFFSET);
							}
						}
					}
					else if (step >= CSI_EXTEND_WANTBEGINOFFSET)
					{
						GBezierLine * pBezier;
						float fSplit = 0.0f;
						if (step == CSI_EXTEND_WANTBEGINOFFSET)
						{
							pBezier = pTempLineLeft;
						}
						else
						{
							pBezier = pTempLineRight;
						}
						if (pObj == pBezier)
						{
							fSplit = pgp->CalculateProportion() * pBezier->getLength();
						}
						else
						{
						}
						if (step == CSI_EXTEND_WANTBEGINOFFSET)
						{
							pcommand->SetParamF(CSP_EXTEND_I_F_INDEX_BEGINOFFSET, fSplit, CWP_BEGINOFFSET);
							pgp->SetLockLockLine(pTempLineRight);
							pcommand->StepTo(CSI_EXTEND_WANTENDOFFSET, CWP_ENDOFFSET);
						}
						else
						{
							pcommand->SetParamF(CSP_EXTEND_F_ENDOFFSET, fSplit, CWP_ENDOFFSET);
							pgp->UnlockLockLine();
							pcommand->StepTo(CSI_FINISH);
						}
					}
				}
			}
		}
		else if (step == CSI_FINISHCONTINUE)
		{
			ProtectPendingFinishCommand();

			CommitFrontCommand(
				CCMake_C(COMM_EXTEND),
				NULL
				);
		}
	}
	RenderToTarget();
}

void ExtendCommand::RenderToTarget()
{
	int nstep = pcommand->GetStep();
	if (nstep >= CSI_EXTEND_WANTBEGINOFFSET && nstep <= CSI_EXTEND_WANTENDOFFSET && pTempLineLeft)
	{
		HTARGET tar = RenderTargetManager::getInstance().UpdateTarget(RTID_COMMAND);

		prh->BeginRenderTar(tar);
		pTempLineLeft->CallRender();
		pTempLineRight->CallRender();
		prh->EndRenderTar();

		pcommand->SetRenderTarget(tar);
	}
}

void ExtendCommand::OnDoneCommand()
{
	int index = pcommand->GetParamI(CSP_EXTEND_I_F_INDEX_BEGINOFFSET);
	GObject * pObj = pgm->FindObjectByID(index);
	if (!pObj || !pObj->isLine())
	{
		pcommand->TerminalCommand();
		return;
	}
	GBezierLine * pBezier = (GBezierLine *)pObj;
	float fLength = pBezier->getLength();
	if (fLength <= 0)
	{
		pcommand->TerminalCommand();
		return;
	}
	float fBeginOffset = pcommand->GetParamF(CSP_EXTEND_I_F_INDEX_BEGINOFFSET);
	float fEndOffset = pcommand->GetParamF(CSP_EXTEND_F_ENDOFFSET);
	float fBegin = fBeginOffset/fLength;
	float fEnd = fEndOffset/fLength;
	if (fBegin < 0 || fBegin > 1 || fEnd < 0 || fEnd > 1)
	{
		pcommand->TerminalCommand();
		return;
	}
	QuadBezierPointF2D quadBezier(pBezier);
	if (pBezier->Extend(fBegin, fEnd))
	{
		PushRevertable(
			CCMake_C(COMM_I_COMMAND, 4, 1),
			CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
			CCMake_C(COMM_EXTEND),
			CCMake_I(index),
			CCMake_F(fBeginOffset),
			CCMake_F(fEndOffset),
			CCMake_C(COMM_I_UNDO_PARAM, 9),
			CCMake_I(index),
			CCMake_F(quadBezier.ptb.x),
			CCMake_F(quadBezier.ptb.y),
			CCMake_F(quadBezier.ptbh.x),
			CCMake_F(quadBezier.ptbh.y),
			CCMake_F(quadBezier.pteh.x),
			CCMake_F(quadBezier.pteh.y),
			CCMake_F(quadBezier.pte.x),
			CCMake_F(quadBezier.pte.y),
			NULL);
	}

	ClearTemp();
}

void ExtendCommand::OnProcessUnDoCommand( RevertableCommand * rc )
{
	ASSERT(rc);
	list<CommittedCommand>::iterator it=rc->commandlist.begin();
	int index = it->ival;
	++it;
	float bx = it->fval;
	++it;
	float by = it->fval;
	++it;
	float bhx = it->fval;
	++it;
	float bhy = it->fval;
	++it;
	float ehx = it->fval;
	++it;
	float ehy = it->fval;
	++it;
	float ex = it->fval;
	++it;
	float ey = it->fval;

	GLine * pLine = (GLine *)pgm->FindObjectByID(index);
	bool bBezier = !pLine->isStraightLine();
	ASSERT(pLine);
	pLine->SetBeginEnd(bx, by, ex, ey);
	if (bBezier)
	{
		GBezierLine * pBezier = (GBezierLine *)pLine;
		pBezier->SetBeginHandlePos(bhx, bhy);
		pBezier->SetEndHandlePos(ehx, ehy);
	}
}

void ExtendCommand::OnInitCommand()
{
	ClearTemp();
}

void ExtendCommand::OnClearCommand()
{
	ClearTemp();
}

void ExtendCommand::OnTerminalCommand()
{
	ClearTemp();
}

void ExtendCommand::ClearTemp()
{
	if (pTempLineLeft)
	{
		pTempLineLeft->RemoveFromParent(true);
		pTempLineLeft = NULL;
	}
	if (pTempLineRight)
	{
		pTempLineRight->RemoveFromParent(true);
		pTempLineRight = NULL;
	}
	pgp->UnlockLockLine();
}