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

private:
	virtual void setInitial(int layerID, DWORD linecolor, const char * layername);
public:

	virtual const char * getDisplayName();
	virtual bool isLayer(){return true;};

	virtual bool Clone( GObject * pNewParent );
	int layerID;
};

