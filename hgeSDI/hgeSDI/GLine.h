#pragma once
#include "GObject.h"

#include "GPoint.h"

class GLine :
	public GObject
{
public:
	GLine(void);
	GLine(GObject * parent);
	~GLine(void);

	virtual float getX()
	{
		if (pmid)
		{
			return pmid->getX();
		}
		return 0;
	};
	virtual float getY()
	{
		if (pmid)
		{
			return pmid->getY();
		}
		return 0;
	};

	virtual bool isLine(){return true;};


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

	virtual bool MoveTo(float newx, float newy, bool bTry);

	virtual void SetBeginEnd(float xb, float yb, float xe, float ye);
	virtual void UpdateMidPoint() = 0;
	virtual bool CheckNearTo(float px, float py, float r, float *plx, float *ply) = 0;
	virtual bool CheckIntersectWithRect(float xl, float yt, float xr, float yb) = 0;

	virtual const char * getDisplayName();

	virtual void OnModify();
	GPoint * plbegin;
	GPoint * plend;
	GPoint * pmid;
};

class GSubstantiveLine :
	public GLine
{
public:
	GSubstantiveLine(){};
	~GSubstantiveLine(){};

	virtual bool isRepresentableLine(){return true;};
};

class GVirtualLine : 
	public GLine
{
	GVirtualLine(){};
	~GVirtualLine(){};
};

class GStraightLine : public GSubstantiveLine
{
public:
	GStraightLine();
	GStraightLine(GObject * parent);
	~GStraightLine();

	virtual void UpdateMidPoint();

	virtual bool canMove(){return true;};

	virtual void OnInit();
	virtual void OnRender(int iHighlightLevel=0);

	virtual bool CheckNearTo(float px, float py, float r, float *plx, float *ply);
	virtual bool CheckIntersectWithRect(float xl, float yt, float xr, float yb);
	virtual void GetBoundingBox(float *xl, float *yt, float *xr, float * yb);

	virtual bool Clone( GObject * pNewParent );

	bool bModified;
};