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
int LineCommand::Line()
{
	Command * pcommand = &Command::getInstance();
	MainInterface * pmain = &MainInterface::getInstance();
	GUICoordinate * pguic = &GUICoordinate::getInstance();
	switch (pcommand->GetStep())
	{
	case CSI_INIT:

		GUICursor::getInstance().ChangeCursor(GUIC_CREATEPOINT);
		pcommand->StepTo(
			CSI_LINE_WANTX1, 
			CSP_LINE_B_XY, 
			COMMPARAMFLAG_X, 
			CWP_XY);
		break;

	case CSI_LINE_WANTX1:

		if (pcommand->CheckParamSet(
			CSP_LINE_B_XY, 
			COMMPARAMFLAG_X))
		{
			pcommand->StepTo(
				CSI_LINE_WANTY1, 
				CSP_LINE_B_XY, 
				COMMPARAMFLAG_Y, 
				CWP_XY);
		}
		else if (pmain->DoPickPoint())
		{
			if (!pcommand->IsInternalProcessing())
			{
				float x = pmain->pickx;
				float y = pmain->picky;
				pguic->ClientToCoordinate(&x, &y);
				pcommand->SetParamX(CSP_LINE_B_XY, x);
				pcommand->SetParamY(CSP_LINE_B_XY, y);
				pcommand->StepTo(
					CSI_LINE_WANTX2, 
					CSP_LINE_E_XY, 
					COMMPARAMFLAG_X, 
					CWP_XY);
			}
		}
		break;

	case CSI_LINE_WANTY1:

		if (pcommand->CheckParamSet(
			CSP_LINE_B_XY, 
			COMMPARAMFLAG_Y))
		{
			pcommand->StepTo(
				CSI_LINE_WANTX2, 
				CSP_LINE_E_XY, 
				COMMPARAMFLAG_X, 
				CWP_XY);
		}
		else if (pmain->DoPickPoint())
		{
			if (!pcommand->IsInternalProcessing())
			{
				float x = pmain->pickx;
				float y = pmain->picky;
				pguic->ClientToCoordinate(&x, &y);
				pcommand->SetParamX(CSP_LINE_B_XY, x);
				pcommand->SetParamY(CSP_LINE_B_XY, y);
				pcommand->StepTo(
					CSI_LINE_WANTX2, 
					CSP_LINE_E_XY, 
					COMMPARAMFLAG_X, 
					CWP_XY);
			}
		}
		break;

	case CSI_LINE_WANTX2:

		if (pcommand->CheckParamSet(
			CSP_LINE_E_XY, 
			COMMPARAMFLAG_X))
		{
			pcommand->StepTo(
				CSI_LINE_WANTY2, 
				CSP_LINE_E_XY, 
				COMMPARAMFLAG_Y, 
				CWP_XY);
		}
		else if (pmain->DoPickPoint())
		{
			if (!pcommand->IsInternalProcessing())
			{
				float x = pmain->pickx;
				float y = pmain->picky;
				pguic->ClientToCoordinate(&x, &y);
				pcommand->SetParamX(CSP_LINE_E_XY, x);
				pcommand->SetParamY(CSP_LINE_E_XY, y);
				pcommand->StepTo(CSI_FINISH);
			}
		}
		break;

	case CSI_LINE_WANTY2:

		if (pcommand->CheckParamSet(
			CSP_LINE_E_XY, 
			COMMPARAMFLAG_Y))
		{
			pcommand->StepTo(CSI_FINISH);
		}
		if (pmain->DoPickPoint())
		{
			if (!pcommand->IsInternalProcessing())
			{
				float x = pmain->pickx;
				float y = pmain->picky;
				pguic->ClientToCoordinate(&x, &y);
				pcommand->SetParamX(CSP_LINE_E_XY, x);
				pcommand->SetParamY(CSP_LINE_E_XY, y);
				pcommand->StepTo(CSI_FINISH);
			}
		}
		break;
	case CSI_PAUSE:
		GUICursor::getInstance().ChangeCursor();
		ReleaseTarget();
		break;
	case CSI_RESUME:
		GUICursor::getInstance().ChangeCursor(GUIC_CREATEPOINT);
		break;

	case CSI_FINISH:
		GUICursor::getInstance().ChangeCursor();
		ReleaseTarget();
		DoneLineCommand();
		pcommand->FinishCommand();
		break;

	case CSI_TERMINAL:
		GUICursor::getInstance().ChangeCursor();
		ReleaseTarget();
		pcommand->TerminalCommand();
		break;
	}
	LineCommandRenderToTarget();
	return 0;
}

void LineCommand::LineCommandRenderToTarget()
{
	Command * pcommand = &Command::getInstance();
	int nstep = pcommand->GetStep();
	if (nstep >= CSI_LINE_WANTX2 && nstep <= CSI_LINE_WANTY2)
	{
		float x1, y1;
		pcommand->GetParamXY(CSP_LINE_B_XY, &x1, &y1);
		GUICoordinate::getInstance().CoordinateToClient(&x1, &y1);

		float x2 = MainInterface::getInstance().mousex;
		float y2 = MainInterface::getInstance().mousey;

		HTARGET tar = RenderTargetManager::getInstance().UpdateTarget(RTID_COMMAND);

		RenderHelper::BeginRenderTar(tar);
		RenderHelper::RenderLine(x1, y1, x2, y2, ColorManager::GetLayerLineColor());
		RenderHelper::EndRenderTar();

		Command::getInstance().SetRenderTarget(tar);
	}
}

void LineCommand::ReleaseTarget()
{
	Command::getInstance().SetRenderTarget(0);
}

void LineCommand::DoneLineCommand()
{
	GStraightLine * line = new GStraightLine(&(GObjectManager::getInstance().basenode));
	Command * pcommand = &Command::getInstance();
	float xb, yb, xe, ye;
	pcommand->GetParamXY(CSP_LINE_B_XY, &xb, &yb);
	pcommand->GetParamXY(CSP_LINE_E_XY, &xe, &ye);
	line->SetBeginEnd(xb, yb, xe, ye);
}