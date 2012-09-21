#pragma once
#include "gobject.h"

class GLayer :
	public GObject
{
public:
	typedef GObject super;
	virtual const char * getTypeName();		// Do not implement separately
protected:
	GLayer(void);
public:
	GLayer(int layerID, const char * layername);
	virtual ~GLayer(void);

	virtual bool ReadXML(QXmlStreamReader &qsr);
	virtual bool WriteXML(QXmlStreamWriter &qsw);

private:
	virtual void setInitial(int layerID, const char * layername);
public:
	virtual GObject * CreateNewClone(GObject * pNewParent=NULL, GObject * pBeforeObj=NULL);
	virtual bool CloneData(GObject * pClone, GObject * pNewParent, bool bNoRelationship=true);

	virtual const char * getDisplayName();
	virtual bool isLayer(){return true;};

	int getLayerID(){return layerID;};

	/************************************************************************/
	/* Members                                                              */
	/************************************************************************/
protected:
	//////////////////////////////////////////////////////////////////////////
	GOBJM_NEEDINITIAL();

	GOBJM_DONOTCOPY();
	int layerID;
	GOBJM_DONOTCOPYEND();
};

