#include "StdAfx.h"
#include "GBaseNode.h"


GBaseNode::GBaseNode()
{
}


GBaseNode::~GBaseNode(void)
{
}

void GBaseNode::CopyBaseTo( GBaseNode * pTo )
{
	Clone(pTo);
}

bool GBaseNode::Clone( GObject * pNewParent )
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
				(*it)->Clone(this);
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

bool GMainBaseNode::Clone( GObject * pNewParent )
{
	_GOBJ_CLONE_PRE(GBaseNode);
	_GOBJ_CLONE_POST();
// 	GBaseNode * _node = new GBaseNode();
// 	ASSERT(pNewParent);
// 	ASSERT(pNewParent != this);
// 	ASSERT(!(pNewParent->isDescendantOf(this)));
// 	*_node = *this;
// 	_node->Independ();
// 
// 	for (list<GObject *>::reverse_iterator it=getChildren()->rbegin(); it!=getChildren()->rend(); ++it)
// 	{
// 		(*it)->Clone(_node);
// 	}
// 	pNewParent->AddChild(_node);
// 	return true;
}