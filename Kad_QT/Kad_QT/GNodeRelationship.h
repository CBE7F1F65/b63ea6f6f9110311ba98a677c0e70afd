#pragma once

#include "GLine.h"

enum{
	GREL_NULL,
	GREL_MERGEWITH,
	GREL_BINDWITH,
	GREL_CLINGTO,
	GREL_CLINGBY,
};

class GNodeRelationship
{
public:
	GNodeRelationship(void);
	virtual ~GNodeRelationship(void);

	virtual bool CanAddRelTo(GObject * pObj) = 0;
	GObject * GetOther(){return pOther;};

	virtual bool RestoreTo(GObject * pObj, GObject * pFrom=NULL) = 0;

protected:
	GObject * pOther;
	int nRelType;
};

class GNodeRelMergeWith : public GNodeRelationship
{
public:
	typedef GNodeRelationship super;
public:
	GNodeRelMergeWith(GPoint * ptOther);
	virtual ~GNodeRelMergeWith(){};

	virtual bool CanAddRelTo( GObject * pObj );
	virtual bool RestoreTo(GObject * pObj, GObject * pFrom=NULL);
};

class GNodeRelBindWith : public GNodeRelationship
{
public:
	typedef GNodeRelationship super;
public:
	GNodeRelBindWith(/*GHandlePoint * pHdlSelf, */GHandlePoint * pHdlOther);
	virtual ~GNodeRelBindWith(){};

	virtual bool CanAddRelTo( GObject * pObj );
	virtual bool RestoreTo(GObject * pObj, GObject * pFrom=NULL);
};

class GNodeRelClingTo : public GNodeRelationship
{
public:
	typedef GNodeRelationship super;
public:
	GNodeRelClingTo(GClingInfo &cli);
	virtual ~GNodeRelClingTo(){};

	virtual bool CanAddRelTo( GObject * pObj );
	virtual bool RestoreTo(GObject * pObj, GObject * pFrom=NULL);

	GClingInfo clInfo;
};

class GNodeRelClingBy : public GNodeRelationship
{
public:
	typedef GNodeRelationship super;
public:
	GNodeRelClingBy(/*GLine * pLnSelf, */GPoint * pPtOther, GClingInfo &cli);
	virtual ~GNodeRelClingBy(){};

	virtual bool CanAddRelTo( GObject * pObj );
	virtual bool RestoreTo(GObject * pObj, GObject * pFrom=NULL);

	GClingInfo clInfo;
};

class GNodeRelationshipGroup
{
public:
	GNodeRelationshipGroup(GObject * pObj);
	virtual ~GNodeRelationshipGroup();

	bool AddRelationship(GNodeRelationship * pRel);
	bool RemoveRelationship(GObject * pOther=NULL);

	list<GNodeRelationship *> * GetRelList(){return &lstRels;};

	bool RestoreTo(GObject * pObj);

	GObject * GetSelf(){return pSelf;};

private:
	GObject * pSelf;
	list<GNodeRelationship *> lstRels;
};