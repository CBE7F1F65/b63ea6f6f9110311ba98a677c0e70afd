#include "StdAfx.h"
#include "BezierCommand.h"

#include "RenderTargetManager.h"
#include "RenderHelper.h"
#include "MathHelper.h"
#include "GUICoordinate.h"
#include "ColorManager.h"
#include "Command.h"
#include "GObjectManager.h"
#include "GUICursor.h"
#include "GLine.h"
#include "MarkingManager.h"
#include "MarkingObject.h"

BezierCommand::BezierCommand(void)
{
}

BezierCommand::~BezierCommand(void)
{
}

void BezierCommand::OnProcessCommand()
{
	int step = OnNormalProcessCommand(GUIC_CREATEPOINT);

	int nowstep = pcommand->GetStep();
	if (IsStepped())
	{
		if (nowstep > CSI_INIT)
		{
			pcommand->EnableSubCommand(
				(laststep.step==CSI_RESUME)?false:true,
				SSC_UNDO,
				SSC_REDO,
				SSC_TERMINAL,
				SSC_NULL);
		}
	}
	UpdateLastStep();

	int ret = -1;

	if (step == CSI_INIT)
	{
		pcommand->StepTo(
			CSI_BEZIER_WANTBAX,
			CWP_XY_B);
	}
	else if (step == CSI_BEZIER_WANTBAX)
	{
		ret = pcommand->ProcessPending(
			CSP_BEZIER_XY_BA, COMMPARAMFLAG_X, CWP_X_B,	// Fill in Param
			CSI_BEZIER_WANTBAY, CWP_Y_B					// Step to with want
			);
	}
	else if (step == CSI_BEZIER_WANTBAY)
	{
		ret = pcommand->ProcessPending(
			CSP_BEZIER_XY_BA, COMMPARAMFLAG_Y, CWP_Y_B,	// Fill in Param
			CSI_BEZIER_WANTBHX, CWP_HANDLEX					// Step to with want
			);
	}
	else if (step == CSI_BEZIER_WANTBHX)
	{
		ret = pcommand->ProcessPending(
			CSP_BEZIER_XY_BH, COMMPARAMFLAG_X, CWP_HANDLEX,	// Fill in Param
			CSI_BEZIER_WANTBHY, CWP_HANDLEY					// Step to with want
			);
	}
	else if (step == CSI_BEZIER_WANTBHY)
	{
		ret = pcommand->ProcessPending(
			CSP_BEZIER_XY_BH, COMMPARAMFLAG_Y, CWP_HANDLEY,	// Fill in Param
			CSI_BEZIER_WANTNAX, CWP_XY_N					// Step to with want
			);
	}
	else if (step == CSI_BEZIER_WANTNAX)
	{
		ret = pcommand->ProcessPending(
			CSP_BEZIER_XY_NA, COMMPARAMFLAG_X, CWP_X_N,	// Fill in Param
			CSI_BEZIER_WANTNAY, CWP_Y_N					// Step to with want
			);
	}
	else if (step == CSI_BEZIER_WANTNAY)
	{
		ret = pcommand->ProcessPending(
			CSP_BEZIER_XY_NA, COMMPARAMFLAG_Y, CWP_Y_N,	// Fill in Param
			CSI_BEZIER_WANTNHX, CWP_HANDLEX					// Step to with want
			);
	}
	else if (step == CSI_BEZIER_WANTNHX)
	{
		ret = pcommand->ProcessPending(
			CSP_BEZIER_XY_NH, COMMPARAMFLAG_X, CWP_HANDLEX,	// Fill in Param
			CSI_BEZIER_WANTNHY, CWP_HANDLEY					// Step to with want
			);
	}
	else if (step == CSI_BEZIER_WANTNHY)
	{
		ret = pcommand->ProcessPending(
			CSP_BEZIER_XY_NH, COMMPARAMFLAG_Y, CWP_HANDLEY,	// Fill in Param
			CSI_FINISHCONTINUE					// Step to with want
			);
	}


	if (step >= CSI_BEZIER_WANTBHX)
	{
		float bx, by;
		pcommand->GetParamXY(CSP_BEZIER_XY_BA, &bx, &by);
		pgp->PushInterestPoint(bx, by);
		if (step >= CSI_BEZIER_WANTNHX)
		{
			pcommand->GetParamXY(CSP_BEZIER_XY_NA, &bx, &by);
			pgp->PushInterestPoint(bx, by);
		}
	}

	if (!ret)
	{
	}
	else if (ret > 0)
	{
		// Dispatch Subcommand
		DispatchNormalSubCommand(ret);
		pcommand->FinishPendingSubCommand();
	}
	else if (ret < 0)
	{
		// Routine
		if (step > CSI_INIT)
		{
			if (!pcommand->IsInternalProcessing())
			{
				int iret = pgp->PickPoint();
				if (pgp->IsMouseDownReady())
				{
					int tosetpindex = -1;
					switch (step)
					{
					case CSI_BEZIER_WANTBAX:
					case CSI_BEZIER_WANTBAY:
						tosetpindex = CSP_BEZIER_XY_BA;
						break;
					case CSI_BEZIER_WANTNAX:
					case CSI_BEZIER_WANTNAY:
						tosetpindex = CSP_BEZIER_XY_NA;
						break;
					}
					if (tosetpindex >= 0)
					{
						pcommand->SetParamX(tosetpindex, pgp->GetPickX_C(), CWP_X);
						pcommand->SetParamY(tosetpindex, pgp->GetPickY_C(), CWP_Y);
					}
					switch (tosetpindex)
					{
					case CSP_BEZIER_XY_BA:
						pcommand->StepTo(CSI_BEZIER_WANTBHX, CWP_HANDLEX);
						pMergeToBegin = pgp->GetPickedObj();
						if (pMergeToBegin)
						{
							fProportionBegin = pgp->CalculateProportion();
						}
						break;
					case CSP_BEZIER_XY_NA:
						pcommand->StepTo(CSI_BEZIER_WANTNHX, CWP_HANDLEX);
						pMergeToEnd = pgp->GetPickedObj();
						if (pMergeToEnd)
						{
							fProportionEnd = pgp->CalculateProportion();
						}
						break;
					}
				}
				if (pgp->IsPickReady(iret))
				{
					int tosetpindex=-1;
					float pickx = pgp->GetPickX_C();
					float picky = pgp->GetPickY_C();

					GObject * pPicked = pgp->GetPickedObj();
					if (pPicked)
					{
						if (pPicked->isAnchorPoint())
						{
							if (step < CSI_BEZIER_WANTNAX)
							{
								pBindAnchorBegin = (GAnchorPoint *)pPicked;
							}
							else
							{
								pBindAnchorEnd = (GAnchorPoint *)pPicked;
							}
						}
					}

					switch (step)
					{
					case CSI_BEZIER_WANTBHX:
					case CSI_BEZIER_WANTBHY:
						tosetpindex = CSP_BEZIER_XY_BH;
						break;
					case CSI_BEZIER_WANTNHX:
					case CSI_BEZIER_WANTNHY:
						{
							tosetpindex = CSP_BEZIER_XY_NH;
							float nx, ny;
							pcommand->GetParamXY(CSP_BEZIER_XY_NA, &nx, &ny);
							pickx = 2*nx-pickx;
							picky = 2*ny-picky;
						}
						break;
					}
					if (tosetpindex >= 0)
					{
						GAnchorPoint * pBindAnchor = step < CSI_BEZIER_WANTNHX ? pBindAnchorBegin : pBindAnchorEnd;
						if (pBindAnchor)
						{
							GHandlePoint * pHandle = pBindAnchor->GetHandle();
							float hx = pHandle->getX();
							float hy = pHandle->getY();
							pcommand->SetParamX(tosetpindex, 2*pickx-hx, CWP_HANDLEX);
							pcommand->SetParamY(tosetpindex, 2*picky-hy, CWP_HANDLEY);
						}
						else
						{
							pcommand->SetParamX(tosetpindex, pickx, CWP_HANDLEX);
							pcommand->SetParamY(tosetpindex, picky, CWP_HANDLEY);
						}
					}
					switch (tosetpindex)
					{
					case CSP_BEZIER_XY_BH:
						pcommand->StepTo(CSI_BEZIER_WANTNAX, CWP_XY_N);
						break;
					case CSP_BEZIER_XY_NH:
						pcommand->StepTo(CSI_FINISHCONTINUE);
						break;
					}
				}
			}

		}
		else if (step == CSI_FINISHCONTINUE)
		{
			float nx1, ny1;
			float nhx1, nhy1;
			pcommand->GetParamXY(CSP_BEZIER_XY_NA, &nx1, &ny1);
			pcommand->GetParamXY(CSP_BEZIER_XY_NH, &nhx1, &nhy1);
			nhx1 = 2*nx1-nhx1;
			nhy1 = 2*ny1-nhy1;

			ProtectPendingFinishCommand();

			CommitFrontCommand(
				CCMake_C(COMM_BEZIER),
				CCMake_F(nx1),
				CCMake_F(ny1),
				CCMake_F(nhx1),
				CCMake_F(nhy1),
				NULL
				);

			if (pNCLine)
			{
				if (!pMergeToEnd)
				{
					pMergeToEnd = TestPickObjSingleFilter(pNCLine->GetEndPoint(), pNCLine, &fProportionEnd);
				}
				if (pNextMergeToBegin)
				{
					pMergeToBegin = pNextMergeToBegin;
				}
				else if (!pMergeToBegin)
				{
					pMergeToBegin = TestPickObjSingleFilter(pNCLine->GetBeginPoint(), pNCLine, &fProportionBegin);
				}
				if (pMergeToEnd)
				{
					MergeClingNewPoint(pNCLine->GetEndPoint(), pMergeToEnd, fProportionEnd);
				}
				if (pMergeToBegin)
				{
					MergeClingNewPoint(pNCLine->GetBeginPoint(), pMergeToBegin, fProportionBegin);
				}
				pNextMergeToBegin = pNCLine->GetEndPoint();
				
				if (!pBindAnchorEnd)
				{
					if (pMergeToEnd)
					{
						if (pMergeToEnd->isAnchorPoint())
						{
							MathHelper * pmh = &MathHelper::getInstance();
							GAnchorPoint * pMergeToEndAnchor = (GAnchorPoint *)pMergeToEnd;

							int nToAngle = pmh->GetLineAngle(pMergeToEndAnchor->GetHandle()->GetPointF2D(), pMergeToEndAnchor->GetPointF2D());
							int nFromAngle = pmh->GetLineAngle(pNCLine->GetEndPoint()->GetPointF2D(), pNCLine->GetEndPoint()->GetHandle()->GetPointF2D());
							if (pmh->AreTwoAngleClose(nFromAngle, nToAngle))
							{
								pBindAnchorEnd = (GAnchorPoint *)pMergeToEnd;
							}
						}
					}
				}
				if (!pBindAnchorBegin)
				{
					if (pMergeToBegin)
					{
						if (pMergeToBegin->isAnchorPoint())
						{
							MathHelper * pmh = &MathHelper::getInstance();
							GAnchorPoint * pMergeToBeginAnchor = (GAnchorPoint *)pMergeToBegin;

							int nToAngle = pmh->GetLineAngle(pMergeToBeginAnchor->GetHandle()->GetPointF2D(), pMergeToBeginAnchor->GetPointF2D());
							int nFromAngle = pmh->GetLineAngle(pNCLine->GetBeginPoint()->GetPointF2D(), pNCLine->GetBeginPoint()->GetHandle()->GetPointF2D());
							if (pmh->AreTwoAngleClose(nFromAngle, nToAngle))
							{
								pBindAnchorBegin = (GAnchorPoint *)pMergeToBegin;
							}
						}
					}
				}
				
				if (pBindAnchorEnd)
				{
					BindNewAnchorPoint(pNCLine->GetEndPoint(), pBindAnchorEnd);
				}
				if (pBindAnchorBegin)
				{
					BindNewAnchorPoint(pNCLine->GetBeginPoint(), pBindAnchorBegin);
				}
			}
		}
	}

	if (step >= CSI_BEZIER_WANTBHX)
	{
		float xb, yb, bhx, bhy, xn, yn, nhx, nhy;
		float tx = pgp->GetPickX_C();
		float ty = pgp->GetPickY_C();

		bDrawTempBezierLine=false;
		bDrawTempLineHandle=false;

		switch (step)
		{
			// No Break
		case CSI_BEZIER_WANTNHX:
		case CSI_BEZIER_WANTNHY:
			pcommand->GetParamXY(CSP_BEZIER_XY_NA, &xn, &yn);
			bDrawTempLineHandle = true;
		case CSI_BEZIER_WANTNAX:
		case CSI_BEZIER_WANTNAY:
			pcommand->GetParamXY(CSP_BEZIER_XY_BH, &bhx, &bhy);
			bDrawTempBezierLine = true;
		case CSI_BEZIER_WANTBHX:
		case CSI_BEZIER_WANTBHY:
			pcommand->GetParamXY(CSP_BEZIER_XY_BA, &xb, &yb);
		}
		if (!bDrawTempLineHandle)
		{
			if (!bDrawTempBezierLine)
			{
				bhx = tx;
				bhy = ty;

				xn = nhx = xb;
				yn = nhy = yb;
			}
			else
			{
				xn = tx;
				yn = ty;

				nhx = tx;
				nhy = ty;
			}
		}
		else
		{
			nhx = 2*xn-tx;
			nhy = 2*yn-ty;
		}
		if (!pTempLine)
		{
			pTempLine = new GBezierLine(&tBaseNode, PointF2D(), PointF2D());
		}
		pTempLine->SetBeginEnd(xb, yb, xn, yn, 0);
		pTempLine->SetBeginHandlePos(bhx, bhy, 0);
		pTempLine->SetEndHandlePos(nhx, nhy, 0);
	}
	if (step >= CSI_BEZIER_WANTNAX)
	{
		if (!pMarking)
		{
			pMarking = new MarkingLine(pTempLine, MARKFLAG_LENGTH);
			pMarking->getMarkingUI(MARKFLAG_LENGTH)->SetEditable(false);
			pMarking->getMarkingUI(MARKFLAG_LENGTH)->SetCallback(staticMIDCBLength);
			MarkingManager::getInstance().EnableMarking(pMarking);
		}
	}
	if (step >= CSI_BEZIER_WANTNHX)
	{
		if (pMarking)
		{
			pMarking->getMarkingUI(MARKFLAG_LENGTH)->SetEditable(true);
		}
	}
	else
	{
		if (pMarking)
		{
			pMarking->getMarkingUI(MARKFLAG_LENGTH)->SetEditable(false);
		}
	}

	RenderToTarget();
}

