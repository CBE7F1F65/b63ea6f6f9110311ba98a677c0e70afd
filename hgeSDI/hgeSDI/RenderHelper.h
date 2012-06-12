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

	void BaseRenderPoint_S(float x, float y, DWORD col=0);
	void RenderPoint_S(float x, float y, DWORD col=0);
	void RenderPoint(float x, float y, DWORD col=0)
	{
		RenderPoint_S(_CTOSP(x, y), col);
	};

	void BaseRenderLine_S(float x1, float y1, float x2, float y2, DWORD col=0);

	void RenderLine_S(float x1, float y1, float x2, float y2, DWORD col=0);
	void RenderLineR_S(float x, float y, float length, DWORD col=0);
	void RenderLineB_S(float x, float y, float length, DWORD col=0);
	void RenderSquare_S(float x, float y, float a, DWORD col=0);
	void RenderRect_S(float x, float y, float a, float b, DWORD col=0);

	void RenderLine(float x1, float y1, float x2, float y2, DWORD col=0)
	{
		RenderLine_S(_CTOSP(x1, y1), _CTOSP(x2, y2), col);
	};
	void RenderLineR(float x, float y, float length, DWORD col=0)
	{
		RenderLineR_S(_CTOSP(x, y), length, col);
	};
	void RenderLineB(float x, float y, float length, DWORD col=0)
	{
		RenderLineB_S(_CTOSP(x, y), length, col);
	};
	void RenderSquare(float x, float y, float a, DWORD col=0)
	{
		RenderSquare_S(_CTOSP(x, y), a, col);
	};
	void RenderRect(float x, float y, float a, float b, DWORD col=0)
	{
		RenderRect_S(_CTOSP(x, y), a, b, col);
	};

	void RenderArrow_S(float x, float y, int angle, float length, float arrowsize, DWORD col=0);
	void RenderArrowR_S(float x, float y, float length, float arrowsize, DWORD col=0);
	void RenderArrowB_S(float x, float y, float length, float arrowsize, DWORD col=0);
	void RenderArrowL_S(float x, float y, float length, float arrowsize, DWORD col=0);
	void RenderArrowT_S(float x, float y, float length, float arrowsize, DWORD col=0);

	void RenderBezier(PointF2D p1, PointF2D p2, PointF2D p3, PointF2D p4, DWORD col=0, float precisemul=1.0f);

#undef _CTOSP

	void TargetQuadRender_S(HTARGET tar, float x, float y, DWORD col=0);

	void SetLineStyle(int style=0);
	int style;

	HGE * hge;
};