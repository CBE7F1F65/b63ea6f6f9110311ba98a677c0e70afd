#include "StdAfx.h"
#include "RestoreToSnapshotCommand.h"
#include "SnapshotManager.h"


RestoreToSnapshotCommand::RestoreToSnapshotCommand(void)
{
}


RestoreToSnapshotCommand::~RestoreToSnapshotCommand(void)
{
}

void RestoreToSnapshotCommand::OnProcessCommand()
{
	int step = OnNormalProcessCommand();
	UpdateLastStep();

	int ret = -1;

	if (step == CSI_INIT)
	{
		pcommand->StepTo(
			CSI_RESTORETOSNAPSHOT_WANTINDEX,
			CWP_INDEX);
	}
	else if (step == CSI_RESTORETOSNAPSHOT_WANTINDEX)
	{
		ret = pcommand->ProcessPending(
			CSP_RESTORETOSNAPSHOT_I_INDEX, COMMPARAMFLAG_I, CWP_INDEX,
			CSI_FINISH
			);
	}
}

void RestoreToSnapshotCommand::OnDoneCommand()
{
	int index = pcommand->GetParamI(CSP_RESTORETOSNAPSHOT_I_INDEX);
	index--;
	if (index < 0)
	{
		pcommand->TerminalCommand();
		return;
	}
	if (SnapshotManager::getInstance().RevertToSnapshot(index))
	{
		PushRevertible(
			CCMake_C(COMM_I_COMMAND, 2, 0),
			CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
			CCMake_C(COMM_RESTORETOSNAPSHOT),
			CCMake_I(index),
			NULL);
	}
}