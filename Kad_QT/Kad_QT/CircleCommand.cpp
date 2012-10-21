#include "StdAfx.h"
#include "CircleCommand.h"
#include "GUICursor.h"
#include "MarkingManager.h"
#include "RenderTargetManager.h"

CircleCommand::CircleCommand(void)
{
}


CircleCommand::~CircleCommand(void)
{
}

void CircleCommand::OnProcessCommand()
{
	int step = OnNormalProcessCommand(GUIC_CREATEPOINT);

	int nowstep = pcommand->GetStep();
	UpdateLastStep();

	int ret = -1;

	if (step == CSI_INIT)
	{
		pcommand->StepTo(
			CSI_CIRCLE_WANTX,
			CWP_XY);
	}
	else if (step == CSI_CIRCLE_WANTX)
	{
		ret = pcommand->ProcessPending(
			CSP_CIRCLE_XY_F_CENTER_R, COMMPARAMFLAG_X, CWP_X,	// Fill in Param
			CSI_CIRCLE_WANTY, CWP_Y					// Step to with want
			);
	}
	else if (step == CSI_CIRCLE_WANTY)
	{
		ret = pcommand->ProcessPending(
			CSP_CIRCLE_XY_F_CENTER_R, COMMPARAMFLAG_Y, CWP_Y,	// Fill in Param
			CSI_CIRCLE_WANTR, CWP_R					// Step to with want
			);
	}
	else if (step == CSI_CIRCLE_WANTR)
	{
		ret = pcommand->ProcessPending(
			CSP_CIRCLE_XY_F_CENTER_R, COMMPARAMFLAG_F, CWP_R,	// Fill in Param
			CSI_FINISH					// Step to with want
			);
	}

	if (step >= CSI_CIRCLE_WANTR)
	{
		float bx, by;
		pcommand->GetParamXY(CSP_CIRCLE_XY_F_CENTER_R, &bx, &by);
		pgp->PushInterestPoint(bx, by);
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
					if (step == CSI_CIRCLE_WANTX || step == CSI_CIRCLE_WANTY)
					{
						pcommand->SetParamX(CSP_CIRCLE_XY_F_CENTER_R, pgp->GetPickX_C(), CWP_X);
						pcommand->SetParamY(CSP_CIRCLE_XY_F_CENTER_R, pgp->GetPickY_C(), CWP_Y);
						pcommand->StepTo(CSI_CIRCLE_WANTR, CWP_R);
					}
					else
					{
						float bx, by;
						pcommand->GetParamXY(CSP_CIRCLE_XY_F_CENTER_R, &bx, &by);
						float r = MathHelper::getInstance().LineSegmentLength(PointF2D(pgp->GetPickX_C(), pgp->GetPickY_C()), PointF2D(bx, by));
						pcommand->SetParamF(CSP_CIRCLE_XY_F_CENTER_R, r, CWP_R);
						pcommand->StepTo(CSI_FINISH);
					}
				}
			}
		}
	}

	if (step >= CSI_CIRCLE_WANTR)
	{
		float x, y;
		pcommand->GetParamXY(CSP_CIRCLE_XY_F_CENTER_R, &x, &y);


		float xr = pgp->GetPickX_C();
		float yr = pgp->GetPickY_C();

		float r = MathHelper::getInstance().LineSegmentLength(PointF2D(x, y), PointF2D(xr, yr));

		if (!pTempRLine)
		{
			pTempRLine = new GBezierLine(&tBaseNode, PointF2D(), PointF2D());
			MarkingLine * pMarking = new MarkingLine(pTempRLine, MARKFLAG_LENGTH);
			MarkingUI * pmuiLength = pMarking->getMarkingUI(MARKFLAG_LENGTH);
			pmuiLength->SetEditable(true);
			pmuiLength->SetCallback(staticMIDCBLength);
			MarkingManager::getInstance().EnableMarking(pMarking);
		}
		pTempRLine->SetBeginEnd(x, y, xr, yr);

		for (int i=0; i<QUADRANTMAX; i++)
		{
			if (!pTempLines[i])
			{
				pTempLines[i] = new GBezierLine(&tBaseNode, PointF2D(), PointF2D());
			}
			pTempLines[i]->SetPosByQuarterCircle(x, y, r, i);
		}
	}
}

void CircleCommand::OnDoneCommand()
{
	float x, y;
	pcommand->GetParamXY(CSP_CIRCLE_XY_F_CENTER_R, &x, &y);
	float r = pcommand->GetParamF(CSP_CIRCLE_XY_F_CENTER_R);

	GBezierLine * pLine[4];
	for (int i=0; i<QUADRANTMAX; i++)
	{
		pLine[i] = new GBezierLine(pgm->GetActiveLayer(), x, y, r, i);
	}
	for (int i=0; i<QUADRANTMAX; i++)
	{
		int inext = (i+1)%QUADRANTMAX;
		pLine[i]->GetEndPoint()->MergeWith(pLine[inext]->GetBeginPoint());
	}

	PushRevertible(
		CCMake_C(COMM_I_COMMAND, 4),
		CCMake_CI(COMM_I_COMM_WORKINGLAYER, workinglayerID),
		CCMake_C(COMM_CIRCLE),
		CCMake_F(x),
		CCMake_F(y),
		CCMake_F(r),
		NULL
		);
}

void CircleCommand::OnClearTemp()
{
	if (pTempRLine)
	{
		pTempRLine->RemoveFromParent(true);
		pTempRLine = NULL;
	}
	for (int i=0; i<QUADRANTMAX; i++)
	{
		if (pTempLines[i])
		{
			pTempLines[i]->RemoveFromParent(true);
			pTempLines[i] = NULL;
		}
	}
}

void CircleCommand::RenderToTarget()
{
	int nstep = pcommand->GetStep();
	if (nstep == CSI_CIRCLE_WANTR && pTempLines[0])
	{
		HTARGET tar = RenderTargetManager::getInstance().UpdateTarget(RTID_COMMAND);

		prh->BeginRenderTar(tar);
		for (int i=0; i<QUADRANTMAX; i++)
		{
			if (pTempLines[i])
			{
				pTempLines[i]->CallRender();
			}
		}
		prh->EndRenderTar();

		pcommand->SetRenderTarget(tar);
	}
}

bool CircleCommand::staticMIDCBLength(MarkingUI * pmui, bool bAccept)
{
	return getInstance().MIDCBLength(pmui, bAccept);
}

bool CircleCommand::MIDCBLength( MarkingUI * pmui, bool bAccept )
{
	GObjectPicker * pgp = &GObjectPicker::getInstance();
	if (pmui->IsValueLocked())
	{
		bool bOk;
		float fLockedLength = pmui->getFloat(&bOk);
		if (bOk)
		{
			float x, y;
			pcommand->GetParamXY(CSP_CIRCLE_XY_F_CENTER_R, &x, &y);
			pgp->SetLockOrigin(x, y);
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