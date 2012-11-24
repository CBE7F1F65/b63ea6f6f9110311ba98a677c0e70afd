#include "StdAfx.h"
#include "GBaseNode.h"
#include "GObjectManager.h"

GBaseNode::GBaseNode()
{
	setLineColor(ColorManager::getInstance().GetLayerLineColorSetByIndex());

	savedactivelayer = -1;
	savedlstselect.clear();
}


GBaseNode::~GBaseNode(void)
{
}

bool GBaseNode::CloneData( GObject * pClone, GObject * pNewParent, bool bNoRelationship/* =true */ )
{
	if (super::CloneData(pClone, pNewParent, bNoRelationship))
	{
		GBaseNode * pBaseNode = (GBaseNode *)pClone;
		pBaseNode->savedactivelayer = savedactivelayer;
		pBaseNode->savedlstselect = savedlstselect;
		return true;
	}
	return false;
}

void GBaseNode::CopyBaseTo( GBaseNodeCopyStack * pTo )
{
	ASSERT(pTo);
	GBaseNode * pNewBase = (GBaseNode *)CreateNewClone(pTo);
	if (!pNewBase)
	{
		DASSERT(false);
		return;
	}
}

GObject * GBaseNode::CreateNewClone( GObject * pNewParent/*=NULL*/, GObject * pBeforeObj/*=NULL*/ )
{
	_GOBJ_CLONE_PRE(GBaseNode);
	_GOBJ_CLONE_POST();

}

void GBaseNode::RestoreBaseFrom( GBaseNodeCopyStack * pFrom )
{
	if (pFrom)
	{
		GBaseNode * pOBase = (GBaseNode *)pFrom->getChildren()->back();
		if (pOBase)
		{
			RemoveAllChildren(true);
			for (list<GObject *>::iterator it=pOBase->getChildren()->begin(); it!=pOBase->getChildren()->end(); ++it)
			{
				(*it)->CreateNewClone(this);
			}

			pgm->ReSelect(pOBase->savedlstselect, pOBase->savedactivelayer);
		}
	}
}

void GBaseNode::CallBaseUpdate()
{
	nUpdateMoveActionID = pgm->GetNextMoveActionID(GMMATYPE_MOVE);
	CallUpdate();
}

void GBaseNode::SaveSelectState()
{
	list<GObject *> * plstselected = pgm->GetSelectedNodes(false);
	if (plstselected->empty())
	{
		return;
	}

	savedactivelayer = -1;
	savedlstselect.clear();

	if (!plstselected->empty())
	{
		for (list<GObject *>::iterator it=plstselected->begin(); it!=plstselected->end(); ++it)
		{
			savedlstselect.push_back((*it)->getID());
		}
	}

	savedactivelayer = pgm->GetActiveLayer()->getID();
}

/************************************************************************/
/* GMainBaseNode                                                        */
/************************************************************************/

GMainBaseNode::GMainBaseNode()
{
	setTreeBase(this);
}

GMainBaseNode::~GMainBaseNode()
{

}

GObject * GMainBaseNode::CreateNewClone( GObject * pNewParent/*=NULL*/, GObject * pBeforeObj/*=NULL*/ )
{
	_GOBJ_CLONE_PRE(GMainBaseNode);
	_GOBJ_CLONE_POST();
	setTreeBase(this);
}

GHistoryBaseNode::GHistoryBaseNode()
{

}

GHistoryBaseNode::~GHistoryBaseNode()
{

}

void GHistoryBaseNode::OnAddChildAfterObj( GObject * pChild, GObject * pAfter )
{
	BuildRelationship(pChild);
}

void GHistoryBaseNode::OnRemoveChild( GObject * pChild, bool bRelease )
{
	RestoreRelationship(pChild);
}

GNodeRelationshipGroup * GHistoryBaseNode::FindRelationshipGroup( GObject * pObj, bool bRemove/*=true */ )
{
	for (list<GNodeRelationshipGroup *>::reverse_iterator it=lstRelations.rbegin(); it!=lstRelations.rend(); ++it)
	{
		GNodeRelationshipGroup * pnrg = *it;
		if (pnrg->GetSelf() == pObj)
		{
			if (bRemove)
			{
				lstRelations.erase(--it.base());
			}
			return pnrg;
		}
	}
	return NULL;
}

GObject * GHistoryBaseNode::CreateNewClone( GObject * pNewParent/*=NULL*/, GObject * pBeforeObj/*=NULL*/ )
{
	ASSERT(false);
	return NULL;
}

bool GHistoryBaseNode::CloneData( GObject * pClone, GObject * pNewParent, bool bNoRelationship/*=true*/ )
{
	ASSERT(false);
	return false;
}

void GHistoryBaseNode::BuildRelationship( GObject * pObj )
{
	GNodeRelationshipGroup * pnrg = pObj->CreateRelationshipGroup();
	if (pnrg)
	{
		lstRelations.push_back(pnrg);
		pObj->CallIndepend();
	}
	for (list<GObject *>::iterator it=pObj->getChildren()->begin(); it!=pObj->getChildren()->end(); ++it)
	{
		BuildRelationship(*it);
	}
}

void GHistoryBaseNode::RestoreRelationship( GObject * pObj )
{
	GNodeRelationshipGroup * pRelGroup = FindRelationshipGroup(pObj);
	if (pRelGroup)
	{
		pRelGroup->RestoreTo(pObj);
		delete pRelGroup;
	}
	for (list<GObject *>::iterator it=pObj->getChildren()->begin(); it!=pObj->getChildren()->end(); ++it)
	{
		RestoreRelationship(*it);
	}
}