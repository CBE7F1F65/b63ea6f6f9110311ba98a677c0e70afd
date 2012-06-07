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
			CSI_DELETEITEM_WANTINDEX,
			CWP_INDEX);
	}
	else if (step == CSI_NEWLAYER_WANTNAME)
	{
		ret = pcommand->ProcessPending(
			CSP_DELETEITEM_I_INDEX, COMMPARAMFLAG_I, CWP_INDEX,
			CSI_FINISH
			);
		if (ret<0)
		{
			list<GObject*> * activenodelist = pgm->GetSelectedNodes();
			
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
					pcommand->SetParamI(CSP_DELETEITEM_I_INDEX+i, (*it), CWP_INDEX);
					i++;
				}
				pcommand->SetParamI(CSP_DELETEITEM_I_INDEX+i, -1, CWP_INDEX);

				pcommand->StepTo(CSI_FINISH);
			}

		}
		else
		{
			pcommand->SetParamI(CSP_DELETEITEM_I_INDEX+1, -1, CWP_INDEX);
		}
	}

}

void DeleteItemCommand::OnDoneCommand()
{
	int i=0;
	while (true)
	{
		int index = pcommand->GetParamI(CSP_DELETEITEM_I_INDEX+i);
		if (index < 0)
		{
			break;
		}
		GObject * pObj = pgm->FindObjectByID(index);
		if (!pObj)
		{
			return;
		}
		GObject * pOrderSibling = pObj->getYoungerSibling();

		GObject * pParent = pObj->getParent();
		ASSERT(pParent);
		int parentid = pParent->getID();
		int afterid = -1;
		if (pOrderSibling)
		{
			afterid = pOrderSibling->getID();
		}
		pgm->MoveToUnDoList(pObj);
		PushRevertable(
			CCMake_C(COMM_I_DELETENODE, 2),
			CCMake_I(parentid),
			CCMake_I(afterid),
			CCMake_C(COMM_I_COMMAND, 2, 0),
			CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
			CCMake_C(COMM_DELETEITEM),
			CCMake_I(index),
			NULL
			);
		i++;
	}
}

void DeleteItemCommand::OnProcessUnDoCommand( RevertableCommand * rc )
{

}