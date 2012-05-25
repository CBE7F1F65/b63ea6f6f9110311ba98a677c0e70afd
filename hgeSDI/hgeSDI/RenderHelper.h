#pragma once

#include "MainDependency.h"
#include "GUICoordinate.h"
#include "MathHelper.h"

class RenderHelper
{
public:
	RenderHelper(void);
	~RenderHelper(void);

	static GUICoordinate * pguic;

	static void Release();

	static void BeginRenderTar(HTARGET tar, DWORD col=0);
	static void EndRenderTar();

#define _CTOSP(X, Y)	pguic->CtoSx(X), pguic->CtoSy(Y)

	static void RenderLine_S(float x1, float y1, float x2, float y2, DWORD col=0);
	static void RenderLineR_S(float x, float y, float length, DWORD col=0);
	static void RenderLineB_S(float x, float y, float length, DWORD col=0);
	static void RenderSquare_S(float x, float y, float a, DWORD col=0);
	static void RenderRect_S(float x, float y, float a, float b, DWORD col=0);

	static void RenderLine(float x1, float y1, float x2, float y2, DWORD col=0)
	{
		RenderLine_S(_CTOSP(x1, y1), _CTOSP(x2, y2), col);
	};
	static void RenderLineR(float x, float y, float length, DWORD col=0)
	{
		RenderLineR_S(_CTOSP(x, y), length, col);
	};
	static void RenderLineB(float x, float y, float length, DWORD col=0)
	{
		RenderLineB_S(_CTOSP(x, y), length, col);
	};
	static void RenderSquare(float x, float y, float a, DWORD col=0)
	{
		RenderSquare_S(_CTOSP(x, y), a, col);
	};
	static void RenderRect(float x, float y, float a, float b, DWORD col=0)
	{
		RenderRect_S(_CTOSP(x, y), a, b, col);
	};

	static void RenderArrow_S(float x, float y, int angle, float length, float arrowsize, DWORD col=0);
	static void RenderArrowR_S(float x, float y, float length, float arrowsize, DWORD col=0);
	static void RenderArrowB_S(float x, float y, float length, float arrowsize, DWORD col=0);
	static void RenderArrowL_S(float x, float y, float length, float arrowsize, DWORD col=0);
	static void RenderArrowT_S(float x, float y, float length, float arrowsize, DWORD col=0);

	static void RenderBezier(PointF2D p1, PointF2D p2, PointF2D p3, PointF2D p4, DWORD col=0, float precisemul=1.0f);;

#undef _CTOSP

	static void TargetQuadRender_S(HTARGET tar, float x, float y, DWORD col=0);

	static HGE * hge;
};

