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

	virtual float getX(){return x;};
	virtual float getY(){return y;};
public:
	void SetPosition(float x, float y);
public:
	virtual bool canMove(){return true;};

	virtual bool MoveTo( float newx, float newy, bool bTry );
	virtual bool isPoint(){return true;};

	virtual GObject * getLine()
	{
		GObject * pObj = pParent;
		while (pObj)
		{
			if (pObj->isLine())
			{
				return pObj;
			}
			pObj = pObj->getParent();
		}
		return NULL;
	};
	virtual GObject * getPiece()
	{
		GObject * pObj = pParent;
		while (pObj)
		{
			if (pObj->isPiece())
			{
				return pObj;
			}
			pObj = pObj->getParent();
		}
		return NULL;
	};

	virtual const char * getDisplayName();

	virtual bool Clone( GObject * pNewParent );

	PointF2D GetPointF2D(){return PointF2D(x, y);};

	float x;
	float y;
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

	virtual void OnRender(int iHighlightLevel/* =0 */);
	GObject * GetAnchor(){return getParent();};
	virtual bool isHandlePoint(){return true;};

	virtual const char * getDisplayName();
	virtual bool Clone( GObject * pNewParent );
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

	virtual bool MoveTo( float newx, float newy, bool bTry );
	virtual const char * getDisplayName();
	virtual bool Clone( GObject * pNewParent );
	virtual bool isAnchorPoint(){return true;};

	GHandlePoint * GetHandle(){return phandle;};
	void SetHandlePosition( float x, float y );
	bool isHandleIdentical();
protected:
	GHandlePoint * phandle;
};