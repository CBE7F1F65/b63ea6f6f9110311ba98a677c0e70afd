#include "StdAfx.h"
#include "RenderHelper.h"

#include "MathHelper.h"
#include "MainInterface.h"

#define _GATTRPT_RENDER_A	5
#define _GSUBSPT_RENDER_L	5

RenderHelper::RenderHelper(void)
{
	hge = hgeCreate(HGE_VERSION);
	pguic = &GUICoordinate::getInstance();
	pdxfw = &DXFWriter::getInstance();
	style = 0;

	ppath = NULL;
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


void RenderHelper::TrueBaseRenderPoint_S( float x, float y, DWORD col )
{
	if (ppath)
	{
		float tx = (x+printXOffset)*printMul;
		float ty = (y+printYOffset)*printMul;
		ppath->moveTo(tx, ty);
		ppath->lineTo(tx, ty);
	}
	else
	{
		hge->Gfx_RenderPoint(x, y, 0, col);
	}
	pdxfw->WriteLine(x, y, x, y, 13);
}

void RenderHelper::TrueBaseRenderLine_S( float x1, float y1, float x2, float y2, DWORD col )
{
	if (ppath)
	{
		float tx1 = (x1+printXOffset)*printMul;
		float ty1 = (y1+printYOffset)*printMul;
		float tx2 = (x2+printXOffset)*printMul;
		float ty2 = (y2+printYOffset)*printMul;
		ppath->moveTo(tx1, ty1);
		ppath->lineTo(tx2, ty2);
	}
	else
	{
		hge->Gfx_RenderLine(x1, y1, x2, y2, col);
	}
	pdxfw->WriteLine(x1, y1, x2, y2, 13);
}

void RenderHelper::BaseRenderPoint_S( float x, float y, DWORD col )
{
	if (!ppath && !(x >= 0 && x <= pguic->GetScreenWidth_S() && y >= 0 && y <= pguic->GetScreenHeight_S()))
	{
		return;
	}
	TrueBaseRenderPoint_S(x, y ,col);
}

void RenderHelper::BaseRenderLine_S( float x1, float y1, float x2, float y2, DWORD col )
{
	if (!ppath && !MathHelper::getInstance().LinePartialInRect(x1, y1, x2, y2, 0, 0, pguic->GetScreenWidth_S(), pguic->GetScreenHeight_S(), true))
	{
		return;
	}
#define _RHDOTTEDLINE_SPACE	5
#define _RHSLASHLINE_SPACE	10
	switch (style)
	{
	case RHLINESTYLE_LINE:
		TrueBaseRenderLine_S(x1, y1, x2, y2, col);
//		hge->Gfx_RenderLine(x1, y1, x2, y2, col);
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
				TrueBaseRenderPoint_S(ivx, ivy, col);
//				hge->Gfx_RenderPoint(ivx, ivy, 0, col);
			}
			TrueBaseRenderPoint_S(x2, y2, col);
//			hge->Gfx_RenderPoint(x2, y2, 0, col);
		}
		break;
	case RHLINESTYLE_SLASHLINE:
		if (true)
		{
			float length = MathHelper::getInstance().LineSegmentLength(PointF2D(x1, y1), PointF2D(x2, y2));
			if (length < _RHSLASHLINE_SPACE)
			{
				TrueBaseRenderLine_S(x1, y1, x2, y2, col);
//				hge->Gfx_RenderLine(x1, y1, x2, y2, col);
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
					TrueBaseRenderLine_S(ivxlast, ivylast, ivx, ivy, col);
//					hge->Gfx_RenderLine(ivxlast, ivylast, ivx, ivy, col);
				}
				ivxlast = ivx;
				ivylast = ivy;
			}
			TrueBaseRenderPoint_S(x2, y2, col);
//			hge->Gfx_RenderPoint(x2, y2, 0, col);
		}
		break;
	}
}

void RenderHelper::RenderPoint_S( float x, float y, DWORD col )
{
//	if (x >= 0 && x <= pguic->GetScreenWidth_S() && y >= 0 && y <= pguic->GetScreenHeight_S())
//	{
		BaseRenderPoint_S(x, y, col);
//	}
}

void RenderHelper::RenderLine_S( float x1, float y1, float x2, float y2, DWORD col )
{
//	if (MathHelper::getInstance().LinePartialInRect(x1, y1, x2, y2, 0, 0, pguic->GetScreenWidth_S(), pguic->GetScreenHeight_S(), true))
//	{
		BaseRenderLine_S(x1, y1, x2, y2, col);
//		hge->Gfx_RenderLine(x1, y1, x2, y2, col);
//	}
}

