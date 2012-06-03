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
	int step = OnNormalProcessCommand();
	int nowstep = pcommand->GetStep();
	UpdateLastStep();

	if (step == CSI_INIT)
	{
		pcommand->StepTo(
			CSI_FINISH
			);
	}
}

void InitialCommand::DoneCommand()
{
	PushRevertable(
		CCMake_C(COMM_I_COMMAND, 1, 0),
		CCMake_C(COMM_INITIAL),
		NULL
		);
}

void InitialCommand::RenderToTarget()
{
}