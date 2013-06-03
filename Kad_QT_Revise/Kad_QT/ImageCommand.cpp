#include "StdAfx.h"
#include "ImageCommand.h"


ImageCommand::ImageCommand(void)
{
}


ImageCommand::~ImageCommand(void)
{
}

void ImageCommand::OnProcessCommand()
{
	int step = OnNormalProcessCommand();

	int nowstep = pcommand->GetStep();
	UpdateLastStep();

	int ret = -1;

	if (step == CSI_INIT)
	{
		pcommand->StepTo(
			CSI_IMAGE_WANTFILENAME,
			CWP_NAME);
	}
	else if (step == CSI_IMAGE_WANTFILENAME)
	{
		ret = pcommand->ProcessPending(
			CSP_IMAGE_S_FILENAME, COMMPARAMFLAG_S, CWP_NAME,	// Fill in Param
			CSI_FINISH					// Step to with want
			);
	}
}

void ImageCommand::OnDoneCommand()
{
	string filename = pcommand->GetParamS(CSP_IMAGE_S_FILENAME);
	if (!filename.length())
	{
		Terminal();
		return;
	}

	GImageLine * pImageLine = new GImageLine(pgm->GetActiveLayer(), filename.c_str(), 0, 0);
	if (pImageLine)
	{
		PushRevertible(
			CCMake_C(COMM_I_COMMAND, 2),
			CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
			CCMake_C(COMM_IMAGE),
			CCMake_S(filename.c_str()),
			NULL
			);
	}
}
