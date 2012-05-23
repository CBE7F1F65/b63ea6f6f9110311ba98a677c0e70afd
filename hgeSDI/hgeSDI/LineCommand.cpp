#include "stdafx.h"
#include "Command.h"
#include "LineCommand.h"
#include "Main.h"

#include "GUICursor.h"

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
				pcommand->SetParamX(CSP_LINE_B_XY, pmain->pickx);
				pcommand->SetParamY(CSP_LINE_B_XY, pmain->picky);
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
				pcommand->SetParamX(CSP_LINE_B_XY, pmain->pickx);
				pcommand->SetParamY(CSP_LINE_B_XY, pmain->picky);
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
				pcommand->SetParamX(CSP_LINE_E_XY, pmain->pickx);
				pcommand->SetParamY(CSP_LINE_E_XY, pmain->picky);
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
				pcommand->SetParamX(CSP_LINE_E_XY, pmain->pickx);
				pcommand->SetParamY(CSP_LINE_E_XY, pmain->picky);
				pcommand->StepTo(CSI_FINISH);
			}
		}
		break;

	case CSI_FINISH:
		GUICursor::getInstance()->ChangeCursor();
		pcommand->FinishCommand();
		break;

	case CSI_TERMINAL:
		GUICursor::getInstance()->ChangeCursor();
		pcommand->FinishCommand();
		break;
	}
	return 0;
}