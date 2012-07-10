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
	GLine(void);
	virtual ~GLine(void);

	virtual float getX() { if (pmid) { return pmid->getX(); } return 0; };
	virtual float getY() { if (pmid) { return pmid->getY(); } return 0; };

	virtual bool isLine(){return true;};


	virtual GObject * getPiece();

	virtual bool MoveTo(float newx, float newy, bool bTry, int moveActionID=-1);
	virtual bool CallMoveTo(float newx, float newy, bool bTry, int moveActionID=-1);

	virtual bool isStraightLine() = 0;
	virtual bool toStraightLine();
	virtual bool toBezierLine() = 0;

	virtual void SetBeginEnd(float xb, float yb, float xe, float ye);
	virtual void UpdateMidPoint() = 0;
	virtual bool CheckNearTo(float px, float py, float r, float *plx, float *ply, int *isec=NULL) = 0;
	virtual bool CheckIntersectWithRect(float xl, float yt, float xr, float yb) = 0;
	virtual bool CheckIntersectWithLineObj( GLine * pLine, list<PointF2D> *pPoints );
	virtual float CalculateProportion( float x, float y, int iSec ) = 0;
	virtual float CalculateMidPointProportion() = 0;
	virtual bool GetPositionAtProportion( float fClingToProportion, float* tox, float* toy ) = 0;

	virtual bool isLengthCalculated(){return true;};
	virtual float getLength() = 0;

	virtual const char * getDisplayName();

	virtual void OnModify();

public:
	inline GAnchorPoint * GetBeginPoint(){return plbegin;};
	inline GAnchorPoint * GetEndPoint(){return plend;};
	inline GMidPoint * GetMidPoint(){return pmid;};
protected:
	GAnchorPoint * plbegin;
	GAnchorPoint * plend;
	GMidPoint * pmid;

public:
	virtual void OnRemove();

	bool AddClingBy(GPoint * pPoint);
	void DeclingByOther(GPoint * pPoint=NULL);
	bool isClingBy(GPoint * pPoint);
	list<GPoint *> * getClingBy(){return &clingByList;};
protected:
	list<GPoint *> clingByList;
};
/************************************************************************/
/* GSubstantiveLine                                                     */
/************************************************************************/
class GSubstantiveLine :
	public GLine
{
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
	GStraightLine();
	GStraightLine(GObject * parent, PointF2D pb, PointF2D pe);
	virtual ~GStraightLine();

	virtual void UpdateMidPoint();

	virtual bool canMove(){return true;};
	virtual bool isStraightLine(){return true;};

	virtual float CalculateProportion( float x, float y, int iSec );
	virtual float CalculateMidPointProportion();
	virtual bool GetPositionAtProportion( float fClingToProportion, float* tox, float* toy );

	virtual void OnRender(int iHighlightLevel=0);

	virtual bool CheckNearTo(float px, float py, float r, float *plx, float *ply, int *isec=NULL);
	virtual bool CheckIntersectWithRect(float xl, float yt, float xr, float yb);
	virtual bool CheckIntersectStraightStraight(GStraightLine * pLine, list<PointF2D> *pPoints);
	virtual void GetBoundingBox(float *xl, float *yt, float *xr, float * yb);

	virtual float getLength();
};
/************************************************************************/
/* GBezierLine                                                          */
/************************************************************************/
class GBezierLine : public GStraightLine
{
public:
	GBezierLine();
	GBezierLine(GObject * parent, PointF2D pb, PointF2D pe);
	GBezierLine(GObject * parent, PointF2D pb, PointF2D pbh, PointF2D peh, PointF2D pe);
	virtual ~GBezierLine();

    virtual void OnPrecisionChanged(float fPrecision);
	virtual void OnModify();
	virtual void UpdateMidPoint();
	virtual bool canMove(){return true;};
	virtual bool isStraightLine();
	virtual bool toBezierLine();

	virtual void OnRender(int iHighlightLevel=0);

	virtual void SetBeginEnd(float xb, float yb, float xe, float ye);

	virtual void SetBeginHandlePos(float x, float y);
	virtual void SetEndHandlePos(float x, float y);

	virtual bool CheckNearTo(float px, float py, float r, float *plx, float *ply, int * isec=NULL);
	virtual bool CheckIntersectWithRect(float xl, float yt, float xr, float yb);
	virtual bool CheckIntersectBezierStraight(GStraightLine * pLine, list<PointF2D> *pPoints);
	virtual bool CheckIntersectBezierBezier(GBezierLine * pLine, list<PointF2D> *pPoints);
	virtual void GetBoundingBox(float *xl, float *yt, float *xr, float * yb);

	virtual float CalculateProportion( float x, float y, int iSec );
	virtual float CalculateMidPointProportion();
	virtual bool GetPositionAtProportion( float fClingToProportion, float* tox, float* toy );

	virtual bool isLengthCalculated();
	virtual float getLength();

	virtual bool Clone( GObject * pNewParent );

	BezierSublinesInfo * getBSInfo(){return &bsinfo;};
private:
	BezierSublinesInfo bsinfo;
};
