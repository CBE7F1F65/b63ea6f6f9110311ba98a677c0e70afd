#pragma once
#include "GObject.h"
#include "MathHelper.h"

/************************************************************************/
/* GClingInfo                                                           */
/************************************************************************/

enum{
	GCLING_PROPORTION,
	GCLING_BEGINOFFSET,
	GCLING_ENDOFFSET,
};
class GClingInfo
{
public:
	GClingInfo(){ClearClingTo();};
	virtual ~GClingInfo(){};

	bool SetClingTo(GLine * pTo, float fVal, float fa, int nType=GCLING_PROPORTION);
	void ClearClingTo();

	bool CalculateClingProportion(float * pProp, float fLengthBase=-1);
	bool GetClingPosition(PointF2D * pptPos, int * isec=NULL, QuadBezierPointF2D * pQuadHandles=NULL, float * pProp=NULL);

	GLine * GetClingTo(){return pClingTo;};
	float GetClingVal(){return fClingVal;};
	int GetClingType(){return nClingType;};

	bool isClingTo(GObject * pObj);
	//////////////////////////////////////////////////////////////////////////
	bool ApplyChange( GLine * pLine, float fProportion, float fa );
	bool ApplyTypeChange(int nType);
	//////////////////////////////////////////////////////////////////////////

private:
	GLine * pClingTo;
	float fClingVal;
	int nClingType;
	float fAllowance;
};

