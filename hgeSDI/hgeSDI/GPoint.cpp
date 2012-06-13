#include "StdAfx.h"
#include "GPoint.h"
#include "StringManager.h"
#include "ColorManager.h"
#include "GUICoordinate.h"
#include "RenderHelper.h"

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

void GAttributePoint::OnRender( bool bHighlight/* =false */ )
{
#define _GATTRPT_RENDER_A	5
	DWORD col = getLineColor(bHighlight);
	GUICoordinate * pguic = &GUICoordinate::getInstance();
	float xs = pguic->CtoSx(x);
	float ys = pguic->CtoSy(y);
	RenderHelper::getInstance().RenderSquare_S(xs-_GATTRPT_RENDER_A, ys-_GATTRPT_RENDER_A, _GATTRPT_RENDER_A*2, col);
}

void GSubstantivePoint::OnRender( bool bHighlight/* =false */ )
{
#define _GSUBSPT_RENDER_L	5
	DWORD col = getLineColor(bHighlight);
	GUICoordinate * pguic = &GUICoordinate::getInstance();
	float xs = pguic->CtoSx(x);
	float ys = pguic->CtoSy(y);
	RenderHelper::getInstance().RenderLine_S(xs-_GSUBSPT_RENDER_L, ys-_GSUBSPT_RENDER_L, xs+_GSUBSPT_RENDER_L, ys+_GSUBSPT_RENDER_L, col);
	RenderHelper::getInstance().RenderLine_S(xs+_GSUBSPT_RENDER_L, ys-_GSUBSPT_RENDER_L, xs-_GSUBSPT_RENDER_L, ys+_GSUBSPT_RENDER_L, col);

}