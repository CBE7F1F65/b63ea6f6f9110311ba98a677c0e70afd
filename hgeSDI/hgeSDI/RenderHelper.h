#pragma once

#include "MainDependency.h"

class RenderHelper
{
public:
	RenderHelper(void);
	~RenderHelper(void);

	static void RenderLine(float x1, float y1, float x2, float y2, DWORD col=0);
	static void RenderLineR(float x, float y, float length, DWORD col=0);
	static void RenderLineB(float x, float y, float length, DWORD col=0);
	static void RenderSquare(float x, float y, float a, DWORD col=0);
	static void RenderRect(float x, float y, float a, float b, DWORD col=0);

	static void RenderArrow(float x, float y, int angle, float length, float arrowsize, DWORD col=0);
	static void RenderArrowR(float x, float y, float length, float arrowsize, DWORD col=0);
	static void RenderArrowB(float x, float y, float length, float arrowsize, DWORD col=0);
	static void RenderArrowL(float x, float y, float length, float arrowsize, DWORD col=0);
	static void RenderArrowT(float x, float y, float length, float arrowsize, DWORD col=0);

	static void TargetQuadRender(HTARGET tar, float x, float y, DWORD col=0);

	static HGE * hge;
};

