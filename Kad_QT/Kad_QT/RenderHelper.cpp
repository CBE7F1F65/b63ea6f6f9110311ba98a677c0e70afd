#include "StdAfx.h"
#include "RenderHelper.h"

#include "MathHelper.h"
#include "MainInterface.h"

#include "qmaininterface.h"

#define _GATTRPT_RENDER_A	5
#define _GSUBSPT_RENDER_L	5
#define _GMIDPT_RENDER_L	5

#define _RHDIM_ALPHAAND 0xc0ffffff

RenderHelper::RenderHelper(void)
{
	hge = hgeCreate(HGE_VERSION);
	pguic = &GUICoordinate::getInstance();
	pdxfw = &DXFWriter::getInstance();
	style = 0;

	ppainter = NULL;

	pPainter = NULL;

    bPreviewPrintMode = false;
	bRenderPointIndicator = true;
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
	if (pPainter)
	{
		delete pPainter;
		pPainter = NULL;
	}
}


void RenderHelper::TrueBaseRenderPoint_S( float x, float y, DWORD col )
{
	if (ppainter)
	{
		float tx = (x+printXOffset)*printMul;
		float ty = (y+printYOffset)*printMul;
		if (GETA(col) >= 0xcf)
		{
			ppainter->setPen(Qt::black);
		}
		else
		{
			ppainter->setPen(Qt::darkGray);
		}
		ppainter->drawPoint(tx, ty);
	}
	else
	{
		hge->Gfx_RenderPoint(x, y, 0, col);
	}
	pdxfw->WriteDirectLine(x, y, x, y);
}

void RenderHelper::TrueBaseRenderLine_S( float x1, float y1, float x2, float y2, DWORD col )
{
	if (ppainter)
	{
		float tx1 = (x1+printXOffset)*printMul;
		float ty1 = (y1+printYOffset)*printMul;
		float tx2 = (x2+printXOffset)*printMul;
		float ty2 = (y2+printYOffset)*printMul;
		if (GETA(col) >= 0xcf)
		{
			ppainter->setPen(Qt::black);
		}
		else
		{
			ppainter->setPen(Qt::darkGray);
		}
		ppainter->drawLine(tx1, ty1, tx2, ty2);
	}
	else
	{
		hge->Gfx_RenderLine(x1, y1, x2, y2, col);
	}
	pdxfw->WriteDirectLine(x1, y1, x2, y2);
}

void RenderHelper::BaseRenderPoint_S( float x, float y, DWORD col )
{
	if (!ppainter && !pdxfw->GetDXF() && !(x >= 0 && x <= pguic->GetScreenWidth_S() && y >= 0 && y <= pguic->GetScreenHeight_S()))
	{
		return;
	}
	TrueBaseRenderPoint_S(x, y ,col);
}

void RenderHelper::BaseRenderLine_S( float x1, float y1, float x2, float y2, DWORD col )
{
	if (!ppainter && !pdxfw->GetDXF() && !MathHelper::getInstance().LinePartialInRect(x1, y1, x2, y2, 0, 0, pguic->GetScreenWidth_S(), pguic->GetScreenHeight_S(), true))
	{
		return;
    }
#define _RHDOTTEDLINE_SPACE	5
#define _RHSLASHLINE_SPACE	10
    switch (ResolveLineStyle(style, col))
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
	if (!bRenderPointIndicator)
	{
		return;
	}
	RenderSquare_S(x-_GATTRPT_RENDER_A, y-_GATTRPT_RENDER_A, _GATTRPT_RENDER_A*2, col);

}

void RenderHelper::RenderSubstantivePoint_S( float x, float y, DWORD col )
{
	RenderLine_S(x-_GSUBSPT_RENDER_L, y-_GSUBSPT_RENDER_L, x+_GSUBSPT_RENDER_L, y+_GSUBSPT_RENDER_L, col);
	RenderLine_S(x+_GSUBSPT_RENDER_L, y-_GSUBSPT_RENDER_L, x-_GSUBSPT_RENDER_L, y+_GSUBSPT_RENDER_L, col);

}

