#pragma once
#include "glayer.h"

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

