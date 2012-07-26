#include "stdafx.h"
#include "Command.h"
#include "LineCommand.h"
#include "MainInterface.h"

#include "GUICursor.h"
#include "GUICoordinate.h"
#include "RenderHelper.h"
#include "RenderTargetManager.h"
#include "ColorManager.h"
#include "MarkingObject.h"
#include "MarkingManager.h"

#include "GLine.h"
#include "GObjectManager.h"

#include "PickFilterTemplate.h"

LineCommand::LineCommand()
{
	pNextMergeToBegin = NULL;

	pTempLine = NULL;
}

LineCommand::~LineCommand()
{
//	ClearTemp();
}
void LineCommand::OnProcessCommand()
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
			CSI_LINE_WANTX1,
			CWP_XY_B);
	}
	else if (step == CSI_LINE_WANTX1)
	{
		ret = pcommand->ProcessPending(
			CSP_LINE_XY_B, COMMPARAMFLAG_X, CWP_X_B,	// Fill in Param
			CSI_LINE_WANTY1, CWP_Y_B					// Step to with want
			);
	}
	else if (step == CSI_LINE_WANTY1)
	{
		ret = pcommand->ProcessPending(
			CSP_LINE_XY_B, COMMPARAMFLAG_Y, CWP_Y_B,	// Fill in Param
			CSI_LINE_WANTX2, CWP_XY_N					// Step to with want
			);
	}
	else if (step == CSI_LINE_WANTX2)
	{
		ret = pcommand->ProcessPending(
			CSP_LINE_XY_N, COMMPARAMFLAG_X, CWP_X_N,	// Fill in Param
			CSI_LINE_WANTY2, CWP_Y_N					// Step to with want
			);
	}
	else if (step == CSI_LINE_WANTY2)
	{
		ret = pcommand->ProcessPending(
			CSP_LINE_XY_N, COMMPARAMFLAG_Y, CWP_Y_N,	// Fill in Param
			CSI_FINISHCONTINUE							// Step to with want
			);
	}

	if (step >= CSI_LINE_WANTX2)
	{
		float bx, by;
		pcommand->GetParamXY(CSP_LINE_XY_B, &bx, &by);
		pgp->PushInterestPoint(bx, by);
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
			int iret = pgp->PickPoint();
			if (pgp->IsPickReady(iret))
			{
				if (!pcommand->IsInternalProcessing())
				{
					int tosetpindex=-1;
					switch (step)
					{
					case CSI_LINE_WANTX1:
					case CSI_LINE_WANTY1:
						tosetpindex = CSP_LINE_XY_B;
						pMergeToBegin = pgp->GetPickedObj();
						if (pMergeToBegin)
						{
							fProportionBegin = pgp->CalculateProportion();
						}
						break;
					case CSI_LINE_WANTX2:
					case CSI_LINE_WANTY2:
						tosetpindex = CSP_LINE_XY_N;
						pMergeToEnd = pgp->GetPickedObj();
						if (pMergeToEnd)
						{
							fProportionEnd = pgp->CalculateProportion();
						}
						break;
					}
					pcommand->SetParamX(tosetpindex, pgp->GetPickX_C(), CWP_X);
					pcommand->SetParamY(tosetpindex, pgp->GetPickY_C(), CWP_Y);
					if (step < CSI_LINE_WANTX2)
					{
						pcommand->StepTo(
							CSI_LINE_WANTX2, CWP_XY_N
							);
					}
					else
					{
						pcommand->StepTo(
							CSI_FINISHCONTINUE
							);
					}

				}
			}
		}
		else if (step == CSI_FINISHCONTINUE)
		{
			float nx1, ny1;
			pcommand->GetParamXY(CSP_LINE_XY_N, &nx1, &ny1);

			ProtectPendingFinishCommand();

			CommitFrontCommand(
				CCMake_C(COMM_LINE),
				CCMake_F(nx1),
				CCMake_F(ny1),
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
			}
		}
	}

	if (step >= CSI_LINE_WANTX2)
	{
		float x1, y1;
		pcommand->GetParamXY(CSP_LINE_XY_B, &x1, &y1);

		float x2 = pgp->GetPickX_C();
		float y2 = pgp->GetPickY_C();

		if (!pTempLine)
		{
			pTempLine = new GBezierLine(&tBaseNode, PointF2D(), PointF2D());
			MarkingLine * pMarking = new MarkingLine(pTempLine, MARKFLAG_LENGTH|MARKFLAG_ANGLE);
			MarkingUI * pmuiLength = pMarking->getMarkingUI(MARKFLAG_LENGTH);
			MarkingUI * pmuiAngle = pMarking->getMarkingUI(MARKFLAG_ANGLE);
			pmuiLength->SetEditable(true);
			pmuiAngle->SetEditable(true);
			pmuiLength->SetCallback(staticMIDCBLength);
			pmuiAngle->SetCallback(staticMIDCBAngle);
			MarkingManager::getInstance().EnableMarking(pMarking);
		}

		pTempLine->SetBeginEnd(x1, y1, x2, y2, 0);
	}

	RenderToTarget();
}