void BezierCommand::OnDoneCommand()
{
	float xb, yb, xe, ye;
	float xbh, ybh, xeh, yeh;

	pcommand->GetParamXY(CSP_BEZIER_XY_BA, &xb, &yb);
	pcommand->GetParamXY(CSP_BEZIER_XY_BH, &xbh, &ybh);
	pcommand->GetParamXY(CSP_BEZIER_XY_NA, &xe, &ye);
	pcommand->GetParamXY(CSP_BEZIER_XY_NH, &xeh, &yeh);

	GBezierLine * pLine = new GBezierLine(pgm->GetActiveLayer(), PointF2D(xb, yb), PointF2D(xbh, ybh), PointF2D(xeh, yeh), PointF2D(xe, ye));
	pNCLine = pLine;

	PushRevertable(
		CCMake_C(COMM_I_ADDNODE, 2),
		CCMake_I(pLine->getID()),
		CCMake_I(pLine->getParent()->getID()),
		CCMake_C(COMM_I_COMMAND, 9, 1),
		CCMake_CI(COMM_I_COMM_WORKINGLAYER, workinglayerID),
		CCMake_C(COMM_BEZIER),
		CCMake_F(xb),
		CCMake_F(yb),
		CCMake_F(xbh),
		CCMake_F(ybh),
		CCMake_F(xe),
		CCMake_F(ye),
		CCMake_F(xeh),
		CCMake_F(yeh),
		CCMake_C(COMM_I_UNDO_COMMIT, 5),
		CCMake_C(COMM_BEZIER),
		CCMake_F(xb),
		CCMake_F(yb),
		CCMake_F(xbh),
		CCMake_F(ybh),
		NULL
		);
	ClearTemp();
}

