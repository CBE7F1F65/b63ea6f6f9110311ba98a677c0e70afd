#include "StdAfx.h"
#include "DeleteItemCommand.h"


DeleteItemCommand::DeleteItemCommand(void)
{
}


DeleteItemCommand::~DeleteItemCommand(void)
{
}

void DeleteItemCommand::OnProcessCommand()
{

	int step = OnNormalProcessCommand();
	UpdateLastStep();

	int ret = -1;

	if (step == CSI_INIT)
	{
		pcommand->StepTo(
			CSI_DELETEITEM_WANTINDEXES,
			CWP_INDEX);
	}
	else if (step >= CSI_DELETEITEM_WANTINDEXES)
	{
		int nowindex = step-CSI_DELETEITEM_WANTINDEXES;
		ret = pcommand->ProcessPending(
			CSP_DELETEITEM_I_INDEXES+nowindex, COMMPARAMFLAG_I, CWP_INDEX,
			CSI_DELETEITEM_WANTINDEXES+nowindex+1, CWP_INDEX
			);
		if (!ret)
		{
			int index = pcommand->GetParamI(CSP_DELETEITEM_I_INDEXES+nowindex);
			if (index < 0)
			{
				pcommand->StepTo(CSI_FINISH);
			}
		}
		else if (!nowindex)
		{
			if (ret < 0)
			{
				list<GObject*> * activenodelist = pgm->GetSelectedNodes(true);

				list <int> lids;
				for (list<GObject*>::reverse_iterator it=activenodelist->rbegin(); it!= activenodelist->rend(); ++it)
				{
					if (pgm->CanDeleteItem((*it)))
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
						pcommand->SetParamI(CSP_DELETEITEM_I_INDEXES+i, (*it), CWP_INDEX);
						i++;
					}
					pcommand->SetParamI(CSP_DELETEITEM_I_INDEXES+i, -1, CWP_INDEX);

					pcommand->StepTo(CSI_FINISH);
				}

			}
		}
 		else
 		{
 			pcommand->SetParamI(CSP_DELETEITEM_I_INDEXES+nowindex+1, -1, CWP_INDEX);
			pcommand->StepTo(CSI_FINISH);
 		}
	}

}

void DeleteItemCommand::OnDoneCommand()
{
	int i=0;

	list<GObject *> lstObj;
	while (true)
	{
		int index = pcommand->GetParamI(CSP_DELETEITEM_I_INDEXES+i);
		if (index < 0)
		{
			break;
		}
		GObject * pObj = pgm->FindObjectByID(index);
		if (pgm->CanDeleteItem(pObj))
		{
			lstObj.push_back(pObj);
		}
		i++;
	}

	if (!lstObj.empty())
	{
		lstObj.sort();
		for (list<GObject *>::reverse_iterator it=lstObj.rbegin(); it!=lstObj.rend(); ++it)
		{
			GObject * pObj = *it;
			GObject * pOrderSibling = pObj->getYoungerSibling();

			GObject * pParent = pObj->getParent();
			ASSERT(pParent);
			int parentid = pParent->getID();
			int afterid = -1;
			if (pOrderSibling)
			{
				afterid = pOrderSibling->getID();
			}

			pObj->RemoveFromParent(true);
//			pgm->MoveToUnDoList(pObj);

			PushRevertable(
				CCMake_C(COMM_I_COMMAND, 2, 0),
				CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
				CCMake_C(COMM_DELETEITEM),
				CCMake_I(pObj->getID()),
				NULL
				);
		}
	}
}