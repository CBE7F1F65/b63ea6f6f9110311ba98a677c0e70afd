#include "StdAfx.h"
#include "GBaseNode.h"


GBaseNode::GBaseNode()
{
	setLineColor(ColorManager::getInstance().GetLayerLineColorSetByIndex());
}


GBaseNode::~GBaseNode(void)
{
}

void GBaseNode::CopyBaseTo( GBaseNode * pTo )
{
	CreateNewClone(pTo);
}

GObject * GBaseNode::CreateNewClone( GObject * pNewParent/*=NULL*/, GObject * pBeforeObj/*=NULL*/ )
{
	_GOBJ_CLONE_PRE(GBaseNode);
	_GOBJ_CLONE_POST();

}

void GBaseNode::CopyBaseFrom( GBaseNode * pFrom )
{
	if (pFrom)
	{
		GObject * pOBase = pFrom->getChildren()->back();
		if (pOBase)
		{
			for (list<GObject *>::reverse_iterator it=pOBase->getChildren()->rbegin(); it!=pOBase->getChildren()->rend(); ++it)
			{
				(*it)->CreateNewClone(this);
			}
		}
	}
}

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
		pObj->Independ();
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