void RenderHelper::RenderLineR_S( float x, float y, float length, DWORD col )
{
	RenderLine_S(x, y, x+length, y, col);
}

void RenderHelper::RenderLineB_S( float x, float y, float length, DWORD col )
{
	RenderLine_S(x, y, x, y+length, col);
}

void RenderHelper::RenderSquare_S( float x, float y, float a, DWORD col )
{
	RenderRect_S(x, y, a, a, col);
}

void RenderHelper::RenderRect_S( float x, float y, float w, float h, DWORD col )
{
	RenderLine_S(x, y, x+w, y, col);
	RenderLine_S(x+w, y, x+w, y+h, col);
	RenderLine_S(x+w, y+h, x, y+h, col);
	RenderLine_S(x, y+h, x, y, col);
}

void RenderHelper::RenderArrow_S( float x, float y, int angle, float length, float arrowsize, DWORD col )
{
	angle = MathHelper::getInstance().RestrictAngle(angle);
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

void RenderHelper::RenderArrowR_S( float x, float y, float length, float arrowsize, DWORD col )
{
	if (length)
	{
		RenderLine_S(x, y, x+length, y, col);
	}
	RenderLine_S(x+length, y, x+length-arrowsize, y-arrowsize, col);
	RenderLine_S(x+length, y, x+length-arrowsize, y+arrowsize, col);
}

void RenderHelper::RenderArrowB_S( float x, float y, float length, float arrowsize, DWORD col )
{
	if (length)
	{
		RenderLine_S(x, y, x, y+length, col);
	}
	RenderLine_S(x, y+length, x+arrowsize, y+length-arrowsize, col);
	RenderLine_S(x, y+length, x-arrowsize, y+length-arrowsize, col);
}

inline void RenderHelper::RenderArrowL_S( float x, float y, float length, float arrowsize, DWORD col )
{
	if (length)
	{
		RenderLine_S(x, y, x-length, y, col);
	}
	RenderLine_S(x-length, y, x-length+arrowsize, y-arrowsize, col);
	RenderLine_S(x-length, y, x-length+arrowsize, y+arrowsize, col);
}

void RenderHelper::RenderArrowT_S( float x, float y, float length, float arrowsize, DWORD col )
{
	if (length)
	{
		RenderLine_S(x, y, x, y-length, col);
	}
	RenderLine_S(x, y-length, x+arrowsize, y-length+arrowsize, col);
	RenderLine_S(x, y-length, x-arrowsize, y-length+arrowsize, col);
}

void RenderHelper::TargetQuadRender_S( HTARGET tar, float x, float y, DWORD col )
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

/*
void RenderHelper::RenderBezier( PointF2D pb, PointF2D pbh, PointF2D peh, PointF2D pe, DWORD col/ *=0* /, float precisemul/ *=1.0f* / )
{
	ASSERT(false);
	// Do Not Use

	// Anchor: p1, p4
	// Control: p2, p3

	// Calculate proper section
	float l23 = MathHelper::getInstance().LineSegmentLengthPow2(pbh, peh);
	float l12 = MathHelper::getInstance().LineSegmentLengthPow2(pb, pbh);
	float l34 = MathHelper::getInstance().LineSegmentLengthPow2(peh, pe);

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
	MathHelper::getInstance().CalculateBezier(pb, pbh, peh, pe, s, &pq);
	for (int i=0; i<nseg; i++)
	{
		pqlast = pq;
		s = (float)(i+1)/(float)nseg;
		MathHelper::getInstance().CalculateBezier(pb, pbh, peh, pe, s, &pq);

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
}*/

void RenderHelper::SetLineStyle( int _style/*=0*/ )
{
	style = _style;
}

void RenderHelper::RenderAttributePoint_S( float x, float y, DWORD col )
{
	RenderSquare_S(x-_GATTRPT_RENDER_A, y-_GATTRPT_RENDER_A, _GATTRPT_RENDER_A*2, col);

}

void RenderHelper::RenderSubstantivePoint_S( float x, float y, DWORD col )
{
	RenderLine_S(x-_GSUBSPT_RENDER_L, y-_GSUBSPT_RENDER_L, x+_GSUBSPT_RENDER_L, y+_GSUBSPT_RENDER_L, col);
	RenderLine_S(x+_GSUBSPT_RENDER_L, y-_GSUBSPT_RENDER_L, x-_GSUBSPT_RENDER_L, y+_GSUBSPT_RENDER_L, col);

}

void RenderHelper::RenderHandlePoint_S( float x, float y, DWORD col )
{
	for (int i=-_GATTRPT_RENDER_A; i<=_GATTRPT_RENDER_A; i++)
	{
		RenderLineR_S(x-_GATTRPT_RENDER_A, y+i, _GATTRPT_RENDER_A*2, col);
	}
}

void RenderHelper::RenderBezierByInfo( BezierSublinesInfo * bsinfo, DWORD col, float fsa/*=0.0f*/, float fmul/*=1.0f*/ )
{
	if (!bsinfo)
	{
		return;
	}
	int nseg = bsinfo->GetSubPointsCount()-1;
	if (nseg <= 0)
	{
		return;
	}

	int savedstyle = style;
	style = RHLINESTYLE_LINE;

	float xoffset = 0;
	float yoffset = 0;

	MathHelper * pmh = &MathHelper::getInstance();

	float fSpace=0;
	float space_c = GUICoordinate::getInstance().StoCs(_RHSLASHLINE_SPACE);

	PointF2D pt0 = bsinfo->GetPoint(0);
	PointF2D pt1 = bsinfo->GetPoint(1);
	PointF2D pte0 = bsinfo->GetPoint(nseg);
	PointF2D pte1 = bsinfo->GetPoint(nseg-1);
	PointF2D ptMinus;
	PointF2D ptPlus;
	bool bHaveMinus = false;
	bool bHavePlus = false;

	if (fsa)
	{
		if (!pt0.StrictEquals(pt1))
		{
			ptMinus = (pt0-pt1)*fmul*(fsa/pmh->LineSegmentLength(pt0, pt1))+pt0;
			bHaveMinus = true;
		}
		if (!pte0.StrictEquals(pte1))
		{
			ptPlus = (pte0-pte1)*fmul*(fsa/pmh->LineSegmentLength(pte0, pte1))+pte0;
			bHavePlus = true;
		}
	}

	for (int i=0; i<nseg; i++)
	{

		if (fsa)
		{
			PointF2D ptNormal = pmh->GetNormal(bsinfo->GetPoint(i), bsinfo->GetPoint(i+1), fsa);
			xoffset = ptNormal.x;
			yoffset = ptNormal.y;
		}

		switch (savedstyle)
		{
		case RHLINESTYLE_LINE:
			{
				RenderLine(bsinfo->GetX(i)+xoffset, bsinfo->GetY(i)+yoffset, bsinfo->GetX(i+1)+xoffset, bsinfo->GetY(i+1)+yoffset, col);

			}
			break;
		case RHLINESTYLE_DOTTEDLINE:
			{
				style = savedstyle;
				RenderLine(bsinfo->GetX(i)+xoffset, bsinfo->GetY(i)+yoffset, bsinfo->GetX(i+1)+xoffset, bsinfo->GetY(i+1)+yoffset, col);
			}
			break;
		case RHLINESTYLE_SLASHLINE:
			{
				if (fSpace < space_c)
				{
					RenderLine(bsinfo->GetX(i)+xoffset, bsinfo->GetY(i)+yoffset, bsinfo->GetX(i+1)+xoffset, bsinfo->GetY(i+1)+yoffset, col);
				}
				float ofSpace = fSpace;
				fSpace += bsinfo->GetLength(i);
				if (fSpace >= space_c*2)
				{
					if (ofSpace < space_c)
					{
						fSpace = space_c;
					}
					else
					{
						fSpace = 0;
					}
				}
				if (i == nseg-1)
				{
					RenderLine(bsinfo->GetX(nseg-1)+xoffset, bsinfo->GetY(nseg-1)+yoffset, bsinfo->GetX(nseg)+xoffset, bsinfo->GetY(nseg)+yoffset, col);
				}
			}
		}
		if (i == 0 && bHaveMinus)
		{
			RenderLine(ptMinus.x+xoffset, ptMinus.y+yoffset, bsinfo->GetX(i)+xoffset, bsinfo->GetY(i)+yoffset, col);
		}
		if (i == nseg-1 && bHavePlus)
		{
			RenderLine(bsinfo->GetX(i)+xoffset, bsinfo->GetY(i)+yoffset, ptPlus.x+xoffset, ptPlus.y+yoffset, col);
		}
	}

	style = savedstyle;

}

void RenderHelper::RenderArc_S( float x, float y, float r, int beginangle, int endangle, DWORD col )
{
	MathHelper * pmh = &MathHelper::getInstance();
	if (beginangle > endangle)
	{
		int tangle = beginangle;
		beginangle = endangle;
		endangle = tangle;
	}
	pmh->RestrictAngle(&beginangle);
	pmh->RestrictAngle(&endangle);
	if (endangle < beginangle)
	{
		endangle += ANGLEBASE_360;
	}

	float fArcLength = ARC(endangle-beginangle)*r;
	float fPrecision = MainInterface::getInstance().GetPrecision();
	int nseg = (int)(fPrecision*fArcLength+1.0f);
	int anglediff = endangle-beginangle;

	float bx = x+r*cost(beginangle);
	float by = y+r*sint(beginangle);

	PointF2D pqlast(bx, by);
	PointF2D pq(bx, by);

	int savedstyle = style;
	SetLineStyle();

	switch (savedstyle)
	{
	case RHLINESTYLE_LINE:
		{
			for (int i=1; i<nseg; i++)
			{
				pqlast = pq;
				float s = (float)i/(float)nseg;
				int nowangle = anglediff*s+beginangle;
				pq.x = x+r*cost(nowangle);
				pq.y = y+r*sint(nowangle);
				RenderLine_S(pqlast.x, pqlast.y, pq.x, pq.y, col);
			}
		}
		break;
	case RHLINESTYLE_DOTTEDLINE:
		{
			int angledotted = ANGLE((float)_RHDOTTEDLINE_SPACE)/r;
			if (!angledotted)
			{
				DASSERT(false);
				angledotted = 1;
			}
			RenderPoint_S(pqlast.x, pqlast.y, col);
			for (int i=endangle; i>beginangle; i-=angledotted)
			{
				int nowangle = i;
				pq.x = x+r*cost(nowangle);
				pq.y = y+r*sint(nowangle);
				RenderPoint_S(pq.x, pq.y, col);
			}
		}
		break;
	case RHLINESTYLE_SLASHLINE:
		{
			float fSpace = 0;
			for (int i=1; i<nseg; i++)
			{
				pqlast = pq;
				float s = (float)i/(float)nseg;
				int nowangle = anglediff*s+beginangle;
				pq.x = x+r*cost(nowangle);
				pq.y = y+r*sint(nowangle);
				if (fSpace < _RHSLASHLINE_SPACE)
				{
					RenderLine_S(pqlast.x, pqlast.y, pq.x, pq.y, col);
				}
				float ofSpace = fSpace;
				fSpace += pmh->LineSegmentLength(pq, pqlast);
				if (fSpace >= _RHSLASHLINE_SPACE*2)
				{
					if (ofSpace < _RHDOTTEDLINE_SPACE)
					{
						fSpace = 0;
					}
					else
					{
						fSpace = _RHDOTTEDLINE_SPACE;
					}
				}
			}
		}
		break;
	}
	SetLineStyle(savedstyle);
}

void RenderHelper::RenderCircle_S( float x, float y, float r, DWORD col )
{
	RenderArc_S(x, y, r, 0, ANGLEBASE_360, col);
}

void RenderHelper::RenderLineMeasureMark( float x1, float y1, float x2, float y2, DWORD col )
{
#define _RHLINEMEASUREMARK_MARKERLENGTH	48
	PointF2D pt1(x1, y1);
	PointF2D pt2(x2, y2);
	MathHelper * pmh = &MathHelper::getInstance();
	PointF2D ptp1, ptp2;
	float l = GUICoordinate::getInstance().StoCs(_RHLINEMEASUREMARK_MARKERLENGTH);

	int angle = pmh->GetLineAngle(pt1, pt2);
	int lineangle = angle;

	angle += ANGLEBASE_90;
	pmh->RestrictAngle(&angle);
	if (angle >= 0)
	{
		angle += ANGLEBASE_180;
	}
	float xdiff = l*cost(angle);
	float ydiff = l*sint(angle);

	PointF2D ptdiff(xdiff, ydiff);
	ptp1 = pt1+ptdiff;
	ptp2 = pt2+ptdiff;

	float linglength = pmh->LineSegmentLength(pt1, pt2);
	PointF2D ptarcpoint(pt1.x+linglength, pt1.y);

	int savedstyle = style;
	SetLineStyle(RHLINESTYLE_DOTTEDLINE);
	RenderLine(x1, y1, ptp1.x, ptp1.y, col);
	RenderLine(x2, y2, ptp2.x, ptp2.y, col);
	RenderLine(ptp1.x, ptp1.y, ptp2.x, ptp2.y, col);
	RenderLine(pt1.x, pt1.y, ptarcpoint.x, ptarcpoint.y, col);
	RenderArc(pt1.x, pt1.y, linglength, 0, lineangle, col);
	SetLineStyle(savedstyle);
}

void RenderHelper::SetPrintMode( QPainterPath * path/*=NULL*/, float xoffset/*=0*/, float yoffset/*=0*/, float mul/*=1.0f*/ )
{
	ppath = path;
	printXOffset = xoffset;
	printYOffset = yoffset;
	printMul = mul;
}
