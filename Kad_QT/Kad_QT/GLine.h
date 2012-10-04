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
	virtual const char * getTypeName();		// Do not implement separately
	static const char * getStaticTypeName();		// Do not implement separately
public:
	GLine(void);
	virtual ~GLine(void);

	virtual bool ReadXML( GObjectXMLNode * pnode );
	virtual bool WriteXML(QXmlStreamWriter &qsw);

	virtual bool CloneData(GObject * pClone, GObject * pNewParent, bool bNoRelationship=true);

	virtual float getX() { if (pmid) { return pmid->getX(); } return 0; };
	virtual float getY() { if (pmid) { return pmid->getY(); } return 0; };

	virtual bool isLine(){return true;};

	virtual GLine * getLine(){return this;};
	virtual GPiece * getPiece();

	virtual bool MoveTo(GObject * pCaller, float newx, float newy, bool bTry, int moveActionID=-1);
	virtual bool CallMoveTo(GObject * pCaller, float newx, float newy, bool bTry, int moveActionID=-1);
	virtual bool CallRotate(GObject * pCaller, float orix, float oriy, int angle, bool bTry, int moveActionID=-1);
	virtual bool CallFlip(GObject * pCaller, float orix, float oriy, int angle, bool bTry, int moveActionID=-1);
	virtual bool CallScale(GObject * pCaller, float orix, float oriy, float fScaleX, float fScaleY, bool bTry, int moveActionID=-1);

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
	virtual bool GetPositionAtProportion( float fProp, PointF2D * pptPos, int*isec=NULL, QuadBezierPointF2D * pQuadHandles=NULL ) = 0;
	virtual bool GetPositionAtExtendedProportion(float fProp, PointF2D *pptPos, QuadBezierPointF2D * pQuadOutput=NULL) {return GetPositionAtProportion(fProp, pptPos, NULL, pQuadOutput);};
	virtual GLine * Clip(float fClipProportion) = 0;
	virtual bool JoinLine(GLine * pLine) = 0;
	virtual bool SwapBeginEnd() = 0;
	virtual bool Extend(float tBegin, float tEnd) = 0;

	virtual void SetLineRenderStyle(int nStyle){nLineRenderStyle = nStyle;};
	virtual int GetLineRenderStyle(){return nLineRenderStyle;};

	virtual bool isLengthCalculated(){return true;};
	virtual float getLength() = 0;

	virtual PointF2D GetTangentPointF2D(float fProp) = 0;

	virtual const char * getDisplayName();

	virtual void OnModify();

	inline GAnchorPoint * GetBeginPoint(){return plbegin;};
	inline GAnchorPoint * GetEndPoint(){return plend;};
	inline GMidPoint * GetMidPoint(){return pmid;};

	virtual void OnRemove();

	virtual GNodeRelationshipGroup * CreateRelationshipGroup(bool bClingBy=true, bool bOneWay=false);
	virtual void OnIndepend();

	bool AddClingBy(GPoint * pPoint);
	void DeclingByOther(GPoint * pPoint=NULL);
	bool isClingBy(GPoint * pPoint);
	list<GPoint *> * getClingBy(){return &clingByList;};

	/************************************************************************/
	/* Members                                                              */
	/************************************************************************/
protected:
	//////////////////////////////////////////////////////////////////////////
	GOBJM_COPYABLES();
	int nLineRenderStyle;
	GOBJM_COPYABLESEND();
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
	virtual bool GetPositionAtProportion( float fProp, PointF2D * pptPos, int*isec=NULL, QuadBezierPointF2D * pQuadHandles=NULL );

	virtual void OnRender(int iHighlightLevel=0);

	virtual bool CheckNearTo(float px, float py, float r, float *plx, float *ply, int *isec=NULL);
	virtual bool CheckIntersectWithRect(float xl, float yt, float xr, float yb);
	virtual bool CheckIntersectStraightStraight(GStraightLine * pLine, list<PointF2D> *pPoints);
	virtual void GetBoundingBox(float *xl, float *yt, float *xr, float * yb);

	virtual float getLength();

	virtual PointF2D GetTangentPointF2D(float fProp);
};
/************************************************************************/
/* GBezierLine                                                          */
/************************************************************************/
class GBezierLine : public GStraightLine
{
public:
	typedef GStraightLine super;
	virtual const char * getTypeName();		// Do not implement seperately
	static const char * getStaticTypeName();		// Do not implement separately
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
	virtual bool GetPositionAtProportion( float fProp, PointF2D * pptPos, int*isec=NULL, QuadBezierPointF2D * pQuadHandles=NULL );
	virtual bool GetPositionAtExtendedProportion(float fProp, PointF2D *pptPos, QuadBezierPointF2D * pQuadOutput=NULL);
	virtual GLine * Clip(float fClipProportion);
	virtual bool JoinLine(GLine * pLine);
	virtual bool SwapBeginEnd();

	virtual bool Extend(float tBegin, float tEnd);

	virtual bool isLengthCalculated();
	virtual float getLength();

	virtual PointF2D GetTangentPointF2D(float fProp);


	BezierSublinesInfo * getBSInfo(){return &bsinfo;};
	float GetSFromProportion( float fProp );
	/************************************************************************/
	/* Members                                                              */
	/************************************************************************/
protected:
	//////////////////////////////////////////////////////////////////////////
	GOBJM_NONCOPYABLES();
	BezierSublinesInfo bsinfo;
	GOBJM_NONCOPYABLESEND();
};
