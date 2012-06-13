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

	virtual void SetBeginEnd(float xb, float yb, float xe, float ye);
	virtual void UpdateMidPoint() = 0;
	virtual bool CheckNearTo(float px, float py, float r, float *plx, float *ply) = 0;

	virtual const char * getDisplayName();

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

	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender(bool bHighlight=false);

	virtual bool CheckNearTo(float px, float py, float r, float *plx, float *ply);
	virtual void GetBoundingBox(float *xl, float *yt, float *xr, float * yb);

	virtual bool Clone( GObject * pNewParent );

	bool bModified;
};