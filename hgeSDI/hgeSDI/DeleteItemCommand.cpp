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
			list<GObject*> * activenodelist = GObjectManager::getInstance().GetSelectedNodes();
			// ToDo!!! Change this sort
			list <GObject> lobjects;
			for (list<GObject*>::reverse_iterator it=activenodelist->rbegin(); it!= activenodelist->rend(); ++it)
			{
				lobjects.push_back(*(*it));
			}
			lobjects.sort();
//			activenodelist->sort();
			int i=0;
//			for (list<GObject*>::reverse_iterator it=activenodelist->rbegin(); it!= activenodelist->rend(); ++it)
			for (list<GObject>::reverse_iterator it=lobjects.rbegin(); it!=lobjects.rend(); ++it)
			{
				pcommand->SetParamI(CSP_DELETEITEM_I_INDEX+i, (*it).getID(), CWP_INDEX);
				i++;
			}
			pcommand->SetParamI(CSP_DELETEITEM_I_INDEX+i, -1, CWP_INDEX);

			pcommand->StepTo(CSI_FINISH);
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

		GObject * pParent = pObj->getParent();
		ASSERT(pParent);
		int parentid = pParent->getID();
		int afterid = index-1;
		if (afterid >= 0)
		{
			GObject * pAfterObj = pgm->FindObjectByID(afterid);
			if (pAfterObj->getParent() != pParent)
			{
				afterid = -1;
			}
		}

		pgm->MoveToUnDoList(pObj);
		PushRevertable(
			CCMake_C(COMM_I_DELETENODE, 2),
			CCMake_I(parentid),
			CCMake_I(afterid),
			CCMake_C(COMM_I_COMMAND, 2, 0),
			CCMake_C(COMM_I_COMM_WORKINGLAYER, workingLayerID),
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