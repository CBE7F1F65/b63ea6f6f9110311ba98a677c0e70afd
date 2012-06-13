#pragma once

#include "GObject.h"

#define GOPSNAP_NONE		0x0000
#define GOPSNAP_GRID		0x0001
#define GOPSNAP_GEOMETRY	0x0002
#define GOPSNAP_COORD		0x0004
#define GOPSNAP_CONTINUITY	0x0008

#define GOPSNAPPED_OBJ			0x0100
#define GOPSNAPPED_POINT		0x0200
#define GOPSNAPPED_LINE			0x0400
#define GOPSNAPPED_XAXIS		0x1000
#define GOPSNAPPED_YAXIS		0x2000
#define GOPSNAPPED_CONTINUITY	0x4000

class GObjectPicker
{
public:
	static GObjectPicker& getInstance() { static GObjectPicker instance; return instance; }

private:
	GObjectPicker();
	~GObjectPicker();
	GObjectPicker(GObjectPicker const&);
	void operator=(GObjectPicker const&);

public:

	void Init();
	bool isBeginPtSet();
	bool isBeginAngleSet();
	void SetBeginPt(float beginx, float beginy);
	void SetBeginAngle(int beginangle);

	int state;
	int restrict;

	int havebeginstate;
	float beginx;
	float beginy;
	int beginangle;

private:
	float pickx_s;
	float picky_s;
	float pickx_c;
	float picky_c;

	int snappedstate;
public:

	void Render();

	float GetPickX_C(){return pickx_c;};
	float GetPickY_C(){return picky_c;};

	float GetPickX_S();
	float GetPickY_S();
	
	GObject * pickObj;
	GObject * pickEntityObj;

	int snaptoflag;
	void SetSnapTo(int snapto){snaptoflag=snapto;};

	float snaprange_c;
	float snaprange_s;
	void SetSnapRange(float range){snaprange_s=range;};

	bool IsInSnapRangePoint_C(float x, float y);
	bool IsInSnapRangeXAxis_C(float y);
	bool IsInSnapRangeYAxis_C(float x);

	bool CheckSnapGeometryPoint(GObject * pObj);
	bool CheckSnapGeometryLine(GObject * pObj);

	bool CheckSnapGrid();
	bool CheckSnapCoord();
	bool CheckSnapContinuity();


	bool PickPoint(int restrict=0);
	bool UpdatePickPoint();
};