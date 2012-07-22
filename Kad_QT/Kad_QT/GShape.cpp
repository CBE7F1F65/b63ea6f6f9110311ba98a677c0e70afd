#include "StdAfx.h"
#include "GShape.h"
#include "StringManager.h"

#include "GBaseNode.h"

GShape::GShape(void)
{
}

GShape::~GShape(void)
{
}

const char * GShape::getDisplayName()
{
	if (strDisplayName.length())
	{
		return strDisplayName.c_str();
	}
	return StringManager::getInstance().GetNNShapeName();
}

GObject * GShape::CreateNewClone( GObject * pNewParent/*=NULL*/, GObject * pBeforeObj/*=NULL*/ )
{
	_GOBJ_CLONE_PRE(GShape);
	_GOBJ_CLONE_POST();
}