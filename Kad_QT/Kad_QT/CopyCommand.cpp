#include "StdAfx.h"
#include "CopyCommand.h"
#include "MarqueeSelect.h"


CopyCommand::CopyCommand(void)
{
}


CopyCommand::~CopyCommand(void)
{
}

void CopyCommand::OnProcessCommand()
{
	int step = OnNormalProcessCommand();
	UpdateLastStep();

	int ret = -1;

	if (step == CSI_INIT)
	{
		pcommand->StepTo(
			CSI_COPY_WANTINDEXES,
			CWP_INDEX);
	}
	else if (step >= CSI_COPY_WANTINDEXES)
	{
		int nowindex = step-CSI_COPY_WANTINDEXES;
		ret = pcommand->ProcessPending(
			CSP_COPY_I_INDEXES+nowindex, COMMPARAMFLAG_I, CWP_INDEX,
			CSI_COPY_WANTINDEXES+nowindex+1, CWP_INDEX
			);
		if (!ret)
		{
			int index = pcommand->GetParamI(CSP_COPY_I_INDEXES+nowindex);
			if (index < 0)
			{
				pcommand->StepTo(CSI_FINISH);
			}
		}
		else if (!nowindex)
		{
			if (ret < 0)
			{
				list<GObject*> * selectnodelist = pgm->GetSelectedNodes(false);

				list <int> lids;
				for (list<GObject*>::reverse_iterator it=selectnodelist->rbegin(); it!= selectnodelist->rend(); ++it)
				{
					GObject * pObj = *it;
					if (pObj->isRepresentablePoint() || pObj->isRepresentableLine())
					{
						lids.push_back(pObj->getID());
					}
				}
				if (lids.empty())
				{
					Terminal();
				}
				else
				{
					lids.sort();
					int i=0;
					for (list<int>::reverse_iterator it=lids.rbegin(); it!=lids.rend(); ++it)
					{
						pcommand->SetParamI(CSP_COPY_I_INDEXES+i, (*it), CWP_INDEX);
						i++;
					}
					pcommand->SetParamI(CSP_COPY_I_INDEXES+i, -1, CWP_INDEX);

					pcommand->StepTo(CSI_FINISH);
				}

			}
		}
		else
		{
			pcommand->SetParamI(CSP_COPY_I_INDEXES+nowindex+1, -1, CWP_INDEX);
			pcommand->StepTo(CSI_FINISH);
		}
	}

}

void CopyCommand::OnDoneCommand()
{
	int i=0;

	list<GObject *> lstObj;
	while (true)
	{
		int index = pcommand->GetParamI(CSP_COPY_I_INDEXES+i);
		if (index < 0)
		{
			break;
		}
		GObject * pObj = pgm->FindObjectByID(index);
		if (pObj->isRepresentablePoint() || pObj->isRepresentableLine())
		{
			lstObj.push_back(pObj);
		}
		i++;
	}

	if (!lstObj.empty())
	{
		lstObj.sort();

		pgm->ClearCopiedNodes();

		int objcount = lstObj.size();
		PushRevertibleBatch(
			PUSHREVERTABLESTATE_BEGIN,
			CCMake_C(COMM_I_COMMAND, 2+objcount, 1),
			CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
			CCMake_C(comm),
			NULL
			);
		for (list<GObject *>::iterator it=lstObj.begin(); it!=lstObj.end(); ++it)
		{
			pgm->AddCopyNode(*it, -1, comm==COMM_COPYLAYER);
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

		MarqueeSelect::getInstance().DeSelectAll();
	}

}
