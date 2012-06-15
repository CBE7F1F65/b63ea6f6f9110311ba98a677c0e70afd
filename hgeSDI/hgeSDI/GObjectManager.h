#pragma once

#include "GObject.h"
#include "MainDependency.h"
#include "GLayer.h"

#include "GBaseNode.h"

class GObjectManager
{
	friend class GObject;

public:
	static GObjectManager& getInstance() { static GObjectManager instance; return instance; }

private:
	GObjectManager();
	~GObjectManager();
	GObjectManager(GObjectManager const&);
	void operator=(GObjectManager const&);

public:

	void Init();
	void Release();
	void Update();
	void Render();
	void Delete();

	void OnTreeWillChange();
	void OnTreeChanged(GObject * changingbase, GObject * activeitem, bool bSetActiveLayer=true);

	void AddNodeToDelete(GObject * node);

	void MoveToUnDoList(GObject * node);
	GObject * GetUnDoListFront();

	bool CanDeleteItem( GObject * pObj );
	bool CanReparentItem( GObject * pObj, int newparentindex );
	bool CanDuplicateItem( GObject * pObj );

	GObject * FindObjectByID(int id);

	GLayer * NewLayer(GObject * node, const char * layername, int layerIndex=-1);
	GLayer * NewSubLayer(GObject * node, const char * layername, int layerIndex=-1);

	GMainBaseNode * pBaseNode;
private:
	GObject * pLastToSetActiveNode;
	list<GObject*> nodetodelete;

	int nLockTreeChangeState;
public:
	void SetLockTreeChange();

	GBaseNode undobasenode;
	int stackedLayerIndex;
	string defaultLayerName;
private:
	GLayer * pActiveLayer;

//	GLayer * pActiveLayer;
	GLayer * lastworkinglayer;
public:
	GLayer * getWorkingLayer(){return pActiveLayer;};
	GLayer * getLastWorkingLayer(){return lastworkinglayer;};
	GLayer * GetActiveLayerFromUI();
	void UpdateWorkingLayer(GLayer * pLayer=NULL, bool bothtoactive=false);

	GLayer * GetActiveLayer();
	void SetActiveLayer_Internal(GObject * pObj=NULL, bool bCallUI=true);
	const char * GetDefaultLayerName( int layerIndex=-1 );
	list<GObject*> * GetSelectedNodes();
	GLayer * GetDragDroppedLayerNode();
	GObject * GetDragDroppedAfterNode();
	list<GObject*> selectednodes;

	HTARGET tarObjs;
};