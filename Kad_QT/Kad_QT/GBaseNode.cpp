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