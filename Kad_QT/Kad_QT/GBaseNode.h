#pragma once
#include "glayer.h"
#include "GNodeRelationship.h"

class GBaseNode :
	public GLayer
{
public:
	typedef GLayer super;
public:
	GBaseNode(void);
	virtual ~GBaseNode(void);

	void CopyBaseFrom( GBaseNode * pFrom );
    void CopyBaseTo( GBaseNode * pTo );

	virtual GObject * CreateNewClone(GObject * pNewParent=NULL, GObject * pBeforeObj=NULL);
};

class GMainBaseNode :
	public GBaseNode
{
public:
	typedef GBaseNode super;
public:
	static GMainBaseNode& getInstance() { static GMainBaseNode instance; return instance; }

private:
	GMainBaseNode();
	virtual ~GMainBaseNode();
	GMainBaseNode(GMainBaseNode const&);
	void operator=(GMainBaseNode const&);

public:

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