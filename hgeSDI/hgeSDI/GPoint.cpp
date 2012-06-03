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
	OnModify();
}

const char * GPoint::GetTypeName()
{
	return StringManager::getInstance().GetNNPointName();
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

const char * GEndPoint::GetTypeName()
{
	return StringManager::getInstance().GetNNEndPointName();
}