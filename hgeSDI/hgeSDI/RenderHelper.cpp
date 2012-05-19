#include "StdAfx.h"
#include "RenderHelper.h"

#include "MathHelper.h"

HGE * RenderHelper::hge = hgeCreate(HGE_VERSION);

RenderHelper::RenderHelper(void)
{
}


RenderHelper::~RenderHelper(void)
{
}

void RenderHelper::RenderLine( float x1, float y1, float x2, float y2, DWORD col/*=0*/ )
{
	hge->Gfx_RenderLine(x1, y1, x2, y2, col);
}

void RenderHelper::RenderLineR( float x, float y, float length, DWORD col/*=0*/ )
{
	RenderLine(x, y, x+length, y, col);
}

void RenderHelper::RenderLineB( float x, float y, float length, DWORD col/*=0*/ )
{
	RenderLine(x, y, x, y+length, col);
}

void RenderHelper::RenderSquare( float x, float y, float a, DWORD col/*=0*/ )
{
	RenderRect(x, y, a, a, col);
}

void RenderHelper::RenderRect( float x, float y, float a, float b, DWORD col/*=0*/ )
{
	RenderLine(x, y, x+a, y, col);
	RenderLine(x+a, y, x+a, y+b, col);
	RenderLine(x+a, y+b, x, y+b, col);
	RenderLine(x, y+b, x, y, col);
}

void RenderHelper::RenderArrow( float x, float y, int angle, float length, float arrowsize, DWORD col/*=0*/ )
{
	angle = MathHelper::AngleRestrict(angle);
	if (angle == 0)
	{
		return RenderArrowR(x, y, length, arrowsize, col);
	}
	if (angle == ANGLEBASE_90)
	{
		return RenderArrowB(x, y, length, arrowsize, col);
	}
	if (angle == ANGLEBASE_90*2)
	{
		return RenderArrowL(x, y, length, arrowsize, col);
	}
	if (angle == ANGLEBASE_90*3)
	{
		return RenderArrowT(x, y, length, arrowsize, col);
	}

	float arc = ARC(angle);
	float arrowpointx = x;
	float arrowpointy = y;
	if (length)
	{
		arrowpointx += length*cosf(arc);
		arrowpointy += length*sinf(arc);
		RenderLine(x, y, arrowpointx, arrowpointy, col);
	}
	float arcl = arc+ARC(ANGLEBASE_90/2*3);
	float arcb = arc-ARC(ANGLEBASE_90/2*3);
	RenderLine(arrowpointx, arrowpointy, arrowpointx+arrowsize*cosf(arcl), arrowpointy+arrowsize*sinf(arcl), col);
	RenderLine(arrowpointx, arrowpointy, arrowpointx+arrowsize*cosf(arcb), arrowpointy+arrowsize*sinf(arcb), col);
}

void RenderHelper::RenderArrowR( float x, float y, float length, float arrowsize, DWORD col/*=0*/ )
{
	if (length)
	{
		RenderLine(x, y, x+length, y, col);
	}
	RenderLine(x+length, y, x+length-arrowsize, y-arrowsize, col);
	RenderLine(x+length, y, x+length-arrowsize, y+arrowsize, col);
}

void RenderHelper::RenderArrowB( float x, float y, float length, float arrowsize, DWORD col/*=0*/ )
{
	if (length)
	{
		RenderLine(x, y, x, y+length, col);
	}
	RenderLine(x, y+length, x+arrowsize, y+length-arrowsize, col);
	RenderLine(x, y+length, x-arrowsize, y+length-arrowsize, col);
}

inline void RenderHelper::RenderArrowL( float x, float y, float length, float arrowsize, DWORD col/*=0*/ )
{
	if (length)
	{
		RenderLine(x, y, x-length, y, col);
	}
	RenderLine(x-length, y, x-length+arrowsize, y-arrowsize, col);
	RenderLine(x-length, y, x-length+arrowsize, y+arrowsize, col);
}

void RenderHelper::RenderArrowT( float x, float y, float length, float arrowsize, DWORD col/*=0*/ )
{
	if (length)
	{
		RenderLine(x, y, x, y-length, col);
	}
	RenderLine(x, y-length, x+arrowsize, y-length+arrowsize, col);
	RenderLine(x, y-length, x-arrowsize, y-length+arrowsize, col);
}