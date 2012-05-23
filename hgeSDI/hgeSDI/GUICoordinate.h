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

	void SetCursorPosition(float x_s, float y_s);

	int DoPanCommand();
	void DoPan(float xoffset_s, float yoffset_s);

	int DoZoomCommand();
	void DoZoom(float cx_s, float cy_s, float scale);

	void ClientToCoordinate(float * x, float * y);
	void CoordinateToClient(float * x, float * y);
	void RenderGrid();
	void RenderGridReDraw();
	void RenderCoordinate();
	void DoRenderCoordinate(float atx, float aty);
	void CheckScale()
	{
		if (scale == 0.0f)
			scale = FLT_MIN;
	};

	void UpdateScreenMeasure();

	int measuretype;
	float originx_s;
	float originy_s;
	float scale;

	float lboundary_c;
	float rboundary_c;
	float tboundary_c;
	float bboundary_c;
	float scrw_s;
	float scrh_s;

	float subgridspace_c;
	float subgridspace_s;
	int nsubgrid;

	float cursorx_s;
	float cursory_s;
	float cursorx_c;
	float cursory_c;

	struct GridInfo 
	{
		float scrvalue;
		int mgridID;
	};
	vector<GridInfo> gridxs_s;
	vector<GridInfo> gridys_s;

	bool panwillterminate;

	HTARGET targrid;
	HGE * hge;
};
