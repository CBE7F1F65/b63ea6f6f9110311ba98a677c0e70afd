#pragma once

#include "GLine.h"
#include "GBaseNode.h"
#include "MarkingObject.h"

#define GOPSNAP_NONE			0x0000
#define GOPSNAP_SELF			0x0001
#define GOPSNAP_GRID			0x0002
#define GOPSNAP_GEOMETRY		0x0004
#define GOPSNAP_COORD			0x0008
#define GOPSNAP_GEOMETRYCOORD	0x0010
#define GOPSNAP_VIRTUALCOORD	0x0020
#define GOPSNAP_CONTINUITY		0x0040
#define GOPSNAP_HANDLEONLY		0x0080

#define GOPSNAPPED_OBJ			0x0100
#define GOPSNAPPED_POINT		0x0200
#define GOPSNAPPED_VIRTUALPOINT	0x0400
#define GOPSNAPPED_LINE			0x0800
#define GOPSNAPPED_XAXIS		0x1000
#define GOPSNAPPED_YAXIS		0x2000
#define GOPSNAPPED_CONTINUITY	0x4000

#define GOPSNAPPED_LENGTHLOCK			0x010000
#define GOPSNAPPED_ANGLESLOCK			0x020000
/*
#define GOPSNAPPED_PERPENDICULARLOCK	0x040000	// To Bezier Only (= Multiple Angle Lock)
#define GOPSNAPPED_TANGENTIALLOCK		0x080000	// To Bezier Only (= Multiple Angle Lock)
*/
// Parallel, Perpendicular, Tangential, Axis, Continuity are all ANGLELOCK

#define GOPSNAPINDEX_SHIFT		0x01000000


#define PICKSTATE_NULL				0x00
#define PICKSTATE_REQUIREUPDATE		0x01
#define PICKSTATE_AFTERUPDATE		0x02
#define PICKSTATE_READY				0x03

#define GOPMOUSE_NONE	0x0100
#define GOPMOUSE_DOWN	0x0200
#define GOPMOUSE_UP		0x0300

#define GOPONLINE_MAX	2

typedef bool (*PickFilterCallback)(GObject *);

class PickerInterestPointInfo
{
public:
	PickerInterestPointInfo(){ClearSet();};
	~PickerInterestPointInfo(){};

	void ClearSet()
	{
		x = 0;
		y = 0;
		bHasAngle = false;
		angle = 0;
	};
	void SetPosition(float _x, float _y)
	{
		x = _x;
		y = _y;
	};
	void SetAngle(int _angle)
	{
		angle = _angle;
		bHasAngle = true;
	};
	bool Equals(PickerInterestPointInfo & r);

	float GetX(){return x;};
	float GetY(){return y;};
	int GetAngle(){ASSERT(bHasAngle); return angle;};
	bool HasAngle(){return bHasAngle;};

private:
	float x;
	float y;
	bool bHasAngle;
	int angle;
};

class GObjectPicker
{
public:
	static GObjectPicker& getInstance() { static GObjectPicker instance; return instance; }
	static GObjectPicker& getTestPicker() { static GObjectPicker testpickerinstance; return testpickerinstance; };

private:
	GObjectPicker();
	~GObjectPicker();
	GObjectPicker(GObjectPicker const&);
	void operator=(GObjectPicker const&);

public:

	void Init();
private:
	int state;
	int restrict;

	int mousestate;

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
	void FillInitialPosition();
	bool bFilledPos;
	bool bTestMode;
	float filledX_C;
	float filledY_C;

private:
	GObject * pickObj[GOPONLINE_MAX];
	int pickSection[GOPONLINE_MAX];
	GObject * pickEntityObj[GOPONLINE_MAX];
	GObject * pickCoordEntityObj[GOPONLINE_MAX];
	PickerInterestPointInfo pickPIP[GOPONLINE_MAX];
	int nPickObj;
	int nPickPIP;
//	GObject * mousedownPickObj;
//	GObject * mousedownPickEntityObj;
public:
	GObject * GetPickedObj(int index=0);
	GObject * GetPickedEntityObj(int index=0);
//	GObject * GetMDownPickedObj();
//	GObject * GetMDownPickedEntityObj();
private:
	int snaptoflag;
public:
    void SetSnapTo(int snapto){SetSnapTo(snapto, true);};
    void SetSnapTo(int snapto, bool bSet);
    bool isSnapToGrid(){return snaptoflag&GOPSNAP_GRID;};
    bool isSnapToGeometry(){return snaptoflag&GOPSNAP_GEOMETRY;};
    bool isSnapToCoord(){return snaptoflag&GOPSNAP_COORD;};
    bool isSnapToContinuity(){return snaptoflag&GOPSNAP_CONTINUITY;};
    bool isSnapToGeometryCoord(){return snaptoflag&GOPSNAP_GEOMETRYCOORD;};
    bool isSnapToVirtualCoord(){return snaptoflag&GOPSNAP_VIRTUALCOORD;};
    bool isSnapToHandleOnly(){return snaptoflag&GOPSNAP_HANDLEONLY;};
private:
	float snaprange_c;
	float snaprange_s;
public:
	void SetSnapRange_S(float range_s){snaprange_s=range_s;};
    float GetSnapRange_S(){return snaprange_s;};
private:
	bool IsInSnapRangePoint_C(float x, float y);
	bool IsInSnapRangeAngle_C(float x, float y, int angle, float * plx, float * ply);
	bool IsInSnapRangeXAxis_C(float y);
	bool IsInSnapRangeYAxis_C(float x);

