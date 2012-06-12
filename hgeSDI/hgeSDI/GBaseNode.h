#pragma once
#include "glayer.h"

class GBaseNode :
	public GLayer
{
public:
	GBaseNode(void);
	~GBaseNode(void);

	void CopyBaseFrom( GBaseNode * pFrom );
	void CopyBaseTo( GBaseNode * pTo );

	virtual bool Clone(GObject * pNewParent);
};

class GMainBaseNode :
	public GBaseNode
{
public:
	static GMainBaseNode& getInstance() { static GMainBaseNode instance; return instance; }

private:
	GMainBaseNode();
	~GMainBaseNode();
	GMainBaseNode(GMainBaseNode const&);
	void operator=(GMainBaseNode const&);

public:

	virtual bool Clone(GObject * pNewParent);

}; 

