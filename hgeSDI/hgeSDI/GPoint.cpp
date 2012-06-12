#include "StdAfx.h"
#include "GPoint.h"
#include "StringManager.h"

GPoint::GPoint()
{
}
GPoint::GPoint( float _x, float _y )
{
	SetPosition(_x, _y);
}

GPoint::~GPoint(void)
{
}

void GPoint::SetPosition( float _x, float _y )
{
	x = _x;
	y = _y;
	CallModify();
}

const char * GPoint::getDisplayName()
{
	if (strDisplayName.length())
	{
		return strDisplayName.c_str();
	}
	return StringManager::getInstance().GetNNPointName();
}

bool GPoint::Clone( GObject * pNewParent )
{
	_GOBJ_CLONE_PRE(GPoint);
	_GOBJ_CLONE_POST();
}
GEndPoint::GEndPoint()
{
	setIsAttributeNode(true);
}

GEndPoint::GEndPoint( float _x, float _y )
{
	SetPosition(_x, _y);
}

GEndPoint::~GEndPoint()
{
}

const char * GEndPoint::getDisplayName()
{
	if (strDisplayName.length())
	{
		return strDisplayName.c_str();
	}
	return StringManager::getInstance().GetNNEndPointName();
}

bool GEndPoint::Clone( GObject * pNewParent )
{
	_GOBJ_CLONE_PRE(GEndPoint);
	_GOBJ_CLONE_POST();
}

GMidPoint::GMidPoint()
{
	setIsAttributeNode(true);
	setMotifyParent(false);
}

GMidPoint::GMidPoint( float _x, float _y )
{
	SetPosition(_x, _y);
}

GMidPoint::~GMidPoint()
{
}

const char * GMidPoint::getDisplayName()
{
	if (strDisplayName.length())
	{
		return strDisplayName.c_str();
	}
	return StringManager::getInstance().GetNNMidPointName();
}

bool GMidPoint::Clone( GObject * pNewParent )
{
	_GOBJ_CLONE_PRE(GMidPoint);
	_GOBJ_CLONE_POST();
}