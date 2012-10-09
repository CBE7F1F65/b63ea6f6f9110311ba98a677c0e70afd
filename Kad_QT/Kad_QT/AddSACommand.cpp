#include "StdAfx.h"
#include "AddSACommand.h"


AddSACommand::AddSACommand(void)
{
}


AddSACommand::~AddSACommand(void)
{
}

void AddSACommand::OnProcessCommand()
{
	int step = OnNormalProcessCommand();

	int nowstep = pcommand->GetStep();
	UpdateLastStep();

	int ret = -1;

	if (step == CSI_INIT)
	{
		pcommand->StepTo(
			CSI_ADDSA_WANTX, CWP_X);
	}
	else if (step == CSI_ADDSA_WANTX)
	{
		ret = pcommand->ProcessPending(
			CSP_ADDSA_F_XY_SA_ORIGIN, COMMPARAMFLAG_X, CWP_X,
			CSI_ADDSA_WANTY, CWP_Y
			);
	}
	else if (step == CSI_ADDSA_WANTY)
	{
		ret = pcommand->ProcessPending(
			CSP_ADDSA_F_XY_SA_ORIGIN, COMMPARAMFLAG_Y, CWP_Y,
			CSI_ADDSA_WANTSA, CWP_SA
			);
	}
	else if (step == CSI_ADDSA_WANTSA)
	{
		ret = pcommand->ProcessPending(
			CSP_ADDSA_F_XY_SA_ORIGIN, COMMPARAMFLAG_F, CWP_SA,
			CSI_ADDSA_WANTINDEXES, CWP_INDEX
			);
	}
	else if (step >= CSI_ADDSA_WANTINDEXES)
	{
		int nowindex = step - CSI_ADDSA_WANTINDEXES;
		if (bManualMode)
		{
			if (nowindex)
			{
				pcommand->SetParamI(CSP_ADDSA_I_INDEXES+nowindex, -1, CWP_INDEX);
				pcommand->StepTo(CSI_FINISH);
			}
			else
			{
				int i=nowindex;
				for (list<GObject *>::reverse_iterator it=lstObj.rbegin(); it!=lstObj.rend(); ++it, i++)
				{
					pcommand->SetParamI(CSP_ADDSA_I_INDEXES+i, (*it)->getID(), CWP_INDEX);
				}
				pcommand->SetParamI(CSP_ADDSA_I_INDEXES+i, -1, CWP_INDEX);
				pcommand->StepTo(CSI_FINISH);
			}
		}
		else
		{
			ret = pcommand->ProcessPending(
				CSP_ADDSA_I_INDEXES+nowindex, COMMPARAMFLAG_I, CWP_INDEX,
				CSI_ADDSA_WANTINDEXES+nowindex+1, CWP_INDEX
				);
			if (pcommand->GetParamI(CSP_ADDSA_I_INDEXES+nowindex) < 0)
			{
				pcommand->StepTo(CSI_FINISH);
			}

		}
	}

	if (ret < 0)
	{
		if (step > CSI_INIT)
		{
			if (!bManualMode)
			{
				lstObj = *(pgm->GetSelectedNodes(false));
				if (lstObj.empty())
				{
					Terminal();
					return;
				}
				lstObj.sort();
			}
			bManualMode = true;
		}

		if (step >= CSI_ADDSA_WANTX && step < CSI_ADDSA_WANTINDEXES)
		{
			int iret = pgp->PickPoint(NULL, staticFilterCallback);
			if (pgp->IsPickReady(iret))
			{
				if (!pcommand->IsInternalProcessing())
				{
					pcommand->SetParamX(CSP_ADDSA_F_XY_SA_ORIGIN, pgp->GetPickX_C());
					pcommand->SetParamY(CSP_ADDSA_F_XY_SA_ORIGIN, pgp->GetPickY_C());
					pcommand->StepTo(CSI_ADDSA_WANTSA, CWP_SA);
				}
			}
		}
	}
}

void AddSACommand::OnDoneCommand()
{
	float orix, oriy;
	pcommand->GetParamXY(CSP_ADDSA_F_XY_SA_ORIGIN, &orix, &oriy);
	float fsa = pcommand->GetParamF(CSP_ADDSA_F_XY_SA_ORIGIN);

	int index = pcommand->GetParamI(CSP_ADDSA_I_INDEXES);
	list<GObject *> lobjs;
	int i=0;
	while (index >= 0)
	{
		GObject * pObj = pgm->FindObjectByID(index);
		if (pObj)
		{
			lobjs.push_back(pObj);
		}
		i++;
		index = pcommand->GetParamI(CSP_ADDSA_I_INDEXES+i);
	}

	if (lobjs.empty())
	{
		Terminal();
		return;
	}

	pgm->SetLockTreeChange();
	for (list<GObject *>::iterator it=lobjs.begin(); it!=lobjs.end();)
	{
		GObject * pAddSAObj = *it;
		if (!pAddSAObj->isLine())
		{
			it = lobjs.erase(it);
		}
		else
		{
			pgm->OnTreeChanged(pAddSAObj->getParent(), pAddSAObj);
			++it;
		}
	}


	if (lobjs.empty())
	{
		Terminal();
	}
	else
	{
		for (list<GObject *>::iterator it=lobjs.begin(); it!=lobjs.end(); ++it)
		{
			GLine * pLine = (GLine *)*it;
			pLine->AddSA(orix, oriy, fsa);
		}
		int objcount = lobjs.size();
		PushRevertableBatch(
			PUSHREVERTABLESTATE_BEGIN,
			CCMake_C(COMM_I_COMMAND, 4+objcount, 1),
			CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
			CCMake_C(COMM_ADDSA),
			CCMake_F(orix),
			CCMake_F(oriy),
			NULL
			);
		for (list<GObject *>::iterator it=lobjs.begin(); it!=lobjs.end(); ++it)
		{
			PushRevertableBatch(
				PUSHREVERTABLESTATE_CONTINUE,
				CCMake_I((*it)->getID()),
				NULL
				);
		}
		PushRevertableBatch(
			PUSHREVERTABLESTATE_END,
			CCMake_I(-1),
			CCMake_C(COMM_I_UNDO_PARAMFROMCOMMAND),
			NULL
			);

	}
}

void AddSACommand::OnClearTemp()
{
	bManualMode = false;
	lstObj.clear();
}

bool AddSACommand::staticFilterCallback( GObject * pObj )
{
	return true;
}
