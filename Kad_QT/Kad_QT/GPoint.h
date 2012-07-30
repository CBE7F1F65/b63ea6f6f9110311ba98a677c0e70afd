#pragma once
#include "GObject.h"
#include "MathHelper.h"
/************************************************************************/
/* GPoint                                                               */
/************************************************************************/
class GPoint :
	public GObject
{
public:
	typedef GObject super;

public:
	GPoint();
	virtual ~GPoint();

	virtual bool isPoint(){return true;};

	virtual GLine * getLine();
	virtual GPiece * getPiece();

	virtual const char * getDisplayName();

	virtual GObject * CreateNewClone(GObject * pNewParent=NULL, GObject * pBeforeObj=NULL);
	virtual bool CloneData(GObject * pClone, GObject * pNewParent, bool bNoRelationship=true);

	virtual inline float getX(){return x;};
	virtual inline float getY(){return y;};
	void SetPosition(float x, float y);

	virtual bool canMove(){return true;};
	virtual bool MoveTo( GObject * pCaller, float newx, float newy, bool bTry, int moveActionID=-1 );
	virtual bool CallMoveTo( GObject * pCaller, float newx, float newy, bool bTry, int moveActionID=-1 );
	virtual bool CallRotate(GObject * pCaller, float orix, float oriy, int angle, bool bTry, int moveActionID=-1);
	virtual bool CallFlip(GObject * pCaller, float orix, float oriy, int angle, bool bTry, int moveActionID=-1);
	virtual bool CallScale(GObject * pCaller, float orix, float oriy, float fScaleX, float fScaleY, bool bTry, int moveActionID=-1);

	PointF2D GetPointF2D(){return PointF2D(x, y);};

	virtual void OnRemove();

	virtual GNodeRelationshipGroup * CreateRelationshipGroup();
	virtual void Independ();

	// Only Point To Line!
	void ClearClingTo();
	bool ClingTo(GObject * pObj, float fProp);
	void DeclingToOther();
	bool isClingTo(GObject * pObj);
	GLine * getClingTo(){return pClingTo;};
	float getClingProportion(){return fClingToProportion;};

	// Only Point To Point!
	bool MergeWith(GPoint * pPoint, bool bNoBackward=false);
	bool isMergeWith(GPoint * pPoint);
	bool SeperateFrom(GPoint * pPoint=NULL, bool bNoBackward=false);
	list<GPoint *> * getMergeWith(){return &mergeWithList;};
	void CallClingToMoved( bool bTry, int moveActionID );

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
	GLine * pClingTo;
	float fClingToProportion;
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
public:
	GMidPoint();
	GMidPoint(GObject * parent);
	virtual ~GMidPoint();

	virtual bool canBeMergedWith(){return true;};

	virtual GObject * getEntity(){return (GObject *)getLine();};

	virtual bool isMidPoint(){return true;};
	virtual bool isModifyParent(){return false;};
	virtual bool isSlaveToLine(){return true;};
	virtual bool canMove(){return false;};

	virtual const char * getDisplayName();
	virtual GObject * CreateNewClone(GObject * pNewParent=NULL, GObject * pBeforeObj=NULL);
	/************************************************************************/
	/* Members                                                              */
	/************************************************************************/
};
/************************************************************************/
/* GHandlePoint                                                         */
/************************************************************************/
class GHandlePoint : public GAttributePoint
{
public:
	typedef GAttributePoint super;
public:
	GHandlePoint();
	GHandlePoint(GObject * parent, float x, float y);
	virtual ~GHandlePoint();

	virtual GObject * CreateNewClone(GObject * pNewParent=NULL, GObject * pBeforeObj=NULL);
	virtual bool CloneData(GObject * pClone, GObject * pNewParent, bool bNoRelationship=true);

	virtual GObject * getEntity(){return (GObject *)getLine();};

	virtual void OnRender(int iHighlightLevel/* =0 */);
	GAnchorPoint * GetAnchor(){return (GAnchorPoint *)getParent();};
	virtual bool isHandlePoint(){return true;};

	bool isIdenticalToAnchor();

	virtual bool MoveTo( GObject * pCaller, float newx, float newy, bool bTry, int moveActionID=-1 );

	virtual const char * getDisplayName();

	virtual GNodeRelationshipGroup * CreateRelationshipGroup();
	virtual void Independ();

	GHandlePoint * getBindWith(){return pBindWith;};
    bool isBindWith(GHandlePoint * pHandle){return pBindWith==pHandle;};
	bool BindWith(GHandlePoint * pHandle=NULL);
	bool UnbindTo(GHandlePoint * pHandle);
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
public:
	GAnchorPoint();
	GAnchorPoint(GObject * parent, float x, float y);
	virtual ~GAnchorPoint();

	virtual GObject * CreateNewClone(GObject * pNewParent=NULL, GObject * pBeforeObj=NULL);
	virtual bool CloneData(GObject * pClone, GObject * pNewParent, bool bNoRelationship=true);

	virtual bool canAttach(){return true;};
	virtual bool canBeMergedWith(){return true;};
	virtual GObject * getEntity(){return (GObject *)getLine();};

	virtual bool MoveTo( GObject * pCaller, float newx, float newy, bool bTry, int moveActionID=-1 );
	virtual const char * getDisplayName();
	virtual bool isAnchorPoint(){return true;};

	GHandlePoint * GetHandle(){return phandle;};
	void SetHandlePosition( float hx, float hy, float fAllowance=-1 );
	bool isHandleIdentical();
	/************************************************************************/
	/* Members                                                              */
	/************************************************************************/
protected:
	//////////////////////////////////////////////////////////////////////////
	GOBJM_CHILDPOINTERS();
	GHandlePoint * phandle;
	GOBJM_CHILDPOINTERSEND();
};
