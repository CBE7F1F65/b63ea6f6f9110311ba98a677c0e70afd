#pragma once

#include "MainDependency.h"
#include "GUICoordinate.h"
#include "MathHelper.h"

enum{
	RHLINESTYLE_LINE=0,
	RHLINESTYLE_DOTTEDLINE,
	RHLINESTYLE_SLASHLINE,
};

class RenderHelper
{
public:
	static RenderHelper& getInstance() { static RenderHelper instance; return instance; }

private:
	RenderHelper();
	~RenderHelper();
	RenderHelper(RenderHelper const&);
	void operator=(RenderHelper const&);

public:

	GUICoordinate * pguic;

	void Release();

	void BeginRenderTar(HTARGET tar, DWORD col=0);
	void EndRenderTar();

#define _CTOSP(X, Y)	pguic->CtoSx(X), pguic->CtoSy(Y)
#define _CTOSS(S)		pguic->CtoSs(S)

	void BaseRenderPoint_S(float x, float y, DWORD col);
	void RenderPoint_S(float x, float y, DWORD col);
	void RenderPoint(float x, float y, DWORD col)
	{
		RenderPoint_S(_CTOSP(x, y), col);
	};

	void BaseRenderLine_S(float x1, float y1, float x2, float y2, DWORD col);

	void RenderLine_S(float x1, float y1, float x2, float y2, DWORD col);
	void RenderLineR_S(float x, float y, float length, DWORD col);
	void RenderLineB_S(float x, float y, float length, DWORD col);
	void RenderSquare_S(float x, float y, float a, DWORD col);
	void RenderRect_S(float x, float y, float w, float h, DWORD col);
	void RenderArc_S(float x, float y, float r, int beginangle, int endangle, DWORD col);
	void RenderCircle_S(float x, float y, float r, DWORD col);

	void RenderLine(float x1, float y1, float x2, float y2, DWORD col)
	{
		RenderLine_S(_CTOSP(x1, y1), _CTOSP(x2, y2), col);
	};
	void RenderLineR(float x, float y, float length, DWORD col)
	{
		RenderLineR_S(_CTOSP(x, y), _CTOSS(length), col);
	};
	void RenderLineB(float x, float y, float length, DWORD col)
	{
		RenderLineB_S(_CTOSP(x, y), _CTOSS(length), col);
	};
	void RenderSquare(float x, float y, float a, DWORD col)
	{
		RenderSquare_S(_CTOSP(x, y), _CTOSS(a), col);
	};
	void RenderRect(float x, float y, float w, float h, DWORD col)
	{
		RenderRect_S(_CTOSP(x, y), _CTOSS(w), _CTOSS(h), col);
	};
	void RenderArc( float x, float y, float r, int beginangle, int endangle, DWORD col )
	{
		RenderArc_S(_CTOSP(x, y), _CTOSS(r), beginangle, endangle, col);
	};
	void RenderCircle(float x, float y, float r, DWORD col)
	{
		RenderCircle_S(_CTOSP(x, y), _CTOSS(r), col);
	};

	void RenderArrow_S(float x, float y, int angle, float length, float arrowsize, DWORD col);
	void RenderArrowR_S(float x, float y, float length, float arrowsize, DWORD col);
	void RenderArrowB_S(float x, float y, float length, float arrowsize, DWORD col);
	void RenderArrowL_S(float x, float y, float length, float arrowsize, DWORD col);
	void RenderArrowT_S(float x, float y, float length, float arrowsize, DWORD col);

//	void RenderBezier(PointF2D pb, PointF2D pbh, PointF2D peh, PointF2D pe, DWORD col, float precisemul=1.0f);
	void RenderBezierByInfo(BezierSublinesInfo * bsinfo, DWORD col);

	void RenderAttributePoint_S(float x, float y, DWORD col);
	void RenderSubstantivePoint_S(float x, float y, DWORD col);
	void RenderHandlePoint_S(float x, float y, DWORD col);
	void RenderAttributePoint(float x, float y, DWORD col)
	{
		RenderAttributePoint_S(_CTOSP(x, y), col);
	};
	void RenderSubstantivePoint(float x, float y, DWORD col)
	{
		RenderSubstantivePoint_S(_CTOSP(x, y), col);
	};
	void RenderHandlePoint( float x, float y, DWORD col )
	{
		RenderHandlePoint_S(_CTOSP(x, y), col);
	}

#undef _CTOSP

	void TargetQuadRender_S(HTARGET tar, float x, float y, DWORD col);

	void SetLineStyle(int style=0);
	void RenderLineMeasureMark( float x1, float y1, float x2, float y2, DWORD col );
	int style;

	HGE * hge;
};