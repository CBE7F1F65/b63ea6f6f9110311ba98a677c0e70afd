#include "StdAfx.h"
#include "RenderHelper.h"

#include "MathHelper.h"

RenderHelper::RenderHelper(void)
{
	hge = hgeCreate(HGE_VERSION);
	pguic = &GUICoordinate::getInstance();
	style = 0;
}

RenderHelper::~RenderHelper(void)
{
}

void RenderHelper::Release()
{
	if (hge)
	{
		hge->Release();
	}
}

void RenderHelper::BaseRenderPoint_S( float x, float y, DWORD col/*=0*/ )
{
	hge->Gfx_RenderPoint(x, y, 0, col);
}

void RenderHelper::BaseRenderLine_S( float x1, float y1, float x2, float y2, DWORD col/*=0*/ )
{
#define _RHDOTTEDLINE_SPACE	5
#define _RHSLASHLINE_SPACE	10
	switch (style)
	{
	case RHLINESTYLE_LINE:
		hge->Gfx_RenderLine(x1, y1, x2, y2, col);
		break;
	case RHLINESTYLE_DOTTEDLINE:
		if (true)
		{
			float length = MathHelper::getInstance().LineSegmentLength(PointF2D(x1, y1), PointF2D(x2, y2));
			float ivx;
			float ivy;
			for (float f=0; f<length; f+=_RHDOTTEDLINE_SPACE)
			{
				ivx = x1+(x2-x1)*f/length;
				ivy = y1+(y2-y1)*f/length;
				hge->Gfx_RenderPoint(ivx, ivy, 0, col);
			}
			hge->Gfx_RenderPoint(x2, y2, 0, col);
		}
		break;
	case RHLINESTYLE_SLASHLINE:
		if (true)
		{
			float length = MathHelper::getInstance().LineSegmentLength(PointF2D(x1, y1), PointF2D(x2, y2));
			if (length < _RHSLASHLINE_SPACE)
			{
				hge->Gfx_RenderLine(x1, y1, x2, y2, col);
			}
			float ivx;
			float ivy;
			float ivxlast=x1;
			float ivylast=y1;
			int i=0;
			for (float f=_RHSLASHLINE_SPACE; f<length; f+=_RHSLASHLINE_SPACE)
			{
				ivx = x1+(x2-x1)*f/length;
				ivy = y1+(y2-y1)*f/length;
				i++;
				if (i%2)
				{
					hge->Gfx_RenderLine(ivxlast, ivylast, ivx, ivy, col);
				}
				ivxlast = ivx;
				ivylast = ivy;
			}
			hge->Gfx_RenderPoint(x2, y2, 0, col);
		}
		break;
	}
}

void RenderHelper::RenderPoint_S( float x, float y, DWORD col/*=0*/ )
{
	if (x >= 0 && x <= pguic->GetScreenWidth_S() && y >= 0 && y <= pguic->GetScreenHeight_S())
	{
		BaseRenderPoint_S(x, y, col);
	}
}

void RenderHelper::RenderLine_S( float x1, float y1, float x2, float y2, DWORD col/*=0*/ )
{
	if (MathHelper::getInstance().LinePartialInRect(x1, y1, x2, y2, 0, 0, pguic->GetScreenWidth_S(), pguic->GetScreenHeight_S(), true))
	{
		BaseRenderLine_S(x1, y1, x2, y2, col);
//		hge->Gfx_RenderLine(x1, y1, x2, y2, col);
	}
}

void RenderHelper::RenderLineR_S( float x, float y, float length, DWORD col/*=0*/ )
{
	RenderLine_S(x, y, x+length, y, col);
}

void RenderHelper::RenderLineB_S( float x, float y, float length, DWORD col/*=0*/ )
{
	RenderLine_S(x, y, x, y+length, col);
}

void RenderHelper::RenderSquare_S( float x, float y, float a, DWORD col/*=0*/ )
{
	RenderRect_S(x, y, a, a, col);
}

void RenderHelper::RenderRect_S( float x, float y, float w, float h, DWORD col/*=0*/ )
{
	RenderLine_S(x, y, x+w, y, col);
	RenderLine_S(x+w, y, x+w, y+h, col);
	RenderLine_S(x+w, y+h, x, y+h, col);
	RenderLine_S(x, y+h, x, y, col);
}

