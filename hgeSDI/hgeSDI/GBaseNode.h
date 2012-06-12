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
	static GMainBaseNode& getInstance()
	{
		static GMainBaseNode instance;
		// Guaranteed to be destroyed. Instantiated on first use.
		return instance;
	}

private:
	GMainBaseNode();
	~GMainBaseNode();
	// Don't forget to declare these two.
	GMainBaseNode(GMainBaseNode const&);
	// Don't Implement
	void operator=(GMainBaseNode const&);
	// Don't implement

public:

	virtual bool Clone(GObject * pNewParent);

}; 

