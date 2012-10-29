#include "StdAfx.h"
#include "IsolateCommand.h"


IsolateCommand::IsolateCommand(void)
{
}


IsolateCommand::~IsolateCommand(void)
{
}

void IsolateCommand::OnProcessCommand()
{
	int step = OnNormalProcessCommand();
	int nowstep = pcommand->GetStep();
	UpdateLastStep();

	int ret = -1;

	if (step == CSI_INIT)
	{
		pcommand->StepTo(
			CSI_ISOLATE_WANTINDEX,
			CWP_INDEX);
	}
	else if (step == CSI_ISOLATE_WANTINDEX)
	{
		ret = pcommand->ProcessPending(
			CSP_ISOLATE_I_INDEX, COMMPARAMFLAG_I, CWP_INDEX,
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
				pcommand->SetParamI(CSP_ISOLATE_I_INDEX, index, CWP_INDEX);
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

							pcommand->SetParamI(CSP_ISOLATE_I_INDEX, index, CWP_INDEX);
							pcommand->StepTo(CSI_FINISH);
						}
					}
				}
			}
		}
	}
}

void IsolateCommand::OnDoneCommand()
{
	int index = pcommand->GetParamI(CSP_ISOLATE_I_INDEX);
	GObject * pObj = pgm->FindObjectByID(index);
	if (!pObj)
	{
		Terminal();
		return;
	}
	
	if (pObj->Isolate())
	{
		PushRevertible(
			CCMake_C(COMM_I_COMMAND, 2),
			CCMake_CI(COMM_I_COMM_WORKINGLAYER, workinglayerID),
			CCMake_C(COMM_ISOLATE),
			CCMake_I(index),
			NULL
			);		
	}
}