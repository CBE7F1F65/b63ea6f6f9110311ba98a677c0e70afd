#pragma once
#include "gline.h"
class GShape :
	public GObject
{
public:
	typedef GObject super;
public:
	GShape(void);
	virtual ~GShape(void);

	virtual const char * getDisplayName();

	virtual GObject * CreateNewClone(GObject * pNewParent=NULL, GObject * pBeforeObj=NULL);
};