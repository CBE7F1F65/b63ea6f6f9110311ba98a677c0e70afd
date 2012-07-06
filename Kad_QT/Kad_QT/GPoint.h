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
	GPoint();
	virtual ~GPoint();

	virtual inline float getX(){return x;};
	virtual inline float getY(){return y;};
public:
	void SetPosition(float x, float y);
public:
	virtual bool canMove(){return true;};

	virtual bool MoveTo( float newx, float newy, bool bTry, int moveActionID=-1 );
	virtual bool CallMoveTo( float newx, float newy, bool bTry, int moveActionID=-1 );
	virtual bool isPoint(){return true;};

	virtual GObject * getLine();
	virtual GObject * getPiece();

	virtual const char * getDisplayName();

	virtual bool Clone( GObject * pNewParent );

	PointF2D GetPointF2D(){return PointF2D(x, y);};

protected:
	float x;
	float y;

public:

	virtual void OnRemove();

	// Only Point To Line!
	void ClearClingTo();
	bool ClingTo(GObject * pObj, float fProp);
	void DeclingToOther();
	bool isClingTo(GObject * pObj);
	GObject * getClingTo(){return pClingTo;};
	float getClingProportion(){return fClingToProportion;};

	// Only Point To Point!
	bool MergeWith(GPoint * pPoint, bool bNoBackward=false);
	bool isMergeWith(GPoint * pPoint);
	bool SeperateFrom(GPoint * pPoint=NULL, bool bNoBackward=false);
	list<GPoint *> * getMergeWith(){return &mergeWithList;};
	void CallClingToMoved( bool bTry, int moveActionID );
protected:
	GObject * pClingTo;
	float fClingToProportion;

	list<GPoint *> mergeWithList;
};
/************************************************************************/
/* GSubstantivePoint                                                    */
/************************************************************************/
class GSubstantivePoint :
	public GPoint
{
public:
	GSubstantivePoint();
	virtual ~GSubstantivePoint();

//	virtual bool isAttributeNode(){return false;};
	virtual bool isRepresentablePoint(){return true;};
	virtual bool canAttach(){return true;};
	virtual bool canBeMergedWith(){return true;};

	virtual void OnRender(int iHighlightLevel=0);
};
/************************************************************************/
/* GAttributePoint                                                      */
/************************************************************************/
class GAttributePoint :
	public GPoint
{
public:
	GAttributePoint(){};
	virtual ~GAttributePoint(){};

	virtual bool isAttributeNode(){return true;};
	virtual bool isRepresentablePoint(){return true;};

	virtual void OnRender(int iHighlightLevel=0);
};
/************************************************************************/
/* GVirtualPoint                                                        */
/************************************************************************/
class GVirtualPoint :
	public GAttributePoint
{
public:
	GVirtualPoint();
	virtual ~GVirtualPoint();

	virtual bool isRepresentablePoint(){return false;};
	virtual bool isModifyParent(){return false;};

	virtual void OnRender(int iHighlightLevel=0);
};
/************************************************************************/
/* GMidPoint                                                            */
/************************************************************************/
class GMidPoint : public GAttributePoint
{
public:
	GMidPoint();
	GMidPoint(GObject * parent);
	virtual ~GMidPoint();

	virtual bool canBeMergedWith(){return true;};

	virtual GObject * getEntity(){return getLine();};

	virtual bool isMidPoint(){return true;};
	virtual bool isModifyParent(){return false;};
	virtual bool isSlaveToLine(){return true;};
	virtual bool canMove(){return false;};

	virtual const char * getDisplayName();
	virtual bool Clone( GObject * pNewParent );
};
/************************************************************************/
/* GHandlePoint                                                         */
/************************************************************************/
class GHandlePoint : public GAttributePoint
{
public:
	GHandlePoint();
	GHandlePoint(GObject * parent, float x, float y);
	virtual ~GHandlePoint();

	virtual GObject * getEntity(){return getLine();};

	virtual void OnRender(int iHighlightLevel/* =0 */);
	GObject * GetAnchor(){return getParent();};
	virtual bool isHandlePoint(){return true;};

	virtual bool MoveTo( float newx, float newy, bool bTry, int moveActionID=-1 );

	virtual const char * getDisplayName();
	virtual bool Clone( GObject * pNewParent );

	GHandlePoint * getBindTo(){return pBindTo;};
	bool isBindTo(GHandlePoint * pHandle){DASSERT(pHandle); return pBindTo==pHandle;};
	bool BindTo(GHandlePoint * pHandle=NULL);
	bool UnbindTo(GHandlePoint * pHandle);
	GHandlePoint * pBindTo;
};
/************************************************************************/
/* GAnchorPoint                                                         */
/************************************************************************/
class GAnchorPoint : public GAttributePoint
{
public:
	GAnchorPoint();
	GAnchorPoint(GObject * parent, float x, float y);
	virtual ~GAnchorPoint();

	virtual bool canAttach(){return true;};
	virtual bool canBeMergedWith(){return true;};
	virtual GObject * getEntity(){return getLine();};

	virtual bool MoveTo( float newx, float newy, bool bTry, int moveActionID=-1 );
	virtual const char * getDisplayName();
	virtual bool Clone( GObject * pNewParent );
	virtual bool isAnchorPoint(){return true;};

	GHandlePoint * GetHandle(){return phandle;};
	void SetHandlePosition( float x, float y );
	bool isHandleIdentical();
protected:
	GHandlePoint * phandle;
};