/************************************************************************/
/* GPoint                                                               */
/************************************************************************/
class GPoint :
	public GObject
{
public:
	typedef GObject super;
	virtual const char * getTypeName();		// Do not implement separately
	static const char * getStaticTypeName();		// Do not implement separately

public:
	GPoint();
	virtual ~GPoint();

	virtual bool ReadXML( GObjectXMLNode * pnode );	// Do not implement separately
	virtual bool WriteXML(QXmlStreamWriter &qsw);	// Do not implement separately

	virtual bool isPoint(){return true;};

	virtual GLine * getLine();
	virtual GPiece * getPiece();

	virtual const char * getDisplayName();

	virtual GObject * CreateNewClone(GObject * pNewParent=NULL, GObject * pBeforeObj=NULL);
	virtual bool CloneData(GObject * pClone, GObject * pNewParent, bool bNoRelationship=true);

	virtual inline float getX(){return x;};
	virtual inline float getY(){return y;};
	void SetPosition(float x, float y);

	virtual bool canMove(){return !isDisplayLocked();};
	virtual bool MoveTo( GObject * pCaller, float newx, float newy, bool bTry, int moveActionID=-1 );
	virtual bool CallMoveTo( GObject * pCaller, float newx, float newy, bool bTry, int moveActionID=-1 );
	virtual bool CallRotate(GObject * pCaller, float orix, float oriy, int angle, bool bTry, int moveActionID=-1);
	virtual bool CallFlip(GObject * pCaller, float orix, float oriy, int angle, bool bTry, int moveActionID=-1);
	virtual bool CallScale(GObject * pCaller, float orix, float oriy, float fScaleX, float fScaleY, bool bTry, int moveActionID=-1);

	PointF2D GetPointF2D(){return PointF2D(x, y);};

	virtual bool GetBoundingBox(float * lx, float * ty, float * rx, float * by);

	virtual void OnRemove();

	virtual GNodeRelationshipGroup * CreateRelationshipGroup(bool bClingBy=true, bool bOneWay=false);
	virtual void OnIndepend();

	// Only Point To Line!
	virtual float GetClingAllowance(){return M_SMALLFLOAT;};

	void ClearClingTo();
	bool ClingTo(GLine* pLine, float fVal, int nType=GCLING_PROPORTION);
	bool ClingTo(GClingInfo &cl){return ClingTo(cl.GetClingTo(), cl.GetClingVal(), cl.GetClingType());};
	bool canClingTo(GLine * pLine);
	static bool staticCheckClingToCB(GObject * pThis, void * param);
	void DeclingToOther();
	bool isClingTo(GObject * pObj);
	GClingInfo * getClingInfo(){return &clInfo;};
	void CallClingToMoved( bool bTry, int moveActionID );

	// Only Point To Point!
	bool MergeWith(GPoint * pPoint, bool bNoBackward=false);
	bool isMergeWith(GPoint * pPoint);
	bool DemergeFrom(GPoint * pPoint=NULL, bool bNoBackward=false);
	list<GPoint *> * getMergeWith(){return &mergeWithList;};
	void DispatchPassiveMergeMove(GObject * pCaller, float newx, float newy, bool bTry, int moveActionID=-1);


	virtual bool Isolate();

protected:
	/************************************************************************/
	/* Members                                                              */
	/************************************************************************/
	//////////////////////////////////////////////////////////////////////////
	GOBJM_COPYABLES();
	float x;
	float y;

	GOBJM_COPYABLESEND();
	//////////////////////////////////////////////////////////////////////////
	
	GOBJM_NONCOPYABLES();
	GClingInfo clInfo;

//	GLine * pClingTo;
//	float fClingToProportion;

	list<GPoint *> mergeWithList;
	GOBJM_NONCOPYABLESEND();
};
/************************************************************************/
/* GSubstantivePoint                                                    */
/************************************************************************/
class GSubstantivePoint :
	public GPoint
{
public:
	typedef GPoint super;

public:
	GSubstantivePoint();
	virtual ~GSubstantivePoint();

//	virtual bool isAttributeNode(){return false;};
	virtual bool isRepresentablePoint(){return true;};
	virtual bool canAttach(){return true;};
	virtual bool canBeMergedWith(){return true;};

	virtual void OnRender(int iHighlightLevel=0);
	/************************************************************************/
	/* Members                                                              */
	/************************************************************************/
};
/************************************************************************/
/* GAttributePoint                                                      */
/************************************************************************/
class GAttributePoint :
	public GPoint
{
public:
	typedef GPoint super;

public:
	GAttributePoint(){};
	virtual ~GAttributePoint(){};

	virtual bool isAttributeNode(){return true;};
	virtual bool isRepresentablePoint(){return true;};

	virtual void OnRender(int iHighlightLevel=0);
	/************************************************************************/
	/* Members                                                              */
	/************************************************************************/
};
/************************************************************************/
/* GVirtualPoint                                                        */
/************************************************************************/
class GVirtualPoint :
	public GAttributePoint
{
public:
	typedef GAttributePoint super;
public:
	GVirtualPoint();
	virtual ~GVirtualPoint();

	virtual bool isRepresentablePoint(){return false;};
	virtual bool isModifyParent(){return false;};

	virtual void OnRender(int iHighlightLevel=0);
	/************************************************************************/
	/* Members                                                              */
	/************************************************************************/
};
/************************************************************************/
/* GMidPoint                                                            */
/************************************************************************/
class GMidPoint : public GAttributePoint
{
public:
	typedef GAttributePoint super;
	virtual const char * getTypeName();		// Do not implement separately
	static const char * getStaticTypeName();		// Do not implement separately
public:
	GMidPoint();
	GMidPoint(GObject * parent);
	virtual ~GMidPoint();

	virtual GObject * getEntity(){return (GObject *)getLine();};

	virtual bool isMidPoint(){return true;};
	virtual bool isModifyParent(){return false;};
	virtual bool isSlaveToLine(){return true;};
	virtual bool canMove(){return false;};

	virtual void OnRender(int iHighlightLevel/* =0 */);

	virtual const char * getDisplayName();
	virtual GObject * CreateNewClone(GObject * pNewParent=NULL, GObject * pBeforeObj=NULL);
	/************************************************************************/
	/* Members                                                              */
	/************************************************************************/
};
/************************************************************************/
/* GHandlePoint                                                         */
/************************************************************************/
class GHandlePoint : public GSubstantivePoint
{
public:
	typedef GSubstantivePoint super;
	virtual const char * getTypeName();		// Do not implement separately
	static const char * getStaticTypeName();		// Do not implement separately
public:
	GHandlePoint();
	GHandlePoint(GObject * parent, float x, float y);
	virtual ~GHandlePoint();

	virtual bool ReadXML( GObjectXMLNode * pnode );	// Do not implement separately
	virtual bool WriteXML(QXmlStreamWriter &qsw);	// Do not implement separately

