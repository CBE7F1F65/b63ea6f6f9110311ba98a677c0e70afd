#pragma once

#include "MainDependency.h"

enum {
	GUICG_METRIC,
	GUICG_IMPERIAL
};

class GUICoordinate
{
public:
	static GUICoordinate& getInstance()
	{
		static GUICoordinate instance;
		// Guaranteed to be destroyed. Instantiated on first use.
		return instance;
	}
private:
	GUICoordinate();
	~GUICoordinate();
	// Don't forget to declare these two.
	GUICoordinate(GUICoordinate const&);
	// Don't Implement
	void operator=(GUICoordinate const&);
	// Don't implement

public:

	void SetGrid(int measuretype, float originxpos=0, float originypos=0, float scale=1.0f);
	void SetGridColors(DWORD gridcol, DWORD subgridcol, DWORD xaxiscol, DWORD yaxiscol, DWORD coordcol);

	void SetCursorPosition(float x_s, float y_s);

	void OnProcessPanCommand();
	void DoPan(float xoffset_s, float yoffset_s);
	void DoScroll(bool horz, int pos, int range);

	void OnProcessZoomCommand();
	void DoZoom(float cx_s, float cy_s, float scale);


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

	inline float StoCx(float x) 
	{
		CheckScale();
		return (x-originx_s)/scale;
	}
	inline float StoCy(float y) 
	{
		CheckScale();
		return (y-originy_s)/scale;
	}
	inline float CtoSx(float x)
	{
		return x*scale+originx_s;
	}
	inline float CtoSy(float y)
	{
		return y*scale+originy_s;
	}
};
