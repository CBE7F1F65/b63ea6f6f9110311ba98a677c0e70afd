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
	GObject * GetUnDoListFront();

	bool CanDeleteItem( GObject * obj );

	GObject * FindObjectByID(int id);

	GLayer * NewLayer(GObject * node, const char * layername, int layerIndex=-1);
	GLayer * NewSubLayer(GObject * node, const char * layername, int layerIndex=-1);

	GLayer * pBaseNode;
	list<GObject*> nodetodelete;

	bool bReleasing;

	GObject undobasenode;
	int stackedLayerIndex;
	string defaultLayerName;

private:
	GLayer * workinglayer;
	GLayer * lastworkinglayer;
public:
	GLayer * getWorkingLayer(){return workinglayer;};
	GLayer * getLastWorkingLayer(){return lastworkinglayer;};
	void OnInternalActiveLayerSetDone();
	void UpdateWorkingLayer(GLayer * pLayer=NULL, bool bothtoactive=false);

	GLayer * GetActiveLayer();
	void SetActiveLayer_Internal(GObject * pObj=NULL);
	const char * GetDefaultLayerName( int layerIndex=-1 );
	list<GObject*> * GetSelectedNodes();
	list<GObject*> selectednodes;

	HTARGET tarObjs;
};