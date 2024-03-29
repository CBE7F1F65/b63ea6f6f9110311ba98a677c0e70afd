#pragma once
#include "glayer.h"
#include "GNodeRelationship.h"

class GBaseNodeCopyStack : public GObject
{
public:
	typedef GObject super;
	virtual const char * getTypeName();		// Do not implement separately
	static const char * getStaticTypeName();		// Do not implement separately
public:
	GBaseNodeCopyStack(){};
	virtual ~GBaseNodeCopyStack(){};

public:
	virtual GObject * CreateNewClone(GObject * pNewParent=NULL, GObject * pBeforeObj=NULL) {ASSERT(false); return NULL;};
};

class GTempHoldingPlace : public GObject
{
public:
	typedef GObject super;
	virtual const char * getTypeName();		// Do not implement separately
	static const char * getStaticTypeName();		// Do not implement separately
public:
	GTempHoldingPlace(){};
	virtual ~GTempHoldingPlace(){};
public:
	virtual GObject * CreateNewClone(GObject * pNewParent=NULL, GObject * pBeforeObj=NULL) {ASSERT(false); return NULL;};
};

class GBaseNode :
	public GLayer
{
public:
	typedef GLayer super;
	virtual const char * getTypeName();		// Do not implement separately
	static const char * getStaticTypeName();		// Do not implement separately
public:
	GBaseNode(void);
	virtual ~GBaseNode(void);

	void SaveSelectState();

	void CallBaseUpdate();
	void RestoreBaseFrom( GBaseNodeCopyStack * pFrom );
    void CopyBaseTo( GBaseNodeCopyStack * pTo );

public:
	/************************************************************************/
	/* Members                                                              */
	/************************************************************************/
	//////////////////////////////////////////////////////////////////////////
	GOBJM_COPYABLES();
	list<int> savedlstselect;
	list<int> savedlstactive;
	int savedactivelayer;
	GOBJM_COPYABLESEND();

public:
	virtual GObject * CreateNewClone(GObject * pNewParent=NULL, GObject * pBeforeObj=NULL);
	virtual bool CloneData(GObject * pClone, GObject * pNewParent, bool bNoRelationship=true);
};

class GMainBaseNode :
	public GBaseNode
{
public:
	typedef GBaseNode super;
	virtual const char * getTypeName();		// Do not implement separately
	static const char * getStaticTypeName();		// Do not implement separately
public:
	static GMainBaseNode& getInstance() { static GMainBaseNode instance; return instance; }

private:
	GMainBaseNode();
	virtual ~GMainBaseNode();
	GMainBaseNode(GMainBaseNode const&);
	void operator=(GMainBaseNode const&);

public:

	bool CallReadXML(QXmlStreamReader &qsr);
	virtual GObject * CreateNewClone(GObject * pNewParent=NULL, GObject * pBeforeObj=NULL);

}; 

class GHistoryBaseNode : public GBaseNode
{
public:
	typedef GBaseNode super;
public:
	GHistoryBaseNode();
	virtual ~GHistoryBaseNode();

protected:
	virtual GObject * CreateNewClone(GObject * pNewParent=NULL, GObject * pBeforeObj=NULL);
	virtual bool CloneData(GObject * pClone, GObject * pNewParent, bool bNoRelationship=true);

	virtual void OnAddChildAfterObj(GObject * pChild, GObject * pAfter);
	virtual void OnRemoveChild(GObject * pChild, bool bRelease);

	void BuildRelationship(GObject * pObj);
	void RestoreRelationship(GObject * pObj);

	GNodeRelationshipGroup * FindRelationshipGroup( GObject * pObj, bool bRemove=true );

	//////////////////////////////////////////////////////////////////////////
	// This Base Cannot Clone
	list<GNodeRelationshipGroup *> lstRelations;
};