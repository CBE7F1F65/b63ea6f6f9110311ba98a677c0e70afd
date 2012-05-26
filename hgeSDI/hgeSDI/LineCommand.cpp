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
	switch (pcommand->GetStep())
	{
	case CSI_INIT:

		GUICursor::getInstance().ChangeCursor(GUIC_CREATEPOINT);
		pcommand->StepTo(
			CSI_LINE_WANTX1, 
			CWP_XY_B);
		break;

	case CSI_LINE_WANTX1:

		if (pcommand->ProcessPending(
			CSP_LINE_B_XY, COMMPARAMFLAG_X, CWP_X_B,	// Fill in Param
			CSI_LINE_WANTY1, CWP_Y_B					// Step to with want
			))
		{

		}
		else if (pmain->DoPickPoint())
		{
			if (!pcommand->IsInternalProcessing())
			{
				pcommand->SetParamX(CSP_LINE_B_XY, pguic->StoCx(pmain->pickx));
				pcommand->SetParamY(CSP_LINE_B_XY, pguic->StoCy(pmain->picky));
				pcommand->StepTo(
					CSI_LINE_WANTX2, CWP_XY_B
					);
			}
		}
		break;

	case CSI_LINE_WANTY1:

		if (pcommand->ProcessPending(
			CSP_LINE_B_XY, COMMPARAMFLAG_Y, CWP_Y_B,	// Fill in Param
			CSI_LINE_WANTX2, CWP_XY_E					// Step to with want
			))
		{

		}
		else if (pmain->DoPickPoint())
		{
			if (!pcommand->IsInternalProcessing())
			{
				pcommand->SetParamX(CSP_LINE_B_XY,  pguic->StoCx(pmain->pickx));
				pcommand->SetParamY(CSP_LINE_B_XY,  pguic->StoCy(pmain->picky));
				pcommand->StepTo(
					CSI_LINE_WANTX2, 
					CWP_XY_B);
			}
		}
		break;

	case CSI_LINE_WANTX2:

		if (pcommand->ProcessPending(
			CSP_LINE_E_XY, COMMPARAMFLAG_X, CWP_X_E,	// Fill in Param
			CSI_LINE_WANTY2, CWP_Y_E					// Step to with want
			))
		{

		}
		else if (pmain->DoPickPoint())
		{
			if (!pcommand->IsInternalProcessing())
			{
				pcommand->SetParamX(CSP_LINE_E_XY, pguic->StoCx(pmain->pickx));
				pcommand->SetParamY(CSP_LINE_E_XY, pguic->StoCy(pmain->picky));
				pcommand->StepTo(CSI_FINISH);
			}
		}
		break;

	case CSI_LINE_WANTY2:

		if (pcommand->ProcessPending(
			CSP_LINE_E_XY, COMMPARAMFLAG_Y, CWP_Y_E,	// Fill in Param
			CSI_FINISH								// Step to with want
			))
		{

		}
		if (pmain->DoPickPoint())
		{
			if (!pcommand->IsInternalProcessing())
			{
				pcommand->SetParamX(CSP_LINE_E_XY, pguic->StoCx(pmain->pickx));
				pcommand->SetParamY(CSP_LINE_E_XY, pguic->StoCy(pmain->picky));
				pcommand->StepTo(CSI_FINISH);
			}
		}
		break;
	case CSI_PAUSE:
//		GUICursor::getInstance().ChangeCursor();
		ReleaseTarget();
		break;
	case CSI_RESUME:
		GUICursor::getInstance().ChangeCursor(GUIC_CREATEPOINT);
		break;

	case CSI_FINISH:
//		GUICursor::getInstance().ChangeCursor();
		ReleaseTarget();
		DoneCommand();
		if (true)
		{
			float nx1, ny1;
			pcommand->GetParamXY(CSP_LINE_E_XY, &nx1, &ny1);

			pcommand->FinishCommand();

			pcommand->CommitFrontCommandF(ny1);
			pcommand->CommitFrontCommandF(nx1);
			pcommand->CommitFrontCommandC(COMM_LINE);
			/*
			pcommand->CreateCommand(COMM_LINE);
			pcommand->StepTo(
				CSI_LINE_WANTX1,
				CWP_XY_B);
			pcommand->SetParamX(CSP_LINE_B_XY, nx1);
			pcommand->StepTo(
				CSI_LINE_WANTY1,
				CWP_Y_B);
			pcommand->SetParamY(CSP_LINE_B_XY, ny1);
			pcommand->StepTo(
				CSI_LINE_WANTX2,
				CWP_XY_E);
				*/
		}

		break;

	case CSI_TERMINAL:
		GUICursor::getInstance().ChangeCursor(GUIC_NORMAL);
		ReleaseTarget();
		pcommand->TerminalCommand();
		break;
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

		RenderHelper::BeginRenderTar(tar);
		RenderHelper::RenderLine(x1, y1, x2, y2, ColorManager::GetLayerLineColor());
		RenderHelper::EndRenderTar();

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