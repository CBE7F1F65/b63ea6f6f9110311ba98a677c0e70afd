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
					int tosetpindex=-1;
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
						pcommand->SetParamX(tosetpindex, pgp->GetPickX_C());
						pcommand->SetParamY(tosetpindex, pgp->GetPickY_C());
					}
					switch (tosetpindex)
					{
					case CSP_BEZIER_XY_BA:
						pcommand->StepTo(CSI_BEZIER_WANTBHX, CWP_HANDLEX);
						break;
					case CSP_BEZIER_XY_NA:
						pcommand->StepTo(CSI_BEZIER_WANTNHX, CWP_HANDLEX);
						break;
					}
				}
				if (pgp->IsPickReady(iret))
				{
					int tosetpindex=-1;
					float pickx = pgp->GetPickX_C();
					float picky = pgp->GetPickY_C();
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
						pcommand->SetParamX(tosetpindex, pickx);
						pcommand->SetParamY(tosetpindex, picky);
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

	GBezierLine * line = new GBezierLine(pgm->GetActiveLayer(), PointF2D(xb, yb), PointF2D(xbh, ybh), PointF2D(xeh, yeh), PointF2D(xe, ye));

	PushRevertable(
		CCMake_C(COMM_I_ADDNODE, 2),
		CCMake_I(line->getID()),
		CCMake_I(line->getParent()->getID()),
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
}

void BezierCommand::RenderToTarget()
{
	int nstep = pcommand->GetStep();

	if (nstep >= CSI_BEZIER_WANTBHX && nstep <= CSI_BEZIER_WANTNHY)
	{
		float xb, yb, bhx, bhy, xn, yn, nhx, nhy;
		float tx = pgp->GetPickX_C();
		float ty = pgp->GetPickY_C();

		bool linebezier=false;
		bool linenh=false;

		switch (nstep)
		{
			// No Break
		case CSI_BEZIER_WANTNHX:
		case CSI_BEZIER_WANTNHY:
			pcommand->GetParamXY(CSP_BEZIER_XY_NA, &xn, &yn);
			linenh = true;
		case CSI_BEZIER_WANTNAX:
		case CSI_BEZIER_WANTNAY:
			pcommand->GetParamXY(CSP_BEZIER_XY_BH, &bhx, &bhy);
			linebezier = true;
		case CSI_BEZIER_WANTBHX:
		case CSI_BEZIER_WANTBHY:
			pcommand->GetParamXY(CSP_BEZIER_XY_BA, &xb, &yb);
		}
		if (!linenh)
		{
			if (!linebezier)
			{
				bhx = tx;
				bhy = ty;
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

		HTARGET tar = RenderTargetManager::getInstance().UpdateTarget(RTID_COMMAND);
		prh->BeginRenderTar(tar);
		// BA-BH Line
		float minxb = 2*xb-bhx;
		float minyb = 2*yb-bhy;
		DWORD col = pgm->GetActiveLayer()->getLineColor();
		prh->RenderLine(xb, yb, bhx, bhy, col);
		prh->RenderLine(xb, yb, minxb, minyb, col);
		prh->RenderAttributePoint(xb, yb, col);
		prh->RenderHandlePoint(bhx, bhy, col);
		prh->RenderHandlePoint(minxb, minyb, col);

		if (linebezier)
		{
			BezierSublinesInfo bsinfo;
			bsinfo.ResetPoints(PointF2D(xb, yb), PointF2D(bhx, bhy), PointF2D(nhx, nhy), PointF2D(xn, yn), pmain->GetPrecision());
			prh->RenderBezierByInfo(&bsinfo, col);
		}
		if (linenh)
		{
			float minxn = 2*xn-nhx;
			float minyn = 2*yn-nhy;
			prh->RenderLine(xn, yn, nhx, nhy, col);
			prh->RenderLine(xn, yn, minxn, minyn, col);
			prh->RenderAttributePoint(xn, yn, col);
			prh->RenderHandlePoint(nhx, nhy, col);
			prh->RenderHandlePoint(minxn, minyn, col);
		}

		RenderHelper::getInstance().EndRenderTar();

		Command::getInstance().SetRenderTarget(tar);

	}

}