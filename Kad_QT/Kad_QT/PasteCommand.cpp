#include "StdAfx.h"
#include "PasteCommand.h"


PasteCommand::PasteCommand(void)
{
}


PasteCommand::~PasteCommand(void)
{
}

void PasteCommand::OnProcessCommand()
{
	InstantProcessCommand();
}

void PasteCommand::OnDoneCommand()
{
	if (pgm->PasteNodes())
	{
		PushRevertible(
			CCMake_C(COMM_I_COMMAND, 1),
			CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
			CCMake_C(COMM_PASTE),
			NULL
			);
	}
}
