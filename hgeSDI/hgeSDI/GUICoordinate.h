#pragma once

#include "MainDependency.h"

enum {
	GUICG_METRIC,
	GUICG_IMPERIAL
};

class GUICoordinate
{
public:
	GUICoordinate();
	~GUICoordinate();

	static GUICoordinate * getInstance();


	void SetGrid(int measuretype, float originxpos=0, float originypos=0, float scale=1.0f);
	void SetGridColors(DWORD gridcol, DWORD subgridcol, DWORD xaxiscol, DWORD yaxiscol, DWORD coordcol);

	void SetCursorPosition(float x, float y);

	void DoPan(float xoffset, float yoffset);
	void DoScale(float scale);
	void ClientToCoordinate(float * x, float * y);
	void CoordinateToClient(float * x, float * y);
	void RenderGrid();
	void RenderGridReDraw();
	void RenderCoordinate();
	void CheckScale()
	{
		if (scale == 0.0f)
			scale = FLT_MIN;
	};

	void UpdateScreenMeasure();

	int measuretype;
	float scroriginx;
	float scroriginy;
	float scale;

	float scrlmeasure;
	float scrrmeasure;
	float scrtmeasure;
	float scrbmeasure;
	float scrw;
	float scrh;

	float subgridspace;
	float subgridscrspace;
	int nsubgrid;

	DWORD gridcol;
	DWORD subgridcol;
	DWORD xaxiscol;
	DWORD yaxiscol;
	DWORD coordcol;

	float cursorx_s;
	float cursory_s;
	float cursorxcoord;
	float cursorycoord;

	struct GridInfo 
	{
		float scrvalue;
		int mgridID;
	};
	vector<GridInfo> scrgridxs;
	vector<GridInfo> scrgridys;

	HTARGET targrid;
	HGE * hge;
};
