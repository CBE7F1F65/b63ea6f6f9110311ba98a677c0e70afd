#pragma once

#include "GObject.h"
#include "MainDependency.h"
#include "GLayer.h"

#include "GBaseNode.h"

class MoveNodeByOffsetInfo
{
public:
	MoveNodeByOffsetInfo(){ClearSet();};
	MoveNodeByOffsetInfo(GObject * _pObj, float _xoffset, float _yoffset)
	{
		ASSERT(pObj);
		pObj = _pObj;
		xoffset = _xoffset;
		yoffset = _yoffset;
	}
	~MoveNodeByOffsetInfo(){};

	void ClearSet()
	{
		pObj = NULL;
		xoffset = 0;
		yoffset = 9;
	};
	bool IsDescendantOf( GObject* _pObj )
	{
		ASSERT(pObj);
		ASSERT(_pObj);
		return pObj->isDescendantOf(_pObj);
	};
	bool IsSameOffset( float _xoffset, float _yoffset )
	{
//		if (_xoffset == xoffset && _yoffset == yoffset)
		if (fabsf(_xoffset-xoffset) < M_FLOATEPS && fabsf(_yoffset-yoffset)<M_FLOATEPS)
		{
			return true;
		}
		return false;
	};
	float GetXOffset(){return xoffset;};
	float GetYOffset(){return yoffset;};
	GObject * GetObj(){return pObj;};
private:
	GObject * pObj;
	float xoffset;
	float yoffset;
};

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

public:
	GMainBaseNode * GetMainBaseNode(){return pBaseNode;};
private:
	GMainBaseNode * pBaseNode;
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
	GLayer * pLastActiveLayer;
public:
	GLayer * GetActiveLayerFromUI();
	void UpdateActiveLayer(GLayer * pLayer=NULL, bool bothtoactive=false);

	GLayer * GetActiveLayer();
	GLayer * GetLastActiveLayer(){return pLastActiveLayer;};
	void SetActiveLayer_Internal(GObject * pObj=NULL, bool bCallUI=true);
	const char * GetDefaultLayerName( int layerIndex=-1 );
    list<GObject*> * GetSelectedNodes();
	GLayer * GetDragDroppedLayerNode();
	GObject * GetDragDroppedAfterNode();
//	list<GObject*> selectednodes;

	int PushMoveNodeByOffsetForBatchCommand( GObject* pObj, float xoffset, float yoffset );
	void DoMoveNodeByOffsetBatch();
	void OnDeleteNode( GObject * node );
	list<MoveNodeByOffsetInfo> pushedmovenodebyoffset;

	HTARGET tarObjs;
};
