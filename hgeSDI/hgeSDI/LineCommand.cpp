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
			CSP_LINE_B_XY, COMMPARAMFLAG_X, CWP_X_B,	// Fill in Param
			CSI_LINE_WANTY1, CWP_Y_B					// Step to with want
			);
	}
	else if (step == CSI_LINE_WANTY1)
	{
		ret = pcommand->ProcessPending(
			CSP_LINE_B_XY, COMMPARAMFLAG_Y, CWP_Y_B,	// Fill in Param
			CSI_LINE_WANTX2, CWP_XY_N					// Step to with want
			);
	}
	else if (step == CSI_LINE_WANTX2)
	{
		ret = pcommand->ProcessPending(
			CSP_LINE_N_XY, COMMPARAMFLAG_X, CWP_X_N,	// Fill in Param
			CSI_LINE_WANTY2, CWP_Y_N					// Step to with want
			);
	}
	else if (step == CSI_LINE_WANTY2)
	{
		ret = pcommand->ProcessPending(
			CSP_LINE_N_XY, COMMPARAMFLAG_Y, CWP_Y_N,	// Fill in Param
			CSI_FINISHCONTINUE							// Step to with want
			);
	}

	if (!ret/* || nowstep == CSI_FINISHCONTINUE*/)
	{
		// Pending Processed
		nowstep = pcommand->GetStep();
		if (nowstep == CSI_LINE_WANTX2)
		{
			float x1, y1;
			pcommand->GetParamXY(CSP_LINE_B_XY, &x1, &y1);
			/*
			PushRevertable(
				CCMake_C(COMM_I_COMMAND, 3),
				CCMake_C(COMM_LINE),
				CCMake_F(x1),
				CCMake_F(y1),
				NULL
				);
				*/
		}
		/*else if (nowstep == CSI_FINISHCONTINUE)
		{
			float x1, y1, x2, y2;
			pcommand->GetParamXY(CSP_LINE_B_XY, &x1, &y1);
			pcommand->GetParamXY(CSP_LINE_N_XY, &x2, &y2);
			PushRevertable(
				CCMake_C(COMM_LINE),
				CCMake_F(x1),
				CCMake_F(y1),
				CCMake_F(x2),
				CCMake_F(y2),
				NULL
				);
		}*/
	}
	if (ret > 0)
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
			if (pmain->DoPickPoint())
			{
				if (!pcommand->IsInternalProcessing())
				{
					int tosetpindex = CSP_LINE_B_XY;
					if (step == CSI_LINE_WANTX2 || step == CSI_LINE_WANTY2)
					{
						tosetpindex = CSP_LINE_N_XY;
					}
					pcommand->SetParamX(tosetpindex, pguic->StoCx(pmain->pickx));
					pcommand->SetParamY(tosetpindex, pguic->StoCy(pmain->picky));
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

					ret = true;
				}
			}
		}
		else if (step == CSI_FINISHCONTINUE)
		{
			float nx1, ny1;
			pcommand->GetParamXY(CSP_LINE_N_XY, &nx1, &ny1);

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
		pcommand->GetParamXY(CSP_LINE_B_XY, &x1, &y1);

		float x2 = MainInterface::getInstance().mousex;
		float y2 = MainInterface::getInstance().mousey;
		x2 = GUICoordinate::getInstance().StoCx(x2);
		y2 = GUICoordinate::getInstance().StoCy(y2);

		HTARGET tar = RenderTargetManager::getInstance().UpdateTarget(RTID_COMMAND);

		RenderHelper::getInstance().BeginRenderTar(tar);
		RenderHelper::getInstance().RenderLine(x1, y1, x2, y2, ColorManager::getInstance().GetLayerLineColor());
		RenderHelper::getInstance().EndRenderTar();

		Command::getInstance().SetRenderTarget(tar);
	}
}

void LineCommand::DoneCommand()
{
	GStraightLine * line = new GStraightLine(&(GObjectManager::getInstance().basenode));
	float xb, yb, xe, ye;
	pcommand->GetParamXY(CSP_LINE_B_XY, &xb, &yb);
	pcommand->GetParamXY(CSP_LINE_N_XY, &xe, &ye);
	line->SetBeginEnd(xb, yb, xe, ye);

	PushRevertable(
		CCMake_C(COMM_I_ADDNODE, 2),
		CCMake_D((int)line),
		CCMake_D((int)(line->parent)),
		CCMake_C(COMM_I_COMMAND, 5, 3),
		CCMake_C(COMM_LINE),
		CCMake_F(xb),
		CCMake_F(yb),
		CCMake_F(xe),
		CCMake_F(ye),
		NULL
		);
}