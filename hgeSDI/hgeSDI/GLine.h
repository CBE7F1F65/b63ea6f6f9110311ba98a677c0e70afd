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

	virtual void SetBeginEnd(float xb, float yb, float xe, float ye);
	virtual void UpdateMidPoint() = 0;

	virtual const char * getDisplayName();

	GPoint * plbegin;
	GPoint * plend;
	GPoint * pmid;
};

class GStraightLine : public GLine
{
public:
	GStraightLine();
	GStraightLine(GObject * parent);
	~GStraightLine();

	virtual void UpdateMidPoint();

	virtual void OnInit();
	virtual void OnUpdate();
	virtual void OnRender();

	virtual bool Clone( GObject * pNewParent );

	bool bModified;
};