	virtual GObject * CreateNewClone(GObject * pNewParent=NULL, GObject * pBeforeObj=NULL);
	virtual bool CloneData(GObject * pClone, GObject * pNewParent, bool bNoRelationship=true);

	virtual GObject * getEntity(){return (GObject *)getLine();};

	virtual void OnRender(int iHighlightLevel/* =0 */);
	GAnchorPoint * GetAnchor(){return (GAnchorPoint *)getParent();};
	virtual bool isHandlePoint(){return true;};

	bool isIdenticalToAnchor();

	virtual bool MoveTo( GObject * pCaller, float newx, float newy, bool bTry, int moveActionID=-1 );

	virtual const char * getDisplayName();

	virtual GNodeRelationshipGroup * CreateRelationshipGroup(bool bClingBy=true, bool bOneWay=false);
	virtual void OnIndepend();

	GHandlePoint * getBindWith(){return pBindWith;};
    bool isBindWith(GHandlePoint * pHandle){return pBindWith==pHandle;};
	bool BindWith(GHandlePoint * pHandle=NULL);
	bool UnbindTo(GHandlePoint * pHandle);

	virtual bool Isolate();
	/************************************************************************/
	/* Members                                                              */
	/************************************************************************/
protected:
	//////////////////////////////////////////////////////////////////////////
	GOBJM_DONOTCOPY();
	GHandlePoint * pBindWith;
	GOBJM_DONOTCOPYEND();
};
/************************************************************************/
/* GAnchorPoint                                                         */
/************************************************************************/
class GAnchorPoint : public GAttributePoint
{
public:
	typedef GAttributePoint super;
	virtual const char * getTypeName();		// Do not implement separately
	static const char * getStaticTypeName();		// Do not implement separately
public:
	GAnchorPoint();
	GAnchorPoint(GObject * parent, float x, float y);
	virtual ~GAnchorPoint();

	virtual bool ReadXML( GObjectXMLNode * pnode );	// Do not implement separately
	virtual bool WriteXML(QXmlStreamWriter &qsw);	// Do not implement separately

	virtual GObject * CreateNewClone(GObject * pNewParent=NULL, GObject * pBeforeObj=NULL);
	virtual bool CloneData(GObject * pClone, GObject * pNewParent, bool bNoRelationship=true);

	virtual bool canAttach(){return true;};
	virtual bool canBeMergedWith(){return true;};
	virtual GObject * getEntity(){return (GObject *)getLine();};

	virtual bool MoveTo( GObject * pCaller, float newx, float newy, bool bTry, int moveActionID=-1 );
	virtual const char * getDisplayName();
	virtual bool isAnchorPoint(){return true;};

	GHandlePoint * GetHandle(){return pHandle;};
	void SetHandlePosition( float hx, float hy, float fAllowance=-1 );
	bool isHandleIdentical();
	/************************************************************************/
	/* Members                                                              */
	/************************************************************************/
protected:
	//////////////////////////////////////////////////////////////////////////
	GOBJM_CHILDPOINTERS();
	GHandlePoint * pHandle;
	GOBJM_CHILDPOINTERSEND();
};

/************************************************************************/
/* GNotch                                                               */
/************************************************************************/

class GNotch : public GAttributePoint
{
public:
	typedef GAttributePoint super;
	virtual const char * getTypeName();		// Do not implement separately
	static const char * getStaticTypeName();		// Do not implement separately

public:
	GNotch();
	GNotch(GObject * pParent, GClingInfo * pClingInfo);
	virtual ~GNotch();

	virtual bool OnUpdate();

	virtual bool ReadXML( GObjectXMLNode * pnode );	// Do not implement separately
	virtual bool WriteXML(QXmlStreamWriter &qsw);	// Do not implement separately

	virtual float GetClingAllowance(){return 0;};
	virtual GObject * CreateNewClone(GObject * pNewParent=NULL, GObject * pBeforeObj=NULL);

	virtual bool isAttributeNode(){return false;};

	virtual bool canAttach(){return true;};
	virtual GObject * getEntity(){return this;};

	virtual int GetAngle();
	virtual void OnRender(int iHighlightLevel/* =0 */);

	virtual GLine * getLine(){return clInfo.GetClingTo();};
	virtual const char * getDisplayName();
	virtual bool isNotch(){return true;};
};