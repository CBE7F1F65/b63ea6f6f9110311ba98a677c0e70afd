#include "StdAfx.h"
#include "OpenCommand.h"

#include "qmaininterface.h"

OpenCommand::OpenCommand(void)
{
}


OpenCommand::~OpenCommand(void)
{
}

void OpenCommand::OnProcessCommand()
{
	InstantProcessCommand();
}

void OpenCommand::OnDoneCommand()
{
	if (pmain->OpenFile())
	{
		PushRevertible(
			CCMake_C(COMM_I_COMMAND, 2),
			CCMake_CI(COMM_I_COMM_WORKINGLAYER, workinglayerID),
			CCMake_C(COMM_OPEN),
			CCMake_S(pmain->GetSaveFileName()),
			NULL
			);

		QMainInterface::getInstance().GetPCommandDockable()->setFocus();
		QMainInterface::getInstance().GetPGLView()->setFocus();
	}
}