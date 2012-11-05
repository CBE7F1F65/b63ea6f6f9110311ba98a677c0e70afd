#include "StdAfx.h"
#include "DumpCommand.h"


DumpCommand::DumpCommand(void)
{
}


DumpCommand::~DumpCommand(void)
{
}

void DumpCommand::OnProcessCommand()
{
	int step = OnNormalProcessCommand();

	int nowstep = pcommand->GetStep();
	UpdateLastStep();

	int ret = -1;

	if (step == CSI_INIT)
	{
		pcommand->StepTo(
			CSI_DUMP_WANTINDEXES, CWP_INDEX);
	}
	else if (step >= CSI_DUMP_WANTINDEXES)
	{
		int nowindex = step - CSI_DUMP_WANTINDEXES;
		if (bManualMode)
		{
			if (nowindex)
			{
				pcommand->SetParamI(CSP_DUMP_I_INDEXES+nowindex, -1, CWP_INDEX);
				pcommand->StepTo(CSI_FINISH);
			}
			else
			{
				int i=nowindex;
				for (list<GObject *>::reverse_iterator it=lstObj.rbegin(); it!=lstObj.rend(); ++it, i++)
				{
					pcommand->SetParamI(CSP_DUMP_I_INDEXES+i, (*it)->getID(), CWP_INDEX);
				}
				pcommand->SetParamI(CSP_DUMP_I_INDEXES+i, -1, CWP_INDEX);
				pcommand->StepTo(CSI_FINISH);
			}
		}
		else
		{
			ret = pcommand->ProcessPending(
				CSP_DUMP_I_INDEXES+nowindex, COMMPARAMFLAG_I, CWP_INDEX,
				CSI_DUMP_WANTINDEXES+nowindex+1, CWP_INDEX
				);
			if (pcommand->GetParamI(CSP_DUMP_I_INDEXES+nowindex) < 0)
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
	}
}

void DumpCommand::OnDoneCommand()
{
	int index = pcommand->GetParamI(CSP_DUMP_I_INDEXES);
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
		index = pcommand->GetParamI(CSP_DUMP_I_INDEXES+i);
	}

	if (lobjs.empty())
	{
		Terminal();
		return;
	}

	if (pgm->Dump(lobjs, comm==COMM_DUMP?true:false))
	{
		int objcount = lobjs.size();
		PushRevertibleBatch(
			PUSHREVERTABLESTATE_BEGIN,
			CCMake_C(COMM_I_COMMAND, 2+objcount, 1),
			CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
			CCMake_C(comm),
			NULL
			);
		for (list<GObject *>::iterator it=lobjs.begin(); it!=lobjs.end(); ++it)
		{
			PushRevertibleBatch(
				PUSHREVERTABLESTATE_CONTINUE,
				CCMake_I((*it)->getID()),
				NULL
				);
		}
		PushRevertibleBatch(
			PUSHREVERTABLESTATE_END,
			CCMake_I(-1),
			CCMake_C(COMM_I_UNDO_PARAMFROMCOMMAND),
			NULL
			);

	}

}

void DumpCommand::OnClearTemp()
{
	bManualMode = false;
	lstObj.clear();
}