void LineCommand::RenderToTarget()
{
	int nstep = pcommand->GetStep();
	if (nstep >= CSI_LINE_WANTX2 && nstep <= CSI_LINE_WANTY2 && pTempLine)
	{
		HTARGET tar = RenderTargetManager::getInstance().UpdateTarget(RTID_COMMAND);

		prh->BeginRenderTar(tar);
		pTempLine->CallRender();
		prh->EndRenderTar();

		pcommand->SetRenderTarget(tar);
	}
}

void LineCommand::OnDoneCommand()
{
	float xb, yb, xe, ye;
	pcommand->GetParamXY(CSP_LINE_XY_B, &xb, &yb);
	pcommand->GetParamXY(CSP_LINE_XY_N, &xe, &ye);
	GStraightLine * pLine = new GBezierLine(pgm->GetActiveLayer(), PointF2D(xb, yb), PointF2D(xe, ye));
	pNCLine = pLine;

	PushRevertable(
		CCMake_C(COMM_I_ADDNODE, 2),
// 		CCMake_D((int)line),
// 		CCMake_D((int)(line->getParent())),
		CCMake_I(pLine->getID()),
		CCMake_I(pLine->getParent()->getID()),
		CCMake_C(COMM_I_COMMAND, 5, 1),
		CCMake_CI(COMM_I_COMM_WORKINGLAYER, workinglayerID),
		CCMake_C(COMM_LINE),
		CCMake_F(xb),
		CCMake_F(yb),
		CCMake_F(xe),
		CCMake_F(ye),
		CCMake_C(COMM_I_UNDO_COMMIT, 3),
		CCMake_C(COMM_LINE),
		CCMake_F(xb),
		CCMake_F(yb),
		NULL
		);
	ClearTemp();
}

bool LineCommand::staticMIDCBLength(MarkingUI * pmui, bool bAccept)
{
	return LineCommand::getInstance().MIDCBLength(pmui, bAccept);
}
bool LineCommand::staticMIDCBAngle(MarkingUI * pmui, bool bAccept)
{
	return LineCommand::getInstance().MIDCBAngle(pmui, bAccept);
}

bool LineCommand::MIDCBLength( MarkingUI * pmui, bool bAccept )
{
	GObjectPicker * pgp = &GObjectPicker::getInstance();
	if (pmui->IsValueLocked())
	{
		bool bOk;
		float fLockedLength = pmui->getFloat(&bOk);
		if (bOk)
		{
			float x1, y1;
			pcommand->GetParamXY(CSP_LINE_XY_B, &x1, &y1);
			pgp->SetLockOrigin(x1, y1);
			pgp->SetLockLength(fLockedLength);
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

bool LineCommand::MIDCBAngle( MarkingUI * pmui, bool bAccept )
{
	GObjectPicker * pgp = &GObjectPicker::getInstance();
	if (pmui->IsValueLocked())
	{
		bool bOk;
		float fAngle = pmui->getFloat(&bOk);
		if (bOk)
		{
			int nLockedAngle = fAngle * ANGLEBASE_90/90;
			float x1, y1;
			pcommand->GetParamXY(CSP_LINE_XY_B, &x1, &y1);
			pgp->SetLockOrigin(x1, y1);
			pgp->SetLockAngle(nLockedAngle);
		}
		else
		{
			return false;
		}
	}
	else
	{
		pgp->UnlockAngles();
	}
	return true;
}

void LineCommand::OnInitCommand()
{
	pMergeToBegin = NULL;
	pMergeToEnd = NULL;
	pNCLine = NULL;

	ClearTemp();
}

void LineCommand::OnTerminalCommand()
{
	pNextMergeToBegin = NULL;
	ClearTemp();
}

void LineCommand::ClearTemp()
{
	if (pTempLine)
	{
		pTempLine->RemoveFromParent(true);
		pTempLine = NULL;
	}
	pgp->UnlockLength();
	pgp->UnlockAngles();
}

void LineCommand::OnClearCommand()
{
	ClearTemp();
}