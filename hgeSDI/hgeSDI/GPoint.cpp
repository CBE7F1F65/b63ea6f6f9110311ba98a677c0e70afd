#include "StdAfx.h"
#include "GPoint.h"
#include "StringManager.h"
#include "ColorManager.h"
#include "GUICoordinate.h"
#include "RenderHelper.h"
#include "Main.h"
#include "Command.h"

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

bool GPoint::MoveTo( float newx, float newy, bool bTry )
{
	if (!canMove())
	{
		return false;
	}
	
	ToggleTryMoveState(bTry);

	x = newx;
	y = newy;

	CallModify();
	return true;
}
GEndPoint::GEndPoint()
{
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
	setDisplayVisible(false);
	setDisplayLock(true);
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

void GAttributePoint::OnRender( int iHighlightLevel/*=0*/ )
{
	DWORD col = getLineColor(iHighlightLevel);
	RenderHelper::getInstance().RenderAttributePoint(x, y, col);
}

void GSubstantivePoint::OnRender( int iHighlightLevel/*=0*/ )
{
	DWORD col = getLineColor(iHighlightLevel);
	RenderHelper::getInstance().RenderSubstantivePoint(x, y, col);
}