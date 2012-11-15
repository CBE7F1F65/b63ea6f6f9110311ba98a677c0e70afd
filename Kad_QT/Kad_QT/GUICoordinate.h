#pragma once

#include "MainDependency.h"

enum {
	GUICG_METRIC,
	GUICG_IMPERIAL
};

class GUICoordinate
{
public:
	static GUICoordinate& getInstance() { static GUICoordinate instance; return instance; }
private:
	GUICoordinate();
	~GUICoordinate();
	GUICoordinate(GUICoordinate const&);
	void operator=(GUICoordinate const&);

public:

	void SetGrid(int measuretype, float originxpos=0, float originypos=0, float scale=1.0f);
	void SetGridColors(DWORD gridcol, DWORD subgridcol, DWORD xaxiscol, DWORD yaxiscol, DWORD coordcol);

	void SetCursorPosition(float x_s, float y_s);

	void OnProcessPanCommand();
	void DoPan(float xoffset_s, float yoffset_s);
	void DoScroll(bool horz, int pos, int range);

	void OnProcessZoomCommand(bool bMul);
	void DoZoom(float cx_s, float cy_s, float scale, bool bMul);

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

private:
    bool bShowGrid;
public:
    bool isShowGrid(){return bShowGrid;};
    void SetShowGrid(bool bSet){bShowGrid=bSet;};

private:
	int measuretype;
	float originx_s;
	float originy_s;
	float scale;
public:
	float GetScale(){return scale;};
private:

	float lboundary_c;
	float rboundary_c;
	float tboundary_c;
	float bboundary_c;
	float scrw_s;
	float scrh_s;
public:
	float GetScreenWidth_S(){return scrw_s;};
	float GetScreenHeight_S(){return scrh_s;};
	float GetScreenWidth_C(){return StoCs(scrw_s);};
	float GetScreenHeight_C(){return StoCs(scrh_s);};
private:
	int nsubgrid;
	float cursorx_s;
	float cursory_s;
	float cursorx_c;
	float cursory_c;
private:

	float subgridspace_c;
	float subgridspace_s;
public:
	float GetSubgirdSpace_C(){return subgridspace_c;};
	float GetSubgirdSpace_S(){return subgridspace_s;};
public:
	float GetCursorX_S(){return cursorx_s;};
	float GetCursorY_S(){return cursory_s;};
	float GetCursorX_C(){return cursorx_c;};
	float GetCursorY_C(){return cursory_c;};

	int GetMeasureType(){return measuretype;};

	struct GridInfo
	{
		float scrvalue;
		int mgridID;
	};
	list<GridInfo> gridxs_s;
	list<GridInfo> gridys_s;

	bool panwillterminate;

	HTARGET tarGrid;
	HGE * hge;

	inline float StoCs(float s)
	{
		CheckScale();
		return s/scale;
	};
	inline float CtoSs(float s)
	{
		CheckScale();
		return s*scale;
	};


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

	void EnterPrintMode();
	void ExitPrintMode();

	bool bprint;
	float savedoriginx_s;
	float savedoriginy_s;
	float savedscale;
};
