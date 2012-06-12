#include "StdAfx.h"
#include "GShape.h"
#include "StringManager.h"

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

bool GShape::Clone( GObject * pNewParent )
{
	_GOBJ_CLONE_PRE(GShape);
	_GOBJ_CLONE_POST();
}