#pragma once
#include "gline.h"
class GShape :
	public GObject
{
public:
	typedef GObject super;
	virtual const char * getTypeName();		// Do not implement separately
	static const char * getStaticTypeName();		// Do not implement separately
public:
	GShape(void);
	virtual ~GShape(void);

	virtual const char * getDisplayName();

	virtual GObject * CreateNewClone(GObject * pNewParent=NULL, GObject * pBeforeObj=NULL);
};