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

void RenderHelper::TargetQuadRender( HTARGET tar, float x, float y, DWORD col/*=0*/ )
{
	if (!tar)
	{
		return;
	}
	HTEXTURE tex = hge->Target_GetTexture(tar);
	if (!tex.tex)
	{
		return;
	}
	int tw = hge->Texture_GetWidth(tex);
	int th = hge->Texture_GetHeight(tex);

	hgeQuad quad;
	quad.blend = BLEND_DEFAULT;
	quad.tex = tex;
	quad.v[0].x = x; quad.v[0].y = y; quad.v[0].tx = 0; quad.v[0].ty = 0, quad.v[0].z = 0, quad.v[0].col = col;
	quad.v[1].x = x+tw; quad.v[1].y = y; quad.v[1].tx = 1; quad.v[1].ty = 0, quad.v[1].z = 0, quad.v[1].col = col;
	quad.v[2].x = x+tw; quad.v[2].y = y+th; quad.v[2].tx = 1; quad.v[2].ty = 1, quad.v[2].z = 0, quad.v[2].col = col;
	quad.v[3].x = x; quad.v[3].y = y+th; quad.v[3].tx = 0; quad.v[3].ty = 1, quad.v[3].z = 0, quad.v[3].col = col;

	hge->Gfx_RenderQuad(&quad);
}