void BezierCommand::RenderToTarget()
{
	int nstep = pcommand->GetStep();

	if (nstep >= CSI_BEZIER_WANTBHX && nstep <= CSI_BEZIER_WANTNHY && pTempLine)
	{
		HTARGET tar = RenderTargetManager::getInstance().UpdateTarget(RTID_COMMAND);
		prh->BeginRenderTar(tar);

		DWORD col = pTempLine->getLineColor();

		PointF2D ptBegin = pTempLine->GetBeginPoint()->GetPointF2D();
		PointF2D ptBeginHandle = pTempLine->GetBeginPoint()->GetHandle()->GetPointF2D();
		PointF2D ptNext = pTempLine->GetEndPoint()->GetPointF2D();
		PointF2D ptNextHandle = pTempLine->GetEndPoint()->GetHandle()->GetPointF2D();

		float minxb = 2*ptBegin.x-ptBeginHandle.x;
		float minyb = 2*ptBegin.y-ptBeginHandle.y;
		prh->RenderLine(ptBegin.x, ptBegin.y, ptBeginHandle.x, ptBeginHandle.y, col);
		prh->RenderLine(ptBegin.x, ptBegin.y, minxb, minyb, col);
		prh->RenderAttributePoint(ptBegin.x, ptBegin.y, col);
		prh->RenderHandlePoint(ptBeginHandle.x, ptBeginHandle.y, col);
		prh->RenderHandlePoint(minxb, minyb, col);

		if (bDrawTempBezierLine)
		{
			pTempLine->CallRender();
		}
		if (bDrawTempLineHandle)
		{
			float minxn = 2*ptNext.x-ptNextHandle.x;
			float minyn = 2*ptNext.y-ptNextHandle.y;
			prh->RenderLine(ptNext.x, ptNext.y, ptNextHandle.x, ptNextHandle.y, col);
			prh->RenderLine(ptNext.x, ptNext.y, minxn, minyn, col);
			prh->RenderAttributePoint(ptNext.x, ptNext.y, col);
			prh->RenderHandlePoint(ptNextHandle.x, ptNextHandle.y, col);
			prh->RenderHandlePoint(minxn, minyn, col);
		}
		prh->EndRenderTar();

		pcommand->SetRenderTarget(tar);

	}

}

