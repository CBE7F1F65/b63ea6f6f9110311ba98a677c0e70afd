#include "stdafx.h"
#include "Command.h"
#include "LineCommand.h"
#include "Main.h"

#include "GUICursor.h"
#include "GUICoordinate.h"
#include "RenderHelper.h"
#include "RenderTargetManager.h"
#include "ColorManager.h"

#include "GLine.h"
#include "GObjectManager.h"

LineCommand::LineCommand()
{
}
LineCommand::~LineCommand()
{
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
		if (!pgp->isBeginPtSet())
		{
			float bx, by;
			pcommand->GetParamXY(CSP_LINE_XY_B, &bx, &by);
			pgp->SetBeginPt(bx, by);
		}
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
					int tosetpindex = CSP_LINE_XY_B;
					if (step == CSI_LINE_WANTX2 || step == CSI_LINE_WANTY2)
					{
						tosetpindex = CSP_LINE_XY_N;
					}
					pcommand->SetParamX(tosetpindex, pgp->GetPickX_C());
					pcommand->SetParamY(tosetpindex, pgp->GetPickY_C());
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
		}
	}

	RenderToTarget();
}

void LineCommand::RenderToTarget()
{
	int nstep = pcommand->GetStep();
	if (nstep >= CSI_LINE_WANTX2 && nstep <= CSI_LINE_WANTY2)
	{
		float x1, y1;
		pcommand->GetParamXY(CSP_LINE_XY_B, &x1, &y1);

		float x2 = pgp->GetPickX_C();//MainInterface::getInstance().mousex;
		float y2 = pgp->GetPickY_C();//MainInterface::getInstance().mousey;

		HTARGET tar = RenderTargetManager::getInstance().UpdateTarget(RTID_COMMAND);

		RenderHelper::getInstance().BeginRenderTar(tar);
		RenderHelper::getInstance().RenderLineMeasureMark(x1, y1, x2, y2, pgm->GetActiveLayer()->getLineColor());
		RenderHelper::getInstance().RenderLine(x1, y1, x2, y2, pgm->GetActiveLayer()->getLineColor());
		RenderHelper::getInstance().EndRenderTar();

		Command::getInstance().SetRenderTarget(tar);
	}
}

void LineCommand::OnDoneCommand()
{
	float xb, yb, xe, ye;
	pcommand->GetParamXY(CSP_LINE_XY_B, &xb, &yb);
	pcommand->GetParamXY(CSP_LINE_XY_N, &xe, &ye);
	GStraightLine * line = new GBezierLine(pgm->GetActiveLayer(), PointF2D(xb, yb), PointF2D(xe, ye));

	PushRevertable(
		CCMake_C(COMM_I_ADDNODE, 2),
// 		CCMake_D((int)line),
// 		CCMake_D((int)(line->getParent())),
		CCMake_I(line->getID()),
		CCMake_I(line->getParent()->getID()),
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
}