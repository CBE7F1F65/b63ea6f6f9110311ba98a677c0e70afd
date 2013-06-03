#include "StdAfx.h"
#include "AttachCommand.h"


AttachCommand::AttachCommand(void)
{
}


AttachCommand::~AttachCommand(void)
{
}

void AttachCommand::OnProcessCommand()
{
	int step = OnNormalProcessCommand();
	int nowstep = pcommand->GetStep();
	UpdateLastStep();

	int ret = -1;

	if (step == CSI_INIT)
	{
		pcommand->StepTo(
			CSI_ATTACH_WANTINDEX,
			CWP_INDEX);
	}
	else if (step == CSI_ATTACH_WANTINDEX)
	{
		ret = pcommand->ProcessPending(
			CSP_ATTACH_I_INDEX, COMMPARAMFLAG_I, CWP_INDEX,
			CSI_FINISH
			);
	}

	if (ret < 0)
	{
		// Routine
		if (step > CSI_INIT)
		{
			list<GObject *> * lobjs = pgm->GetSelectedNodes(false);
			if (lobjs->size() == 1)
			{
				int index = lobjs->front()->getID();
				pcommand->SetParamI(CSP_ATTACH_I_INDEX, index, CWP_INDEX);
				pcommand->StepTo(CSI_FINISH);
			}
			else
			{
				int iret = pgp->PickPoint();
				if (pgp->IsPickReady(iret))
				{
					if (!pcommand->IsInternalProcessing())
					{
						GObject * pObj = pgp->GetPickedObj();
						if (pObj)
						{
							int index = pObj->getID();

							pcommand->SetParamI(CSP_ATTACH_I_INDEX, index, CWP_INDEX);
							pcommand->StepTo(CSI_FINISH);
						}
					}
				}
			}
		}
	}
}

void AttachCommand::OnDoneCommand()
{
	int index = pcommand->GetParamI(CSP_ATTACH_I_INDEX);
	GObject * pObj = pgm->FindObjectByID(index);
	if (!pObj)
	{
		Terminal();
		return;
	}

	ReAttachAfterMoveNode(pObj);
	if (true)
	{
		PushRevertible(
			CCMake_C(COMM_I_COMMAND, 2),
			CCMake_CI(COMM_I_COMM_WORKINGLAYER, workinglayerID),
			CCMake_C(COMM_ATTACH),
			CCMake_I(index),
			NULL
			);		
	}
}
