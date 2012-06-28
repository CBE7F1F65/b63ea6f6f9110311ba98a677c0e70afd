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


#define PICKSTATE_NULL				0x00
#define PICKSTATE_REQUIREUPDATE		0x01
#define PICKSTATE_AFTERUPDATE		0x02
#define PICKSTATE_READY				0x03

#define GOPMOUSE_NONE	0x0100
#define GOPMOUSE_DOWN	0x0200
#define GOPMOUSE_UP		0x0300

typedef bool (*PickFilterCallback)(GObject *);

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
private:
	int state;
	int restrict;

	int havebeginstate;
	int mousestate;
	float beginx;
	float beginy;
	int beginangle;

	float pickx_s;
	float picky_s;
	float pickx_c;
	float picky_c;

	float mousedownx_s;
	float mousedowny_s;
	float mousedownx_c;
	float mousedowny_c;

	int snappedstate;
public:
	int GetSnappedState(){return snappedstate;};

	void Render();

	float GetPickX_C(){return pickx_c;};
	float GetPickY_C(){return picky_c;};

	float GetDownX_C(){return mousedownx_c;};
	float GetDownY_C(){return mousedowny_c;};
	float GetDownX_S(){return mousedownx_s;};
	float GetDownY_S(){return mousedowny_s;};

	float GetPickX_S();
	float GetPickY_S();

private:
	GObject * pickObj;
	GObject * pickEntityObj;
	GObject * mousedownPickObj;
	GObject * mousedownPickEntityObj;
public:
	GObject * GetPickedObj();
	GObject * GetPickedEntityObj();
	GObject * GetMDownPickedObj();
	GObject * GetMDownPickedEntityObj();
private:
	int snaptoflag;
public:
    void SetSnapTo(int snapto){SetSnapTo(snapto, true);};
    void SetSnapTo(int snapto, bool bSet);
    bool isSnapToGrid(){return snaptoflag&GOPSNAP_GRID;};
    bool isSnapToGeometry(){return snaptoflag&GOPSNAP_GEOMETRY;};
    bool isSnapToCoord(){return snaptoflag&GOPSNAP_COORD;};
    bool isSnapToContinuity(){return snaptoflag&GOPSNAP_CONTINUITY;};
private:
	float snaprange_c;
	float snaprange_s;
public:
	void SetSnapRange(float range){snaprange_s=range;};
private:
	bool IsInSnapRangePoint_C(float x, float y);
	bool IsInSnapRangeXAxis_C(float y);
	bool IsInSnapRangeYAxis_C(float x);

	bool CheckSnapGeometryPoint(GObject * pObj);
	bool CheckSnapGeometryLine(GObject * pObj);

	bool CheckSnapGrid();
	bool CheckSnapCoord();
	bool CheckSnapContinuity();
public:
	void ClearSet();

private:
	PickFilterCallback pfilterfunc;

public:
	int PickPoint(PickFilterCallback pfunc=NULL);
	int UpdatePickPoint();
	void OnDeleteNode( GObject * node );

	bool IsPickReady(int iret=-1);
	bool IsMouseDownReady();
};
