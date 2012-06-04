#pragma once

#include "GObject.h"
#include "MainDependency.h"
#include "GLayer.h"

class GObjectManager
{
	friend class GObject;

public:
	static GObjectManager& getInstance()
	{
		static GObjectManager instance;
		// Guaranteed to be destroyed. Instantiated on first use.
		return instance;
	}

private:
	GObjectManager();
	~GObjectManager();
	// Don't forget to declare these two.
	GObjectManager(GObjectManager const&);
	// Don't Implement
	void operator=(GObjectManager const&);
	// Don't implement

public:

	void Init();
	void Release();
	void Update();
	void Render();
	void Delete();

	void OnTreeChanged(GObject * changingbase, GObject * activeitem);

	void AddNodeToDelete(GObject * node);

	void MoveToUnDoList(GObject * node);

	GLayer * NewLayer(GObject * node, const char * layername);
	GLayer * NewSubLayer(GObject * node, const char * layername);

	GObject basenode;
	list<GObject*> nodetodelete;

	GObject undobasenode;
	int layerIndex;
	string defaultLayerName;

	GLayer * GetActiveLayer();
	const char * GetDefaultLayerName( int layerIndex=-1 );
	HTARGET tarObjs;
};