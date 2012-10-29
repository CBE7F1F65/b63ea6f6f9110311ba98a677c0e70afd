#pragma once

#include "GObject.h"
#include "MainDependency.h"
#include "GLayer.h"

#include "GBaseNode.h"

enum{
	GMMATYPE_MOVE,
	GMMATYPE_ROTATE,
	GMMATYPE_FLIP,
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

enum{
	GOBJCOPY_POINT,
	GOBJCOPY_LINE,
};
class GObjectCopyInfo{

	friend class GLine;
public:
	GObjectCopyInfo(GPoint * pPoint, float x, float y){pObj=pPoint; type=GOBJCOPY_POINT; xb=x; yb=y;};
	GObjectCopyInfo(GLine * pLine, float _xb, float _yb, float _xe, float _ye)
	{ pObj=pLine; type = GOBJCOPY_LINE; xb = _xb; yb = _yb; xe = _xe; ye = _ye; xbh = _xb; ybh = _yb; xeh = _xe; yeh = _ye;	};
	GObjectCopyInfo(GLine * pLine, float _xb, float _yb, float _xe, float _ye, float _xbh, float _ybh, float _xeh, float _yeh)
	{ pObj=pLine; type = GOBJCOPY_LINE; xb = _xb; yb = _yb; xe = _xe; ye = _ye; xbh = _xbh; ybh = _ybh; xeh = _xeh; yeh = _yeh;	};

public:
	int type;
	GObject * pObj;
	float xb;
	float yb;
	float xe;
	float ye;
	float xbh;
	float ybh;
	float xeh;
	float yeh;
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

	bool Dump(list<GObject *>& lobjs);

	void OnTreeWillChange();
	void OnTreeChanged(GObject * changingbase, GObject * activeitem, bool bSetActiveLayer=true);
    void OnPrecisionChanged();

	bool ReadXML(QXmlStreamReader * pqsr);
	bool WriteXML(QXmlStreamWriter * pqsw);

	void AddNodeToDelete(GObject * pDeletedObj);
	/*
	void MoveToUnDoList(GObject * node);
	GObject * GetUnDoListFront();
	*/
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

	bool BeginClone();
	void PushClone(GObject * pOri, GObject * pCloned);
	void EndClone();

	bool IsTryMoving(){return bTryMove;};
	bool IsTryMoveBlocking(){return bTryMoveBlock;};

	void SetManualCloning(bool bSet){bManualCloning=bSet;};
	bool IsManualCloning(){return bManualCloning;};

	void SetLockTreeChange();

	void SetWillSelfMoveList(list<GObject *> * pobjs=NULL);
	bool WillSelfMove(GObject * pObj);

	bool AddCopyNode(GObject * pObj);
	void ClearCopiedNodes();
	bool PasteNodes();
	bool IsInCopyList(GObject * pObj);

	void SaveSelectState();
	void ReSelect(list<int> lstselect, int activelayerID);

public:
	GMainBaseNode * GetMainBaseNode(){return pBaseNode;};
	GBaseNode * GetFakeBaseNode(){return &fakebasenode;};
private:
	GMainBaseNode * pBaseNode;
	GObject * pLastToSetActiveNode;
	list<GObject*> nodetodelete;

	bool bCloning;
	map<GObject *, GObject *> mapCloneList;

	int nLockTreeChangeState;

//	GHistoryBaseNode undobasenode;
	GBaseNode fakebasenode;
	int stackedLayerIndex;
	string defaultLayerName;

	list<GObject *> * pwillselfmovelist;

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

	list<GObjectCopyInfo> lstcopy;

	int PushMoveNodeByOffsetForBatchCommand( GObject* pObj, float xoffset, float yoffset );
	void DoMoveNodeByOffsetBatch();
	void OnDeleteNode( GObject * pDeletedObj );

public:
	void SetHandleVisible( bool bSet );
	bool isHandleVisible(){return bHandleVisible;};

	int GetNextMoveActionID(int nMoveType, int nAngle=0, float fXVal=1.0f, float fYVal=1.0f);
	int GetMoveTypeInfo(int * pAngle=NULL, float * pScaleX=NULL, float * pScaleY=NULL);
private:
	bool bHandleVisible;

	list<MoveNodeByOffsetInfo> pushedmovenodebyoffset;

	HTARGET tarObjs;
    bool bRenderUILayerIndicators;
    bool bRedraw;

	bool bTryMove;
	bool bTryMoveBlock;

	bool bManualCloning;

	int nMoveActionType;
	int nMoveActionAngle;
	float fMoveActionXVal;
	float fMoveActionYVal;

	int nMoveActionID;
};
