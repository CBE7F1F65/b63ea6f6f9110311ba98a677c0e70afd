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

	if (ret < 0)
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

								pTempLineLeft->CallIndepend();
								pTempLineRight->CallIndepend();

								pTempLineLeft->SetLineRenderStyle(RHLINESTYLE_DOTTEDLINE);
								pTempLineRight->SetLineRenderStyle(RHLINESTYLE_DOTTEDLINE);

								pgp->SetLockLockLine(pTempLineLeft, 0.0f);

								pcommand->SetParamI(CSP_EXTEND_I_F_INDEX_BEGINOFFSET, index, CWP_INDEX);
								pcommand->StepTo(CSI_EXTEND_WANTBEGINOFFSET, CWP_BEGINOFFSET);
							}
						}
					}
					else if (step >= CSI_EXTEND_WANTBEGINOFFSET)
					{
						GBezierLine * pBezier;
						float fSplit = 0.0f;
						float fSplitProp = 0.0f;
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
							fSplitProp = pgp->CalculateProportion();
							if (fSplitProp < 0 || fSplitProp > 1)
							{
								fSplitProp = 0;
								fSplit = 0;
							}
							else
							{
								fSplit = fSplitProp * pBezier->getLength();
							}
						}
						else if (pObj == pBezier->GetEndPoint())
						{
							fSplitProp = 1.0f;
							fSplit = pBezier->getLength();
						}
						if (step == CSI_EXTEND_WANTBEGINOFFSET)
						{
							pcommand->SetParamF(CSP_EXTEND_I_F_INDEX_BEGINOFFSET, fSplit, CWP_BEGINOFFSET);
							if (fSplit == 0.0f)
							{
								pTempLineLeft->RemoveFromParent(true);
								pTempLineLeft = NULL;
							}
							else
							{
								if (fSplitProp != 1.0f)
								{
									pTempLineLeft->Clip(fSplitProp)->RemoveFromParent(true);
								}
							}
							pgp->SetLockLockLine(pTempLineRight, 0.0f);
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
	}
}

void ExtendCommand::RenderToTarget()
{
	int nstep = pcommand->GetStep();
	if (nstep >= CSI_EXTEND_WANTBEGINOFFSET && nstep <= CSI_EXTEND_WANTENDOFFSET && (pTempLineLeft||pTempLineRight))
	{
		HTARGET tar = RenderTargetManager::getInstance().UpdateTarget(RTID_COMMAND);

		prh->BeginRenderTar(tar);
		if (pTempLineLeft)
		{
			pTempLineLeft->CallRender();
		}
		if (pTempLineRight)
		{
			pTempLineRight->CallRender();
		}
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
	if (fBegin < 0 || fEnd < 0)
	{
		pcommand->TerminalCommand();
		return;
	}

	if (pBezier->Extend(fBegin, fEnd))
	{
		PushRevertible(
			CCMake_C(COMM_I_COMMAND, 4),
			CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
			CCMake_C(COMM_EXTEND),
			CCMake_I(index),
			CCMake_F(fBeginOffset),
			CCMake_F(fEndOffset),
			NULL);
		ReAttachAfterMoveNode(pBezier, true);
	}
}
/*
void ExtendCommand::OnProcessUnDoCommand( RevertibleCommand * rc )
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
*/
void ExtendCommand::OnClearTemp()
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