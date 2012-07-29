#pragma once

#include "GObject.h"
#include "MainDependency.h"
#include "GLayer.h"

#include "GBaseNode.h"

enum{
	GMMATYPE_MOVE,
	GMMATYPE_ROTATE,
	GMMATYPE_SCALE,
};

class MoveNodeByOffsetInfo
{
public:
	MoveNodeByOffsetInfo(){ClearSet();};
	MoveNodeByOffsetInfo(GObject * _pObj, float _xoffset, float _yoffset)
	{
		ASSERT(_pObj);
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
	void RenderIndication();
	void Delete();

	void OnTreeWillChange();
	void OnTreeChanged(GObject * changingbase, GObject * activeitem, bool bSetActiveLayer=true);
    void OnPrecisionChanged();

	void AddNodeToDelete(GObject * pDeletedObj);

	void MoveToUnDoList(GObject * node);
	GObject * GetUnDoListFront();

	bool CanDeleteItem( GObject * pObj );
	bool CanReparentItem( GObject * pObj, int newparentindex );
	bool CanDuplicateItem( GObject * pObj );

    void SetRedraw(){bRedraw = true;};
    void SetRenderUILayerIndicators(bool bSet){bRenderUILayerIndicators=bSet;};

	GObject * FindObjectByID(int id);

	GLayer * NewLayer(GObject * node, const char * layername, int layerIndex=-1);
	GLayer * NewSubLayer(GObject * node, const char * layername, int layerIndex=-1);

	void BeginTryMove();
	void EndTryMove();
	void BlockTryMove();
	void UnblockTryMove();
	void CancelTryMove();

	bool IsTryMoving(){return bTryMove;};
	bool IsTryMoveBlocking(){return bTryMoveBlock;};

public:
	GMainBaseNode * GetMainBaseNode(){return pBaseNode;};
private:
	GMainBaseNode * pBaseNode;
	GObject * pLastToSetActiveNode;
	list<GObject*> nodetodelete;

	int nLockTreeChangeState;
public:
	void SetLockTreeChange();

	GHistoryBaseNode undobasenode;
	GBaseNode fakebasenode;
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
	list<GObject*> * GetSelectedNodes(bool bFromUI);
	GLayer * GetDragDroppedLayerNode();
	GObject * GetDragDroppedAfterNode();
//	list<GObject*> selectednodes;

	int PushMoveNodeByOffsetForBatchCommand( GObject* pObj, float xoffset, float yoffset );
	void DoMoveNodeByOffsetBatch();
	void OnDeleteNode( GObject * pDeletedObj );

public:
	void SetHandleVisible( bool bSet );
	bool isHandleVisible(){return bHandleVisible;};

	int GetNextMoveActionID(int nMoveType, int nAngle=0, float fScaleX=1.0f, float fScaleY=1.0f);
	int GetMoveTypeInfo(int * pAngle=NULL, float * pScaleX=NULL, float * pScaleY=NULL);
private:
	bool bHandleVisible;

	list<MoveNodeByOffsetInfo> pushedmovenodebyoffset;

	HTARGET tarObjs;
    bool bRenderUILayerIndicators;
    bool bRedraw;

	bool bTryMove;
	bool bTryMoveBlock;

	int nMoveActionType;
	int nMoveActionAngle;
	float fMoveActionScaleX;
	float fMoveActionScaleY;

	int nMoveActionID;
};