	bool CheckSnapGeometryPoint(GObject * pObj);
	bool CheckSnapPoint(float x, float y, int tostate);
	bool CheckSnapPointCoord(float x, float y, int tostate);
	bool CheckSnapGeometryLine(GObject * pObj);

	bool CheckSnapGrid();
	bool CheckCoord_Obj(GObject * pObj);
	bool CheckSnapContinuity();

public:
	void ClearSet();
	void OnMouseUp();

private:
	PickFilterCallback pfilterfunc;

public:
	int PickPoint(PickFilterCallback pfunc=NULL);
	int UpdatePickPoint();
	int TestPickPoint(float x, float y, float *pfProportion=0, PickFilterCallback pfunc=NULL, float range_s=0);
	void OnDeleteNode( GObject * pDeletedObj );

	bool IsPickReady(int iret=-1);
	bool IsMouseDownReady();

public:
	void PushInterestPoint(float x, float y, bool bHasAngle=false, int angle=0);
	void SetCheckMouseDown(){bCheckMouseDown=true;};
	bool FindLineIntersectX( float y );
	bool FindLineIntersectY( float x );
	bool FindLineIntersectLine( GLine * pLine );
	bool FindLineIntersectPIP( PickerInterestPointInfo * pPIP );
	void SetPickObj( GObject * pObj );
	void SetPickPIP( PickerInterestPointInfo info );
	void SetPickObjCoord(GObject * pObj);
	bool SubFindLineX( GLine * pLine, float y, int iIndex );
	bool SubFindPIPX( PickerInterestPointInfo * pPIP, float y );
	bool SubFindLineY( GLine * pLine, float x, int iIndex );
	bool SubFindPIPY( PickerInterestPointInfo * pPIP, float x );
	bool SubFindLinePIP( GLine * pLine, PickerInterestPointInfo * pPIP, int iIndex );
	bool SubFindLineLine( GLine * pLine1, GLine * pLine2 );
	bool SubFindPIPPIP( PickerInterestPointInfo * pPIP1, PickerInterestPointInfo * pPIP2 );

	bool SubFindLengthLockX( float y );
	bool SubFindAnglesLockX( float y );
	bool SubFindLengthLockY( float x );
	bool SubFindAnglesLockY( float x );
	bool SubFindLineLengthLock( GLine * pLine, int iIndex );
	bool SubFindLineAnglesLock( GLine * pLine, int iIndex );
	bool SubFindPIPLengthLock( PickerInterestPointInfo * pPIP );
	bool SubFindPIPAnglesLock( PickerInterestPointInfo * pPIP );

	void TraslateLineToStraightLine( GLine * pLine, int index, int isec );
	void TraslatePIPToStraightLine( PickerInterestPointInfo * pPIP, int index, float fLength=-1 );
	float CalculateProportion( int index=0 );

	void SetLockOrigin(float x, float y);
	void SetLockLength(float fLength, GHandlePoint * pHandle=NULL, bool bInvert=false);
	void SetLockAngle(int nAngle){SetLockAngles(1, &nAngle);};
	void SetLockAngles(int nLocks, int * pAngles);
	void UnlockLength();
	void UnlockAngles();
	void SetLockXAxis(float y);
	void SetLockYAxis(float x);
	void UnlockXAxis();
	void UnlockYAxis();

	void SetLockSplitLine(int splitlocktype, float fval);
	void ClearSplitMarking();
	void UnlockSplitLine();

//private:
public:
	void SetLockLockLine(GObject * pObj);
public:
	void UnlockLockLine();

	void AdjustPositionToLocks();
	bool IsAngleInBetween( int angle, int nBeginAngle, int nMidAngle, int nNextAngle );

	void AddSplitUI(GObject * pObj);

	bool MIDCBSplit( MarkingUI * pmui, bool bAccept );
	static bool staticMIDCBSplit( MarkingUI * pmui, bool bAccept );

private:
	list<PickerInterestPointInfo> pipinfo;
	bool bCheckMouseDown;
	bool bRenderMouseDown;

	int nOnLine;



	float fLockOriginX_c;
	float fLockOriginY_c;

	float fLockLength;
	bool bLockLength;
	GHandlePoint * pLockLengthHandle;
	GHandlePoint * pLockLengthAnotherHandle;
	bool bLockLengthHandleInvert;

	int * pLockAngles;
	int numLockAngles;
	int nCurrentLockAngleIndex;
	PointF2D ptCurrentLockAngleDir;

	float fLockXAxis_y;
	bool bLockXAxis;
	float fLockYAxis_x;
	bool bLockYAxis;

	GLine * pLockedLine;

	int nSplitLockType;
	float fSplitValue;

	MarkingSplitLine * pSplitMarking;
	bool bSplitMarkingInUse;

	GBezierLine * pFakeLine[GOPONLINE_MAX];
};
