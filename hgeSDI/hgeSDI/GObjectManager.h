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

	GObject * FindObjectByID(int id);

	GLayer * NewLayer(GObject * node, const char * layername, int layerIndex=-1);
	GLayer * NewSubLayer(GObject * node, const char * layername, int layerIndex=-1);

	GObject basenode;
	list<GObject*> nodetodelete;

	GObject undobasenode;
	int stackedLayerIndex;
	string defaultLayerName;

	GLayer * GetActiveLayer();
	void SetActiveLayer(GObject * pObj);
	const char * GetDefaultLayerName( int layerIndex=-1 );
	HTARGET tarObjs;
};