void BezierCommand::OnInitCommand()
{
	pMergeToBegin = NULL;
	pMergeToEnd = NULL;
	pNCLine = NULL;
	pBindAnchorBegin = NULL;
	pBindAnchorEnd = NULL;

	ClearTemp();
}

void BezierCommand::OnTerminalCommand()
{
	pNextMergeToBegin = NULL;
	ClearTemp();
}

void BezierCommand::ClearTemp()
{
	if (pTempLine)
	{
		pTempLine->RemoveFromParent(true);
		pTempLine = NULL;
		pMarking = NULL;
	}
	pgp->UnlockLength();
	bDrawTempBezierLine = false;
	bDrawTempLineHandle = false;
}

bool BezierCommand::staticMIDCBLength( MarkingUI * pmui, bool bAccept )
{
	return BezierCommand::getInstance().MIDCBLength(pmui, bAccept);
}

bool BezierCommand::MIDCBLength( MarkingUI * pmui, bool bAccept )
{
	GObjectPicker * pgp = &GObjectPicker::getInstance();
	if (pmui->IsValueLocked())
	{
		bool bOk;
		float fLockedLength = pmui->getFloat(&bOk);
		if (bOk)
		{
			bool bRet = MathHelper::getInstance().FindNearestHandlePointForGivenBezierLength(
				fLockedLength, pTempLine->GetBeginPoint()->GetPointF2D(), pTempLine->GetBeginPoint()->GetHandle()->GetPointF2D(),
				pTempLine->GetEndPoint()->GetPointF2D(),
				0, 0, NULL, NULL, true);
			if (!bRet)
			{
				pgp->UnlockLength();
				return false;
			}
			else
			{
				pgp->SetLockOrigin(pTempLine->GetBeginPoint()->getX(), pTempLine->GetBeginPoint()->getY());
				pgp->SetLockLength(fLockedLength, pTempLine->GetEndPoint()->GetHandle(), true);
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		pgp->UnlockLength();
	}
	return true;
}

void BezierCommand::OnClearCommand()
{
	ClearTemp();
}