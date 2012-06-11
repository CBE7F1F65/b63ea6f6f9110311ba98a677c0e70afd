#pragma once
#include "gobject.h"

class GLayer :
	public GObject
{
protected:
	GLayer(void);
public:
	GLayer(int layerID, DWORD linecolor, const char * layername);
	~GLayer(void);

	virtual const char * getDisplayName();
	virtual bool isLayer(){return true;};

	int layerID;
};

