#pragma once
#include "gline.h"
class GShape :
	public GObject
{
public:
	GShape(void);
	~GShape(void);

	virtual const char * getDisplayName();

	virtual bool Clone( GObject * pNewParent );
};