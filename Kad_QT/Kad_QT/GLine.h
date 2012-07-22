#pragma once
#include "GObject.h"

#include "GPoint.h"
/************************************************************************/
/* GLINE                                                                */
/************************************************************************/
class GLine :
	public GObject
{
public:
	typedef GObject super;
public:
	GLine(void);
	virtual ~GLine(void);

	virtual bool CloneData(GObject * pClone, GObject * pNewParent, bool bNoRelationship=true);

	virtual float getX() { if (pmid) { return pmid->getX(); } return 0; };
	virtual float getY() { if (pmid) { return pmid->getY(); } return 0; };

	virtual bool isLine(){return true;};

	virtual GPiece * getPiece();

	virtual bool MoveTo(float newx, float newy, bool bTry, int moveActionID=-1);
	virtual bool CallMoveTo(float newx, float newy, bool bTry, int moveActionID=-1);

	virtual bool isStraightLine() = 0;
	virtual bool toStraightLine();
	virtual bool toBezierLine() = 0;

	virtual void SetBeginEnd(float xb, float yb, float xe, float ye, float fAllowance=-1);
	virtual void UpdateMidPoint() = 0;
	virtual bool CheckNearTo(float px, float py, float r, float *plx, float *ply, int *isec=NULL) = 0;
	virtual bool CheckIntersectWithRect(float xl, float yt, float xr, float yb) = 0;
	virtual bool CheckIntersectWithLineObj( GLine * pLine, list<PointF2D> *pPoints );
	virtual float CalculateProportion( float x, float y, int iSec ) = 0;
	virtual float CalculateMidPointProportion() = 0;
	virtual bool GetPositionAtProportion( float fClingToProportion, float* tox, float* toy, int*isec=NULL ) = 0;

	virtual bool isLengthCalculated(){return true;};
	virtual float getLength() = 0;

	virtual PointF2D GetTangentPointF2D(float t) = 0;

	virtual const char * getDisplayName();

	virtual void OnModify();

	inline GAnchorPoint * GetBeginPoint(){return plbegin;};
	inline GAnchorPoint * GetEndPoint(){return plend;};
	inline GMidPoint * GetMidPoint(){return pmid;};

	virtual void OnRemove();

	bool AddClingBy(GPoint * pPoint);
	void DeclingByOther(GPoint * pPoint=NULL);
	bool isClingBy(GPoint * pPoint);
	list<GPoint *> * getClingBy(){return &clingByList;};

	/************************************************************************/
	/* Members                                                              */
	/************************************************************************/
protected:
	//////////////////////////////////////////////////////////////////////////
	GOBJM_CHILDPOINTERS();
	GAnchorPoint * plbegin;
	GAnchorPoint * plend;
	GMidPoint * pmid;
	GOBJM_CHILDPOINTERSEND();

	//////////////////////////////////////////////////////////////////////////
	GOBJM_DONOTCOPY();
	list<GPoint *> clingByList;
	GOBJM_DONOTCOPYEND();
};
/************************************************************************/
/* GSubstantiveLine                                                     */
/************************************************************************/
class GSubstantiveLine :
	public GLine
{
public:
	typedef GLine super;
public:
	GSubstantiveLine(){};
	virtual ~GSubstantiveLine(){};

	virtual bool isRepresentableLine(){return true;};
	virtual bool canBeClingTo(){return true;};
};
/************************************************************************/
/* GVirturalLine                                                        */
/************************************************************************/
class GVirtualLine : 
	public GLine
{
public:
	typedef GLine super;
public:
	GVirtualLine(){};
	virtual ~GVirtualLine(){};
};
/************************************************************************/
/* GStrightLine                                                         */
/************************************************************************/
// Still Virtual!
class GStraightLine : public GSubstantiveLine
{
public:
	typedef GSubstantiveLine super;
public:
	GStraightLine();
	virtual ~GStraightLine();

	virtual void UpdateMidPoint();

	virtual bool canMove(){return true;};
	virtual bool isStraightLine(){return true;};

	virtual float CalculateProportion( float x, float y, int iSec );
	virtual float CalculateMidPointProportion();
	virtual bool GetPositionAtProportion( float fClingToProportion, float* tox, float* toy, int*isec=NULL );

	virtual void OnRender(int iHighlightLevel=0);

	virtual bool CheckNearTo(float px, float py, float r, float *plx, float *ply, int *isec=NULL);
	virtual bool CheckIntersectWithRect(float xl, float yt, float xr, float yb);
	virtual bool CheckIntersectStraightStraight(GStraightLine * pLine, list<PointF2D> *pPoints);
	virtual void GetBoundingBox(float *xl, float *yt, float *xr, float * yb);

	virtual float getLength();

	virtual PointF2D GetTangentPointF2D(float t);
};
/************************************************************************/
/* GBezierLine                                                          */
/************************************************************************/
class GBezierLine : public GStraightLine
{
public:
	typedef GStraightLine super;
public:
	GBezierLine();
	GBezierLine(GObject * parent, PointF2D pb, PointF2D pe);
	GBezierLine(GObject * parent, PointF2D pb, PointF2D pbh, PointF2D peh, PointF2D pe);
	GBezierLine(GObject * parent, float cx, float cy, float r, int quadrant);
	virtual ~GBezierLine();

	virtual GObject * CreateNewClone(GObject * pNewParent=NULL, GObject * pBeforeObj=NULL);
	virtual bool CloneData(GObject * pClone, GObject * pNewParent, bool bNoRelationship=true);

    virtual void OnPrecisionChanged(float fPrecision);
	virtual void OnModify();
	virtual void UpdateMidPoint();
	virtual bool canMove(){return true;};
	virtual bool isStraightLine();
	virtual bool toBezierLine();

	virtual void OnRender(int iHighlightLevel=0);

	virtual void SetBeginEnd(float xb, float yb, float xe, float ye, float fAllowance=-1);
	virtual void SetPosByQuarterCircle(float cx, float cy, float r, int quadrant);

	virtual void SetBeginHandlePos(float x, float y, float fAllowance=-1);
	virtual void SetEndHandlePos(float x, float y, float fAllowance=-1);

	virtual bool CheckNearTo(float px, float py, float r, float *plx, float *ply, int * isec=NULL);
	virtual bool CheckIntersectWithRect(float xl, float yt, float xr, float yb);
	virtual bool CheckIntersectBezierStraight(GStraightLine * pLine, list<PointF2D> *pPoints);
	virtual bool CheckIntersectBezierBezier(GBezierLine * pLine, list<PointF2D> *pPoints);
	virtual void GetBoundingBox(float *xl, float *yt, float *xr, float * yb);

	virtual float CalculateProportion( float x, float y, int iSec );
	virtual float CalculateMidPointProportion();
	virtual bool GetPositionAtProportion( float fClingToProportion, float* tox, float* toy, int*isec=NULL );

	virtual bool isLengthCalculated();
	virtual float getLength();

	virtual PointF2D GetTangentPointF2D(float t);


	BezierSublinesInfo * getBSInfo(){return &bsinfo;};
	/************************************************************************/
	/* Members                                                              */
	/************************************************************************/
protected:
	//////////////////////////////////////////////////////////////////////////
	GOBJM_NONCOPYABLES();
	BezierSublinesInfo bsinfo;
	GOBJM_NONCOPYABLESEND();
};
