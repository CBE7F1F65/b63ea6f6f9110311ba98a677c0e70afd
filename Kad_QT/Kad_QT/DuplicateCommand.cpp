#include "StdAfx.h"
#include "DuplicateCommand.h"


DuplicateCommand::DuplicateCommand(void)
{
}


DuplicateCommand::~DuplicateCommand(void)
{
}

void DuplicateCommand::OnProcessCommand()
{
	int step = OnNormalProcessCommand();
	UpdateLastStep();

	int ret = -1;

	if (step == CSI_INIT)
	{
		pcommand->StepTo(
			CSI_DUPLICATE_WANTINDEXES,
			CWP_INDEX);
	}
	else if (step == CSI_DUPLICATE_WANTINDEXES)
	{
		ret = pcommand->ProcessPending(
			CSP_DUPLICATE_I_INDEXES, COMMPARAMFLAG_I, CWP_INDEX,
			CSI_FINISH
			);
		if (ret<0)
		{
            list<GObject*> * activenodelist = pgm->GetSelectedNodes(true);
			
			list <int> lids;
            for (list<GObject*>::reverse_iterator it=activenodelist->rbegin(); it!= activenodelist->rend(); ++it)
			{
				if (pgm->CanDuplicateItem((*it)))
				{
					lids.push_back((*it)->getID());
				}
			}
			if (lids.empty())
			{
				pcommand->StepTo(CSI_TERMINAL);
			}
			else
			{
				lids.sort();
				int i=0;
				for (list<int>::reverse_iterator it=lids.rbegin(); it!=lids.rend(); ++it)
				{
					pcommand->SetParamI(CSP_DUPLICATE_I_INDEXES+i, (*it), CWP_INDEX);
					i++;
				}
				pcommand->SetParamI(CSP_DUPLICATE_I_INDEXES+i, -1, CWP_INDEX);

				pcommand->StepTo(CSI_FINISH);
			}

		}
		else
		{
			pcommand->SetParamI(CSP_DUPLICATE_I_INDEXES+1, -1, CWP_INDEX);
		}
	}
}

void DuplicateCommand::OnDoneCommand()
{
	int i=0;

	list<GObject *>listobjs;
	while (true)
	{
		int index = pcommand->GetParamI(CSP_DUPLICATE_I_INDEXES+i);
		if (index < 0)
		{
			break;
		}
		GObject *pObj = pgm->FindObjectByID(index);
		if (pObj)
		{
			listobjs.push_back(pObj);
		}
		i++;
	}

	for (list<GObject *>::iterator it=listobjs.begin(); it!=listobjs.end(); ++it)
	{
		GObject * pObj = *it;
		int index = pObj->getID();
		GObject * pParent = pObj->getParent();
		ASSERT(pParent);

		if (!pgm->CanDuplicateItem(pObj))
		{
			continue;
		}

//		GBaseNode tempholdplace;
//		GObject * pDupObj = pObj->CreateNewClone(&tempholdplace);
//		pDupObj->ReparentBeforeObject(pParent, pObj);
//		tempholdplace.RemoveAllChildren(true);
		GObject * pDupObj = pObj->CreateNewClone(pParent, pObj);


		PushRevertable(
			CCMake_C(COMM_I_ADDNODE, 2),
			CCMake_I(pDupObj->getID()),
			CCMake_I(pParent->getID()),
			CCMake_C(COMM_I_COMMAND, 2, 0),
			CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
			CCMake_C(COMM_DUPLICATE),
			CCMake_I(index),
			NULL
			);
		i++;
	}
}
