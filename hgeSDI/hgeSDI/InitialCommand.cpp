#include "StdAfx.h"
#include "InitialCommand.h"

InitialCommand::InitialCommand(void)
{
}

InitialCommand::~InitialCommand(void)
{
}

void InitialCommand::OnProcessCommand()
{
	InstantProcessCommand();
}

void InitialCommand::OnDoneCommand()
{
	PushRevertable(
		CCMake_C(COMM_I_COMMAND, 1, 0),
		CCMake_C(COMM_I_COMM_WORKINGLAYER, workingLayer?workingLayer->getID():0),
		CCMake_C(COMM_INITIAL),
		NULL
		);
}
