#include "StdAfx.h"
#include "GBaseNode.h"


GBaseNode::GBaseNode()
{
}


GBaseNode::~GBaseNode(void)
{
}

GMainBaseNode::GMainBaseNode()
{
	setTreeBase(this);
}

GMainBaseNode::~GMainBaseNode()
{

}