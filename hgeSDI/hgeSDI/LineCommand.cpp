#include "stdafx.h"
#include "Command.h"
#include "LineCommand.h"
#include "Main.h"

#include "GUICursor.h"
#include "GUICoordinate.h"
#include "RenderHelper.h"
#include "RenderTargetManager.h"
#include "ColorManager.h"

LineCommand * plc = NULL;

LineCommand::LineCommand()
{
	assert(plc==NULL);
}
LineCommand::~LineCommand()
{

}

LineCommand * LineCommand::getInstance()
{
	if (!plc)
	{
		plc = new LineCommand();
	}
	return plc;
}

int LineCommand::Line()
{
	Command * pcommand = Command::getInstance();
	MainInterface * pmain = MainInterface::getInstance();
	GUICoordinate * pguic = GUICoordinate::getInstance();
	switch (pcommand->GetStep())
	{
	case CSI_INIT:

		GUICursor::getInstance()->ChangeCursor(GUIC_CREATEPOINT);
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
		GUICursor::getInstance()->ChangeCursor();
		ReleaseTarget();
		break;
	case CSI_RESUME:
		GUICursor::getInstance()->ChangeCursor(GUIC_CREATEPOINT);
		break;

	case CSI_FINISH:
		GUICursor::getInstance()->ChangeCursor();
		ReleaseTarget();
		pcommand->FinishCommand();
		break;

	case CSI_TERMINAL:
		GUICursor::getInstance()->ChangeCursor();
		ReleaseTarget();
		pcommand->TerminalCommand();
		break;
	}
	LineRenderToTarget();
	return 0;
}

void LineCommand::LineRenderToTarget()
{
	Command * pcommand = Command::getInstance();
	int nstep = pcommand->GetStep();
	if (nstep >= CSI_LINE_WANTX2 && nstep <= CSI_LINE_WANTY2)
	{
		float x1, y1;
		pcommand->GetParamXY(CSP_LINE_B_XY, &x1, &y1);
		GUICoordinate::getInstance()->CoordinateToClient(&x1, &y1);

		float x2 = MainInterface::getInstance()->mousex;
		float y2 = MainInterface::getInstance()->mousey;

		HTARGET tar = RenderTargetManager::getInstance()->UpdateTarget(RTID_COMMAND);

		RenderHelper::BeginRenderTar(tar);
		RenderHelper::RenderLine(x1, y1, x2, y2, ColorManager::GetLayerLineColor());
		RenderHelper::EndRenderTar();

		Command::getInstance()->SetRenderTarget(tar);
	}
}

void LineCommand::ReleaseTarget()
{
	Command::getInstance()->SetRenderTarget(0);
}