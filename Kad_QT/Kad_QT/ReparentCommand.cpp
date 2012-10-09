#include "StdAfx.h"
#include "ReparentCommand.h"


ReparentCommand::ReparentCommand(void)
{
}


ReparentCommand::~ReparentCommand(void)
{
}

void ReparentCommand::OnProcessCommand()
{
	int step = OnNormalProcessCommand();
	UpdateLastStep();

	int ret = -1;

	if (step == CSI_INIT)
	{
		pcommand->StepTo(
			CSI_REPARENT_WANTNEWPARENTINDEX,
			CWP_INDEX);
	}
	else if (step == CSI_REPARENT_WANTNEWPARENTINDEX)
	{
		ret = pcommand->ProcessPending(
			CSP_REPARENT_I_NEWPARENTINDEX, COMMPARAMFLAG_I, CWP_INDEX,
			CSI_REPARENT_WANTAFTERINDEX
			);
		if (ret < 0)
		{
			GObject * pObj = pgm->GetDragDroppedLayerNode();
			if (!pObj)
			{
				Terminal();
				return;
			}
			pcommand->SetParamI(CSP_REPARENT_I_NEWPARENTINDEX, pObj->getID(), CWP_INDEX);
			pcommand->StepTo(CSI_REPARENT_WANTAFTERINDEX);
		}
	}
	else if (step == CSI_REPARENT_WANTAFTERINDEX)
	{
		ret = pcommand->ProcessPending(
			CSP_REPARENT_I_AFTERINDEX, COMMPARAMFLAG_I, CWP_INDEX,
			CSI_REPARENT_WANTOLDINDEXES
			);
		if (ret < 0)
		{
			GObject * pObj = pgm->GetDragDroppedAfterNode();
			int afterindex = -1;
			if (pObj)
			{
				afterindex = pObj->getID();
			}
			pcommand->SetParamI(CSP_REPARENT_I_AFTERINDEX, afterindex, CWP_INDEX);
			pcommand->StepTo(CSI_REPARENT_WANTOLDINDEXES);
		}
	}
	else if (step == CSI_REPARENT_WANTOLDINDEXES)
	{
		ret = pcommand->ProcessPending(
			CSP_REPARENT_I_OLDINDEXES, COMMPARAMFLAG_I, CWP_INDEX,
			CSI_FINISH
			);

		if (ret < 0)
		{
			int newindex = pcommand->GetParamI(CSI_REPARENT_WANTNEWPARENTINDEX);

            list<GObject*> * activenodelist = pgm->GetSelectedNodes(true);
			GObject * pTemp;
            for (list<GObject*>::iterator it=activenodelist->begin(); it!= activenodelist->end();)
			{
				pTemp = *it;
				bool shoulddelete=false;
				// Active Node List should be in order
                for (list<GObject*>::iterator jt=activenodelist->begin(); jt!=it/*activenodelist->end()*/; ++jt)
				{
					if ((*it)->isAncestorOf(*jt))
					{
						shoulddelete=true;
						break;
					}
				}
				if (shoulddelete)
				{
                    it = activenodelist->erase(it);
				}
				else
				{
					++it;
				}
			}

//			GObject * pNewPlaceAfter = pgm->FindObjectByID(newindex);
			list <int> lids;
            for (list<GObject*>::reverse_iterator it=activenodelist->rbegin(); it!= activenodelist->rend(); ++it)
			{
				if (pgm->CanReparentItem((*it), newindex))
				{
					lids.push_back((*it)->getID());
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
					pcommand->SetParamI(CSP_REPARENT_I_OLDINDEXES+i, (*it), CWP_INDEX);
					i++;
				}
				pcommand->SetParamI(CSP_REPARENT_I_OLDINDEXES+i, -1, CWP_INDEX);

				pcommand->StepTo(CSI_FINISH);
			}

		}
		else
		{
			/*
			int newparentindex = pcommand->GetParamI(CSP_REPARENT_I_NEWPARENTINDEX);
			int afterindex = pcommand->GetParamI(CSP_REPARENT_I_AFTERINDEX);
			int oldindex = pcommand->GetParamI(CSP_REPARENT_I_OLDINDEXES);
			GObject * pObj = pgm->FindObjectByID(oldindex);
			if (afterindex >= 0)
			{
				GObject * pAfterObj = pgm->FindObjectByID(afterindex);
				ASSERT(pAfterObj);
				GObject * pNewParentObj = pgm->FindObjectByID(newparentindex);
				if (!pAfterObj->isDescendantOf(pNewParentObj))
				{
					Terminal();
					return;
				}
			}
			if (!pgm->CanReparentItem(pObj, newparentindex))
			{
				Terminal();
				return;
			}
			*/
			pcommand->SetParamI(CSP_REPARENT_I_OLDINDEXES+1, -1, CWP_INDEX);
		}
	}
}

void ReparentCommand::OnDoneCommand()
{
	int i=0;
	int newparentindex = pcommand->GetParamI(CSP_REPARENT_I_NEWPARENTINDEX);
	int afterindex = pcommand->GetParamI(CSP_REPARENT_I_AFTERINDEX);
	int tnewindex = newparentindex;
	GObject * pNewPlaceAfter = pgm->FindObjectByID(afterindex);
	GObject * pNewParent = pgm->FindObjectByID(newparentindex);
	if (!pNewParent)
	{
		return;
	}

	list<GObject *>listobjs;
	while (true)
	{
		int index = pcommand->GetParamI(CSP_REPARENT_I_OLDINDEXES+i);
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
		GObject * pOriParent = pObj->getParent();
		GObject * pOAfter = pObj->getYoungerSibling();

		newparentindex = pNewParent->getID();
		if (!pgm->CanReparentItem(pObj, newparentindex))
		{
			continue;
		}

		GBaseNode tempHoldingPlace;
		pObj->Reparent(&tempHoldingPlace);
		pObj->ReparentAfterObject(pNewParent, pNewPlaceAfter);
		tempHoldingPlace.RemoveAllChildren(true);

		int oparentindex = pOriParent->getID();
		int oafterindex = -1;
		if (pOAfter)
		{
			oafterindex = pOAfter->getID();
		}
		int nowindex = pObj->getID();

		PushRevertable(
			CCMake_C(COMM_I_COMMAND, 4),
			CCMake_C(COMM_I_COMM_WORKINGLAYER, workinglayerID),
			CCMake_C(COMM_REPARENT),
			CCMake_I(tnewindex),
			CCMake_I(afterindex),
			CCMake_I(index),
			NULL
			);
	}
}
