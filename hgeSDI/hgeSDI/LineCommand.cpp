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
	
	bool bok = false;

	if (step == CSI_INIT)
	{
		pcommand->StepTo(
			CSI_LINE_WANTX1, 
			CWP_XY_B);
	}
	else if (step == CSI_LINE_WANTX1)
	{
		bok = pcommand->ProcessPending(
			CSP_LINE_B_XY, COMMPARAMFLAG_X, CWP_X_B,	// Fill in Param
			CSI_LINE_WANTY1, CWP_Y_B					// Step to with want
			);
	}
	else if (step == CSI_LINE_WANTY1)
	{
		bok = pcommand->ProcessPending(
			CSP_LINE_B_XY, COMMPARAMFLAG_Y, CWP_Y_B,	// Fill in Param
			CSI_LINE_WANTX2, CWP_XY_E					// Step to with want
			);
	}
	else if (step == CSI_LINE_WANTX2)
	{
		pcommand->ProcessPending(
			CSP_LINE_E_XY, COMMPARAMFLAG_X, CWP_X_E,	// Fill in Param
			CSI_LINE_WANTY2, CWP_Y_E					// Step to with want
			);
	}
	else if (step == CSI_LINE_WANTY2)
	{
		pcommand->ProcessPending(
			CSP_LINE_E_XY, COMMPARAMFLAG_Y, CWP_Y_E,	// Fill in Param
			CSI_FINISHCONTINUE							// Step to with want
			);
	}

	if (!bok)
	{
		if (step > CSI_INIT)
		{
			if (pmain->DoPickPoint())
			{
				if (!pcommand->IsInternalProcessing())
				{
					int tosetpindex = CSP_LINE_B_XY;
					if (step == CSI_LINE_WANTX2 || step == CSI_LINE_WANTY2)
					{
						tosetpindex = CSP_LINE_E_XY;
					}
					pcommand->SetParamX(tosetpindex, pguic->StoCx(pmain->pickx));
					pcommand->SetParamY(tosetpindex, pguic->StoCy(pmain->picky));
					if (step < CSI_LINE_WANTX2)
					{
						pcommand->StepTo(
							CSI_LINE_WANTX2, CWP_XY_E
							);
					}
					else
					{
						pcommand->StepTo(
							CSI_FINISHCONTINUE
							);
					}

					bok = true;
				}
			}
		}
		else if (step == CSI_FINISHCONTINUE)
		{
			float nx1, ny1;
			pcommand->GetParamXY(CSP_LINE_E_XY, &nx1, &ny1);

			pcommand->FinishCommand();

			pcommand->CommitFrontCommandF(ny1);
			pcommand->CommitFrontCommandF(nx1);
			pcommand->CommitFrontCommandC(COMM_LINE);
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
		RenderHelper::getInstance().RenderLine(x1, y1, x2, y2, ColorManager::GetLayerLineColor());
		RenderHelper::getInstance().EndRenderTar();

		Command::getInstance().SetRenderTarget(tar);
	}
}

void LineCommand::DoneCommand()
{
	GStraightLine * line = new GStraightLine(&(GObjectManager::getInstance().basenode));
	float xb, yb, xe, ye;
	pcommand->GetParamXY(CSP_LINE_B_XY, &xb, &yb);
	pcommand->GetParamXY(CSP_LINE_E_XY, &xe, &ye);
	line->SetBeginEnd(xb, yb, xe, ye);
}