void RenderHelper::RenderMidPoint_S( float x, float y, DWORD col )
{
	/*
	RenderLine_S(x-_GMIDPT_RENDER_L, y+_GMIDPT_RENDER_L, x+_GMIDPT_RENDER_L, y+_GMIDPT_RENDER_L, col);
	RenderLine_S(x-_GMIDPT_RENDER_L, y+_GMIDPT_RENDER_L, x, y-_GMIDPT_RENDER_L, col);
	RenderLine_S(x+_GMIDPT_RENDER_L, y+_GMIDPT_RENDER_L, x, y-_GMIDPT_RENDER_L, col);
	*/
	if (!bRenderPointIndicator)
	{
		return;
	}
	float fmul=1.0f/3.0f;
	RenderLine_S(x-_GSUBSPT_RENDER_L, y-_GSUBSPT_RENDER_L, x-_GSUBSPT_RENDER_L*fmul, y-_GSUBSPT_RENDER_L, col);
	RenderLine_S(x+_GSUBSPT_RENDER_L*fmul, y-_GSUBSPT_RENDER_L, x+_GSUBSPT_RENDER_L, y-_GSUBSPT_RENDER_L, col);

	RenderLine_S(x+_GSUBSPT_RENDER_L, y-_GSUBSPT_RENDER_L, x+_GSUBSPT_RENDER_L, y-_GSUBSPT_RENDER_L*fmul, col);
	RenderLine_S(x+_GSUBSPT_RENDER_L, y+_GSUBSPT_RENDER_L*fmul, x+_GSUBSPT_RENDER_L, y+_GSUBSPT_RENDER_L, col);

	RenderLine_S(x+_GSUBSPT_RENDER_L, y+_GSUBSPT_RENDER_L, x+_GSUBSPT_RENDER_L*fmul, y+_GSUBSPT_RENDER_L, col);
	RenderLine_S(x-_GSUBSPT_RENDER_L*fmul, y+_GSUBSPT_RENDER_L, x-_GSUBSPT_RENDER_L, y+_GSUBSPT_RENDER_L, col);

	RenderLine_S(x-_GSUBSPT_RENDER_L, y+_GSUBSPT_RENDER_L, x-_GSUBSPT_RENDER_L, y+_GSUBSPT_RENDER_L*fmul, col);
	RenderLine_S(x-_GSUBSPT_RENDER_L, y-_GSUBSPT_RENDER_L*fmul, x-_GSUBSPT_RENDER_L, y-_GSUBSPT_RENDER_L, col);
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
	if (!bPreviewPrintMode)
	{
		style = RHLINESTYLE_LINE;
	}

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

        switch (ResolveLineStyle(savedstyle, col))
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

    switch (ResolveLineStyle(savedstyle, col))
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

void RenderHelper::SetPrintMode( QPainter * painter/*=NULL*/, float xoffset/*=0*/, float yoffset/*=0*/, float mul/*=1.0f*/ )
{
	ppainter = painter;
	printXOffset = xoffset;
	printYOffset = yoffset;
    printMul = mul;
}

int RenderHelper::ResolveLineStyle(int tstyle, DWORD &col)
{
    if (bPreviewPrintMode)
    {
        switch (tstyle)
        {
        case RHLINESTYLE_LINE:
			col &= _RHDIM_ALPHAAND;
            return RHLINESTYLE_LINE;
        case RHLINESTYLE_SLASHLINE:
            return RHLINESTYLE_LINE;
        }
    }
    return tstyle;
}

void RenderHelper::RenderImage_S( QImage * pImg, float x, float y, float w, float h, DWORD col )
{
	if (!pImg)
	{
		return;
	}
	if (!pPainter)
	{
		pPainter = new QPainter();
	}
	pPainter->begin(QMainInterface::getInstance().GetPGLView());
	QImage sImg = pImg->scaled(w, h);
	pPainter->drawImage(x, y, sImg);
	pPainter->end();
}

float RenderHelper::RenderCharacter_S( int ch, float x, float y, float size, DWORD col )
{
	float w = size*0.5f;
	float h = size;

	switch (ch)
	{
	case 'A':
	case 'a':
		w=0.714285714285714f*size;
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.571428571428571f, x+w*0.0666666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.571428571428571f, x+w*0.866666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.571428571428571f, x+w*0.866666666666667f, y+h*0.571428571428571f, col);
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.571428571428571f, x+w*0.466666666666667f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.571428571428571f, x+w*0.466666666666667f, y+h*0.0476190476190476f, col);
		break;
	case 'B':
	case 'b':
		w=0.714285714285714f*size;
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.0476190476190476f, x+w*0.666666666666667f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.666666666666667f, y+h*0.0476190476190476f, x+w*0.866666666666667f, y+h*0.19047619047619f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.19047619047619f, x+w*0.866666666666667f, y+h*0.285714285714286f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.285714285714286f, x+w*0.666666666666667f, y+h*0.476190476190476f, col);
		RenderLine_S(x+w*0.266666666666667f, y+h*0.476190476190476f, x+w*0.666666666666667f, y+h*0.476190476190476f, col);
		RenderLine_S(x+w*0.666666666666667f, y+h*0.476190476190476f, x+w*0.866666666666667f, y+h*0.619047619047619f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.619047619047619f, x+w*0.866666666666667f, y+h*0.714285714285714f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.714285714285714f, x+w*0.666666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.904761904761905f, x+w*0.666666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.266666666666667f, y+h*0.0476190476190476f, x+w*0.266666666666667f, y+h*0.904761904761905f, col);
		break;
	case 'C':
	case 'c':
		w=0.761904761904762f*size;
		RenderLine_S(x+w*0.875f, y+h*0.19047619047619f, x+w*0.6875f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.6875f, y+h*0.0476190476190476f, x+w*0.3125f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.3125f, y+h*0.0476190476190476f, x+w*0.125f, y+h*0.19047619047619f, col);
		RenderLine_S(x+w*0.125f, y+h*0.19047619047619f, x+w*0.125f, y+h*0.761904761904762f, col);
		RenderLine_S(x+w*0.125f, y+h*0.761904761904762f, x+w*0.3125f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.3125f, y+h*0.904761904761905f, x+w*0.6875f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.6875f, y+h*0.904761904761905f, x+w*0.875f, y+h*0.761904761904762f, col);
		break;
	case 'D':
	case 'd':
		w=0.714285714285714f*size;
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.0476190476190476f, x+w*0.666666666666667f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.666666666666667f, y+h*0.0476190476190476f, x+w*0.866666666666667f, y+h*0.19047619047619f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.19047619047619f, x+w*0.866666666666667f, y+h*0.761904761904762f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.761904761904762f, x+w*0.666666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.904761904761905f, x+w*0.666666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.266666666666667f, y+h*0.0476190476190476f, x+w*0.266666666666667f, y+h*0.904761904761905f, col);
		break;
	case 'E':
	case 'e':
		w=0.666666666666667f*size;
		RenderLine_S(x+w*0.0714285714285714f, y+h*0.0476190476190476f, x+w*0.857142857142857f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.0714285714285714f, y+h*0.0476190476190476f, x+w*0.0714285714285714f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.0714285714285714f, y+h*0.476190476190476f, x+w*0.5f, y+h*0.476190476190476f, col);
		RenderLine_S(x+w*0.0714285714285714f, y+h*0.904761904761905f, x+w*0.857142857142857f, y+h*0.904761904761905f, col);
		break;
	case 'F':
	case 'f':
		w=0.666666666666667f*size;
		RenderLine_S(x+w*0.0714285714285714f, y+h*0.0476190476190476f, x+w*0.857142857142857f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.0714285714285714f, y+h*0.0476190476190476f, x+w*0.0714285714285714f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.0714285714285714f, y+h*0.476190476190476f, x+w*0.5f, y+h*0.476190476190476f, col);
		break;
	case 'G':
	case 'g':
		w=0.714285714285714f*size;
		RenderLine_S(x+w*0.866666666666667f, y+h*0.0476190476190476f, x+w*0.266666666666667f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.266666666666667f, y+h*0.0476190476190476f, x+w*0.0666666666666667f, y+h*0.19047619047619f, col);
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.19047619047619f, x+w*0.0666666666666667f, y+h*0.761904761904762f, col);
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.761904761904762f, x+w*0.266666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.266666666666667f, y+h*0.904761904761905f, x+w*0.8f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.8f, y+h*0.904761904761905f, x+w*0.866666666666667f, y+h*0.857142857142857f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.857142857142857f, x+w*0.866666666666667f, y+h*0.476190476190476f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.476190476190476f, x+w*0.666666666666667f, y+h*0.476190476190476f, col);
		break;
	case 'H':
	case 'h':
		w=0.714285714285714f*size;
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.0476190476190476f, x+w*0.0666666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.476190476190476f, x+w*0.866666666666667f, y+h*0.476190476190476f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.0476190476190476f, x+w*0.866666666666667f, y+h*0.904761904761905f, col);
		break;
	case 'I':
	case 'i':
		w=0.428571428571429f*size;
		RenderLine_S(x+w*0.111111111111111f, y+h*0.0476190476190476f, x+w*0.777777777777778f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.444444444444444f, y+h*0.0476190476190476f, x+w*0.444444444444444f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.111111111111111f, y+h*0.904761904761905f, x+w*0.777777777777778f, y+h*0.904761904761905f, col);
		break;
	case 'J':
	case 'j':
		w=0.714285714285714f*size;
		RenderLine_S(x+w*0.866666666666667f, y+h*0.0476190476190476f, x+w*0.866666666666667f, y+h*0.761904761904762f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.761904761904762f, x+w*0.666666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.666666666666667f, y+h*0.904761904761905f, x+w*0.266666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.266666666666667f, y+h*0.904761904761905f, x+w*0.0666666666666667f, y+h*0.761904761904762f, col);
		break;
	case 'K':
	case 'k':
		w=0.714285714285714f*size;
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.0476190476190476f, x+w*0.0666666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.476190476190476f, x+w*0.266666666666667f, y+h*0.476190476190476f, col);
		RenderLine_S(x+w*0.266666666666667f, y+h*0.476190476190476f, x+w*0.866666666666667f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.266666666666667f, y+h*0.476190476190476f, x+w*0.866666666666667f, y+h*0.904761904761905f, col);
		break;
	case 'L':
	case 'l':
		w=0.714285714285714f*size;
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.0476190476190476f, x+w*0.0666666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.904761904761905f, x+w*0.866666666666667f, y+h*0.904761904761905f, col);
		break;
	case 'M':
	case 'm':
		w=0.714285714285714f*size;
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.0476190476190476f, x+w*0.0666666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.0476190476190476f, x+w*0.466666666666667f, y+h*0.571428571428571f, col);
		RenderLine_S(x+w*0.466666666666667f, y+h*0.571428571428571f, x+w*0.866666666666667f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.0476190476190476f, x+w*0.866666666666667f, y+h*0.904761904761905f, col);
		break;
	case 'N':
	case 'n':
		w=0.714285714285714f*size;
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.0476190476190476f, x+w*0.0666666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.0476190476190476f, x+w*0.866666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.904761904761905f, x+w*0.866666666666667f, y+h*0.0476190476190476f, col);
		break;
	case 'O':
	case 'o':
		w=0.714285714285714f*size;
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.0952380952380952f, x+w*0.133333333333333f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.133333333333333f, y+h*0.0476190476190476f, x+w*0.8f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.8f, y+h*0.0476190476190476f, x+w*0.866666666666667f, y+h*0.0952380952380952f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.0952380952380952f, x+w*0.866666666666667f, y+h*0.857142857142857f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.857142857142857f, x+w*0.8f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.8f, y+h*0.904761904761905f, x+w*0.133333333333333f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.133333333333333f, y+h*0.904761904761905f, x+w*0.0666666666666667f, y+h*0.857142857142857f, col);
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.857142857142857f, x+w*0.0666666666666667f, y+h*0.0952380952380952f, col);
		break;
	case 'P':
	case 'p':
		w=0.666666666666667f*size;
		RenderLine_S(x+w*0.0714285714285714f, y+h*0.0476190476190476f, x+w*0.0714285714285714f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.0714285714285714f, y+h*0.0476190476190476f, x+w*0.714285714285714f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.714285714285714f, y+h*0.0476190476190476f, x+w*0.857142857142857f, y+h*0.142857142857143f, col);
		RenderLine_S(x+w*0.857142857142857f, y+h*0.142857142857143f, x+w*0.857142857142857f, y+h*0.380952380952381f, col);
		RenderLine_S(x+w*0.857142857142857f, y+h*0.380952380952381f, x+w*0.714285714285714f, y+h*0.476190476190476f, col);
		RenderLine_S(x+w*0.714285714285714f, y+h*0.476190476190476f, x+w*0.0714285714285714f, y+h*0.476190476190476f, col);
		break;
	case 'Q':
	case 'q':
		w=0.714285714285714f*size;
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.19047619047619f, x+w*0.0666666666666667f, y+h*0.761904761904762f, col);
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.761904761904762f, x+w*0.266666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.266666666666667f, y+h*0.904761904761905f, x+w*0.466666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.466666666666667f, y+h*0.904761904761905f, x+w*0.866666666666667f, y+h*0.619047619047619f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.619047619047619f, x+w*0.866666666666667f, y+h*0.19047619047619f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.19047619047619f, x+w*0.666666666666667f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.666666666666667f, y+h*0.0476190476190476f, x+w*0.266666666666667f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.266666666666667f, y+h*0.0476190476190476f, x+w*0.0666666666666667f, y+h*0.19047619047619f, col);
		RenderLine_S(x+w*0.466666666666667f, y+h*0.619047619047619f, x+w*0.866666666666667f, y+h*0.904761904761905f, col);
		break;
	case 'R':
	case 'r':
		w=0.714285714285714f*size;
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.0476190476190476f, x+w*0.0666666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.0476190476190476f, x+w*0.666666666666667f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.666666666666667f, y+h*0.0476190476190476f, x+w*0.866666666666667f, y+h*0.19047619047619f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.19047619047619f, x+w*0.866666666666667f, y+h*0.333333333333333f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.333333333333333f, x+w*0.666666666666667f, y+h*0.476190476190476f, col);
		RenderLine_S(x+w*0.666666666666667f, y+h*0.476190476190476f, x+w*0.0666666666666667f, y+h*0.476190476190476f, col);
		RenderLine_S(x+w*0.266666666666667f, y+h*0.476190476190476f, x+w*0.866666666666667f, y+h*0.904761904761905f, col);
		break;
	case 'S':
	case 's':
		w=0.714285714285714f*size;
		RenderLine_S(x+w*0.866666666666667f, y+h*0.19047619047619f, x+w*0.666666666666667f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.666666666666667f, y+h*0.0476190476190476f, x+w*0.266666666666667f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.266666666666667f, y+h*0.0476190476190476f, x+w*0.0666666666666667f, y+h*0.19047619047619f, col);
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.19047619047619f, x+w*0.866666666666667f, y+h*0.761904761904762f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.761904761904762f, x+w*0.666666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.666666666666667f, y+h*0.904761904761905f, x+w*0.266666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.266666666666667f, y+h*0.904761904761905f, x+w*0.0666666666666667f, y+h*0.761904761904762f, col);
		break;
	case 'T':
	case 't':
		w=0.714285714285714f*size;
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.0476190476190476f, x+w*0.866666666666667f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.466666666666667f, y+h*0.0476190476190476f, x+w*0.466666666666667f, y+h*0.904761904761905f, col);
		break;
	case 'U':
	case 'u':
		w=0.714285714285714f*size;
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.0476190476190476f, x+w*0.0666666666666667f, y+h*0.761904761904762f, col);
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.761904761904762f, x+w*0.266666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.266666666666667f, y+h*0.904761904761905f, x+w*0.666666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.666666666666667f, y+h*0.904761904761905f, x+w*0.866666666666667f, y+h*0.761904761904762f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.761904761904762f, x+w*0.866666666666667f, y+h*0.0476190476190476f, col);
		break;
	case 'V':
	case 'v':
		w=0.904761904761905f*size;
		RenderLine_S(x+w*0.0526315789473684f, y+h*0.0476190476190476f, x+w*0.473684210526316f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.473684210526316f, y+h*0.904761904761905f, x+w*0.894736842105263f, y+h*0.0476190476190476f, col);
		break;
	case 'W':
	case 'w':
		w=size;
		RenderLine_S(x+w*0.0476190476190476f, y+h*0.0476190476190476f, x+w*0.333333333333333f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.333333333333333f, y+h*0.904761904761905f, x+w*0.476190476190476f, y+h*0.476190476190476f, col);
		RenderLine_S(x+w*0.476190476190476f, y+h*0.476190476190476f, x+w*0.619047619047619f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.619047619047619f, y+h*0.904761904761905f, x+w*0.904761904761905f, y+h*0.0476190476190476f, col);
		break;
	case 'X':
	case 'x':
		w=0.714285714285714f*size;
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.0476190476190476f, x+w*0.866666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.0476190476190476f, x+w*0.0666666666666667f, y+h*0.904761904761905f, col);
		break;
	case 'Y':
	case 'y':
		w=0.714285714285714f*size;
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.0476190476190476f, x+w*0.466666666666667f, y+h*0.428571428571429f, col);
		RenderLine_S(x+w*0.466666666666667f, y+h*0.428571428571429f, x+w*0.866666666666667f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.466666666666667f, y+h*0.428571428571429f, x+w*0.466666666666667f, y+h*0.904761904761905f, col);
		break;
	case 'Z':
	case 'z':
		w=0.714285714285714f*size;
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.0476190476190476f, x+w*0.866666666666667f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.0476190476190476f, x+w*0.0666666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.904761904761905f, x+w*0.866666666666667f, y+h*0.904761904761905f, col);
		break;
	case '0':
		w=0.619047619047619f*size;
		RenderLine_S(x+w*0.0769230769230769f, y+h*0.19047619047619f, x+w*0.307692307692308f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.307692307692308f, y+h*0.0476190476190476f, x+w*0.615384615384615f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.615384615384615f, y+h*0.0476190476190476f, x+w*0.846153846153846f, y+h*0.19047619047619f, col);
		RenderLine_S(x+w*0.846153846153846f, y+h*0.19047619047619f, x+w*0.846153846153846f, y+h*0.761904761904762f, col);
		RenderLine_S(x+w*0.846153846153846f, y+h*0.761904761904762f, x+w*0.615384615384615f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.615384615384615f, y+h*0.904761904761905f, x+w*0.307692307692308f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.307692307692308f, y+h*0.904761904761905f, x+w*0.0769230769230769f, y+h*0.761904761904762f, col);
		RenderLine_S(x+w*0.0769230769230769f, y+h*0.761904761904762f, x+w*0.0769230769230769f, y+h*0.19047619047619f, col);
		break;
	case '1':
		w=0.428571428571429f*size;
		RenderLine_S(x+w*0.111111111111111f, y+h*0.19047619047619f, x+w*0.444444444444444f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.444444444444444f, y+h*0.0476190476190476f, x+w*0.444444444444444f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.111111111111111f, y+h*0.904761904761905f, x+w*0.777777777777778f, y+h*0.904761904761905f, col);
		break;
	case '2':
		w=0.714285714285714f*size;
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.19047619047619f, x+w*0.266666666666667f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.266666666666667f, y+h*0.0476190476190476f, x+w*0.666666666666667f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.666666666666667f, y+h*0.0476190476190476f, x+w*0.866666666666667f, y+h*0.19047619047619f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.19047619047619f, x+w*0.866666666666667f, y+h*0.333333333333333f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.333333333333333f, x+w*0.666666666666667f, y+h*0.476190476190476f, col);
		RenderLine_S(x+w*0.666666666666667f, y+h*0.476190476190476f, x+w*0.266666666666667f, y+h*0.476190476190476f, col);
		RenderLine_S(x+w*0.266666666666667f, y+h*0.476190476190476f, x+w*0.0666666666666667f, y+h*0.619047619047619f, col);
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.619047619047619f, x+w*0.0666666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.904761904761905f, x+w*0.866666666666667f, y+h*0.904761904761905f, col);
		break;
	case '3':
		w=0.714285714285714f*size;
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.19047619047619f, x+w*0.266666666666667f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.266666666666667f, y+h*0.0476190476190476f, x+w*0.666666666666667f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.666666666666667f, y+h*0.0476190476190476f, x+w*0.866666666666667f, y+h*0.19047619047619f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.19047619047619f, x+w*0.866666666666667f, y+h*0.333333333333333f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.333333333333333f, x+w*0.666666666666667f, y+h*0.476190476190476f, col);
		RenderLine_S(x+w*0.666666666666667f, y+h*0.476190476190476f, x+w*0.466666666666667f, y+h*0.476190476190476f, col);
		RenderLine_S(x+w*0.666666666666667f, y+h*0.476190476190476f, x+w*0.866666666666667f, y+h*0.619047619047619f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.619047619047619f, x+w*0.866666666666667f, y+h*0.761904761904762f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.761904761904762f, x+w*0.666666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.666666666666667f, y+h*0.904761904761905f, x+w*0.266666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.266666666666667f, y+h*0.904761904761905f, x+w*0.0666666666666667f, y+h*0.904761904761905f, col);
		break;
	case '4':
		w=0.714285714285714f*size;
		RenderLine_S(x+w*0.666666666666667f, y+h*0.0476190476190476f, x+w*0.0666666666666667f, y+h*0.619047619047619f, col);
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.619047619047619f, x+w*0.866666666666667f, y+h*0.619047619047619f, col);
		RenderLine_S(x+w*0.666666666666667f, y+h*0.0476190476190476f, x+w*0.666666666666667f, y+h*0.904761904761905f, col);
		break;
	case '5':
		w=0.714285714285714f*size;
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.0476190476190476f, x+w*0.866666666666667f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.0476190476190476f, x+w*0.0666666666666667f, y+h*0.333333333333333f, col);
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.333333333333333f, x+w*0.666666666666667f, y+h*0.333333333333333f, col);
		RenderLine_S(x+w*0.666666666666667f, y+h*0.333333333333333f, x+w*0.866666666666667f, y+h*0.476190476190476f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.476190476190476f, x+w*0.866666666666667f, y+h*0.761904761904762f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.761904761904762f, x+w*0.666666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.666666666666667f, y+h*0.904761904761905f, x+w*0.266666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.266666666666667f, y+h*0.904761904761905f, x+w*0.0666666666666667f, y+h*0.761904761904762f, col);
		break;
	case '6':
		w=0.714285714285714f*size;
		RenderLine_S(x+w*0.666666666666667f, y+h*0.0476190476190476f, x+w*0.466666666666667f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.466666666666667f, y+h*0.0476190476190476f, x+w*0.0666666666666667f, y+h*0.333333333333333f, col);
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.333333333333333f, x+w*0.0666666666666667f, y+h*0.761904761904762f, col);
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.761904761904762f, x+w*0.266666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.266666666666667f, y+h*0.904761904761905f, x+w*0.666666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.666666666666667f, y+h*0.904761904761905f, x+w*0.866666666666667f, y+h*0.761904761904762f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.761904761904762f, x+w*0.866666666666667f, y+h*0.619047619047619f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.619047619047619f, x+w*0.666666666666667f, y+h*0.476190476190476f, col);
		RenderLine_S(x+w*0.666666666666667f, y+h*0.476190476190476f, x+w*0.0666666666666667f, y+h*0.476190476190476f, col);
		break;
	case '7':
		w=0.714285714285714f*size;
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.0476190476190476f, x+w*0.866666666666667f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.0476190476190476f, x+w*0.266666666666667f, y+h*0.904761904761905f, col);
		break;
	case '8':
		w=0.714285714285714f*size;
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.19047619047619f, x+w*0.266666666666667f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.266666666666667f, y+h*0.0476190476190476f, x+w*0.666666666666667f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.666666666666667f, y+h*0.0476190476190476f, x+w*0.866666666666667f, y+h*0.19047619047619f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.19047619047619f, x+w*0.866666666666667f, y+h*0.333333333333333f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.333333333333333f, x+w*0.666666666666667f, y+h*0.476190476190476f, col);
		RenderLine_S(x+w*0.666666666666667f, y+h*0.476190476190476f, x+w*0.866666666666667f, y+h*0.619047619047619f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.619047619047619f, x+w*0.866666666666667f, y+h*0.761904761904762f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.761904761904762f, x+w*0.666666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.666666666666667f, y+h*0.904761904761905f, x+w*0.266666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.266666666666667f, y+h*0.904761904761905f, x+w*0.0666666666666667f, y+h*0.761904761904762f, col);
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.761904761904762f, x+w*0.0666666666666667f, y+h*0.619047619047619f, col);
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.619047619047619f, x+w*0.266666666666667f, y+h*0.476190476190476f, col);
		RenderLine_S(x+w*0.266666666666667f, y+h*0.476190476190476f, x+w*0.0666666666666667f, y+h*0.333333333333333f, col);
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.333333333333333f, x+w*0.0666666666666667f, y+h*0.19047619047619f, col);
		RenderLine_S(x+w*0.266666666666667f, y+h*0.476190476190476f, x+w*0.666666666666667f, y+h*0.476190476190476f, col);
		break;
	case '9':
		w=0.714285714285714f*size;
		RenderLine_S(x+w*0.866666666666667f, y+h*0.476190476190476f, x+w*0.266666666666667f, y+h*0.476190476190476f, col);
		RenderLine_S(x+w*0.266666666666667f, y+h*0.476190476190476f, x+w*0.0666666666666667f, y+h*0.333333333333333f, col);
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.333333333333333f, x+w*0.0666666666666667f, y+h*0.19047619047619f, col);
		RenderLine_S(x+w*0.0666666666666667f, y+h*0.19047619047619f, x+w*0.266666666666667f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.266666666666667f, y+h*0.0476190476190476f, x+w*0.666666666666667f, y+h*0.0476190476190476f, col);
		RenderLine_S(x+w*0.666666666666667f, y+h*0.0476190476190476f, x+w*0.866666666666667f, y+h*0.19047619047619f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.19047619047619f, x+w*0.866666666666667f, y+h*0.619047619047619f, col);
		RenderLine_S(x+w*0.866666666666667f, y+h*0.619047619047619f, x+w*0.466666666666667f, y+h*0.904761904761905f, col);
		RenderLine_S(x+w*0.466666666666667f, y+h*0.904761904761905f, x+w*0.266666666666667f, y+h*0.904761904761905f, col);
		break;
	}
	return w;
}

void RenderHelper::RenderString( const char * str, float x, float y, float size, DWORD col )
{
	if (!str)
	{
		return;
	}
	int nLength = strlen(str);
	if (!nLength)
	{
		return;
	}
	float xb = x;
	for (int i=0; i<nLength; i++)
	{
		xb += pguic->StoCs(RenderCharacter(str[i], xb, y, size, col));
	}
}