void RenderHelper::RenderArrow_S( float x, float y, int angle, float length, float arrowsize, DWORD col/*=0*/ )
{
	angle = MathHelper::getInstance().AngleRestrict(angle);
	if (angle == 0)
	{
		return RenderArrowR_S(x, y, length, arrowsize, col);
	}
	if (angle == ANGLEBASE_90)
	{
		return RenderArrowB_S(x, y, length, arrowsize, col);
	}
	if (angle == ANGLEBASE_90*2)
	{
		return RenderArrowL_S(x, y, length, arrowsize, col);
	}
	if (angle == ANGLEBASE_90*3)
	{
		return RenderArrowT_S(x, y, length, arrowsize, col);
	}

	float arc = ARC(angle);
	float arrowpointx = x;
	float arrowpointy = y;
	if (length)
	{
		arrowpointx += length*cosf(arc);
		arrowpointy += length*sinf(arc);
		RenderLine_S(x, y, arrowpointx, arrowpointy, col);
	}
	float arcl = arc+ARC(ANGLEBASE_90/2*3);
	float arcb = arc-ARC(ANGLEBASE_90/2*3);
	RenderLine_S(arrowpointx, arrowpointy, arrowpointx+arrowsize*cosf(arcl), arrowpointy+arrowsize*sinf(arcl), col);
	RenderLine_S(arrowpointx, arrowpointy, arrowpointx+arrowsize*cosf(arcb), arrowpointy+arrowsize*sinf(arcb), col);
}

void RenderHelper::RenderArrowR_S( float x, float y, float length, float arrowsize, DWORD col/*=0*/ )
{
	if (length)
	{
		RenderLine_S(x, y, x+length, y, col);
	}
	RenderLine_S(x+length, y, x+length-arrowsize, y-arrowsize, col);
	RenderLine_S(x+length, y, x+length-arrowsize, y+arrowsize, col);
}

void RenderHelper::RenderArrowB_S( float x, float y, float length, float arrowsize, DWORD col/*=0*/ )
{
	if (length)
	{
		RenderLine_S(x, y, x, y+length, col);
	}
	RenderLine_S(x, y+length, x+arrowsize, y+length-arrowsize, col);
	RenderLine_S(x, y+length, x-arrowsize, y+length-arrowsize, col);
}

inline void RenderHelper::RenderArrowL_S( float x, float y, float length, float arrowsize, DWORD col/*=0*/ )
{
	if (length)
	{
		RenderLine_S(x, y, x-length, y, col);
	}
	RenderLine_S(x-length, y, x-length+arrowsize, y-arrowsize, col);
	RenderLine_S(x-length, y, x-length+arrowsize, y+arrowsize, col);
}

void RenderHelper::RenderArrowT_S( float x, float y, float length, float arrowsize, DWORD col/*=0*/ )
{
	if (length)
	{
		RenderLine_S(x, y, x, y-length, col);
	}
	RenderLine_S(x, y-length, x+arrowsize, y-length+arrowsize, col);
	RenderLine_S(x, y-length, x-arrowsize, y-length+arrowsize, col);
}

void RenderHelper::TargetQuadRender_S( HTARGET tar, float x, float y, DWORD col/*=0*/ )
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

void RenderHelper::BeginRenderTar( HTARGET tar, DWORD col/*=0*/ )
{
	hge->Gfx_BeginScene(tar);
	hge->Gfx_Clear(col);
}

void RenderHelper::EndRenderTar()
{
	hge->Gfx_EndScene();
}

void RenderHelper::RenderBezier( PointF2D p1, PointF2D p2, PointF2D p3, PointF2D p4, DWORD col/*=0*/, float precisemul/*=1.0f*/ )
{
	// Anchor: p1, p4
	// Control: p2, p3

	// Calculate proper section
	float l23 = MathHelper::getInstance().LineSegmentLengthPow2(p2, p3);
	float l12 = MathHelper::getInstance().LineSegmentLengthPow2(p1, p2);
	float l34 = MathHelper::getInstance().LineSegmentLengthPow2(p3, p4);

	float l = l23;
	if (l12 + l34 > l23)
	{
		l = l12 + l34;
	}

	if (l < 1.0f)
	{
		l = 1.0f;
	}
	l = sqrtf(l);
	l *= precisemul;
	int nseg = (int)(l+1.0f);

	float s = 0;
	PointF2D pqlast(0, 0);
	PointF2D pq(0, 0);

	int savedstyle = style;
	style = RHLINESTYLE_LINE;
	MathHelper::getInstance().CalculateBezier(p1, p2, p3, p4, s, &pq);
	for (int i=0; i<nseg; i++)
	{
		pqlast = pq;
		s = (float)(i+1)/(float)nseg;
		MathHelper::getInstance().CalculateBezier(p1, p2, p3, p4, s, &pq);

		switch (savedstyle)
		{
		case RHLINESTYLE_LINE:
			RenderLine(pqlast.x, pqlast.y, pq.x, pq.y, col);
			break;
		case RHLINESTYLE_DOTTEDLINE:
			RenderPoint(pqlast.x, pqlast.y, col);
			if (i == nseg-1)
			{
				RenderPoint(pq.x, pq.y, col);
			}
			break;
		case RHLINESTYLE_SLASHLINE:
			if (i%4<2)
			{
				RenderLine(pqlast.x, pqlast.y, pq.x, pq.y, col);
			}
			if (i == nseg-1)
			{
				RenderLine(pqlast.x, pqlast.y, pq.x, pq.y, col);
			}
			break;
		}
	}
	style = savedstyle;
}

void RenderHelper::SetLineStyle( int _style/*=0*/ )
{
	style = _style;
}