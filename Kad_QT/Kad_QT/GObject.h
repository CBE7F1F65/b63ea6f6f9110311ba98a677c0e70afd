#pragma once

#include "ColorManager.h"

#define GOBJ_DISPSTATE_NORMAL				0x00
#define GOBJ_DISPSTATE_INVISIBLE			0x01
#define GOBJ_DISPSTATE_LOCKED				0x02
#define GOBJ_DISPSTATE_RECINVISIBLE			0x10
#define GOBJ_DISPSTATE_RECLOCKED			0x20

#define _GOBJ_CLONE_PRE(TNAME)	\
	if (!pNewParent)	\
	{	\
		pNewParent = pParent; \
	}	\
	ASSERT(pNewParent != this);	\
	ASSERT(!(pNewParent->isDescendantOf(this)));	\
	TNAME * _node = new TNAME();	\
	_node->SetCloning(true); \
	GBaseNode tempHoldingPlace;	\
	CloneData(_node, &tempHoldingPlace);

#define _GOBJ_CLONE_POST_NORET()	\
	tempHoldingPlace.AddChild(_node);	\
	_node->SetCloning(false);	\
	_node->ReparentBeforeObject(pNewParent, pBeforeObj);	\
	_node->OnInit();

#define _GOBJ_CLONE_POST()	\
	_GOBJ_CLONE_POST_NORET();	\
	return _node;

#define GOBJTRYSTATE_MOVE_NULL			0x00
#define GOBJTRYSTATE_MOVE_REQUIREUPDATE	0x01
#define GOBJTRYSTATE_MOVE_AFTERUPDATE	0x02

#define GOBJM_COPYABLES()
#define GOBJM_COPYABLESEND()

#define GOBJM_NONCOPYABLES()
#define GOBJM_NONCOPYABLESEND()

#define GOBJM_DONOTCOPY()
#define GOBJM_DONOTCOPYEND()

#define GOBJM_CHILDPOINTERS()
#define GOBJM_CHILDPOINTERSEND()

#define GOBJM_NEEDINITIAL()

class GPoint;
class GAnchorPoint;
class GHandlePoint;
class GLine;
class GStraightLine;
class GBezierLine;
class GLayer;
class GPiece;

class GNodeRelationshipGroup;

class GObject
{
public:
	GObject(void);
	virtual ~GObject(void);

public:
	virtual GObject * CreateNewClone(GObject * pNewParent=NULL, GObject * pBeforeObj=NULL)=0;
protected:
	virtual bool CloneData(GObject * pClone, GObject * pNewParent, bool bNoRelationship=true);

public:
	virtual int AddChild(GObject * child);
private:
	virtual int AddChildAfterObj( GObject* child, GObject*afterobj );
public:

	virtual int RemoveAllChildren(bool bRelease);

	virtual int RemoveChild(int ID, bool bRelease);
	virtual int RemoveChild(GObject * child, bool bRelease);
	virtual int RemoveFromParent(bool bRelease);
	virtual void CallResetID(int beginindex=0);

	virtual GNodeRelationshipGroup * CreateRelationshipGroup(){return NULL;};
	virtual void Independ(){};

	virtual GObject * FindNodeByID(int id);

private:
	int _ActualAddChildAfterObj(GObject * child, GObject * afterobj);
	int _ReparentAfterObject( GObject * newparent, GObject * afterobj );
	void _SetParent(GObject * parent);
	list<GObject *>::iterator _ActualRemoveChild(list<GObject *>::iterator it, bool bRelease);
	int _RemoveChild(int ID, bool bRelease);
	int _RemoveChild(GObject * child, bool bRelease);
	int _RemoveFromParent(bool bRelease);
	void _SetID(int ID=-1);
	void _CallTreeChanged(GObject * changebase, GObject * activenode);
	void _CallTreeWillChange();
	int _CallResetID(int resetbase);

	void _ModifyNonAttributeChildrenCount(int countchange);

public:
	virtual int Reparent(GObject * newparent);
	virtual int ReparentAfterObject( GObject * newparent, GObject * afterobj );
	virtual int ReparentBeforeObject( GObject * newparent, GObject * beforeobj );

	// Should be derived:
	virtual bool canMove(){return false;};
protected:
	virtual bool MoveTo(GObject * pCaller, float newx, float newy, bool bTry, int moveActionID=-1){DASSERT(false); return false;};
public:
	virtual bool CallMoveTo(GObject * pCaller, float newx, float newy, bool bTry, int moveActionID=-1){DASSERT(false); return false;};//{return MoveTo(pCaller, newx, newy, bTry, moveActionID);};
	virtual bool CallMoveByOffset(GObject * pCaller, float xoffset, float yoffset, bool bTry, int moveActionID=-1){return CallMoveTo(pCaller, getX()+xoffset, getY()+yoffset, bTry, moveActionID);};
	virtual bool CallRotate(GObject * pCaller, float orix, float oriy, int angle, bool bTry, int moveActionID=-1){DASSERT(false); return false;};
	virtual bool CallScale(GObject * pCaller, float orix, float oriy, float fScaleX, float fScaleY, bool bTry, int moveActionID=-1){DASSERT(false); return false;};
	virtual bool CallFlip(GObject * pCaller, float orix, float oriy, int angle, bool bTry, int moveActionID=-1){DASSERT(false); return false;};

	virtual bool isLayer(){return false;};
	virtual bool isLine(){return false;};
	virtual bool isPoint(){return false;};
	virtual bool isHandlePoint(){return false;};
	virtual bool isAnchorPoint(){return false;};
	virtual bool isMidPoint(){return false;};
	virtual bool isPiece(){return false;};
	virtual bool isRepresentablePoint(){return false;};		// A point represent the position
	virtual bool isRepresentableLine(){return false;};
	virtual bool isRepresentablePiece(){return false;};
	virtual bool isAttributeNode(){return false;};
	virtual bool isModifyParent(){return true;};
	virtual bool isSlaveToLine(){return false;};
	virtual bool isSlaveToPiece(){return false;};
	virtual bool isSelected(){return false;};	// ToDo!!

	virtual float getX(){DASSERT(false); return 0;};
	virtual float getY(){DASSERT(false); return 0;};
	virtual GLine * getLine(){return NULL;};
	virtual GPiece * getPiece(){return NULL;};
	virtual GObject * getEntity(){return this;};
	virtual GLayer * getLayer(bool bIncludingSelf=true);
	virtual GLayer * getContainerLayer(){return getLayer(false);};
	virtual GObject * getBase();
	virtual GObject * getNonAttributeParentObj();

	virtual bool canAttach(){return false;};
	virtual bool canBeMergedWith(){return false;};
	virtual bool canBeClingTo(){return false;};
	virtual bool canRender();
	virtual bool canUpdate();

protected:
	virtual void OnInit();
	virtual void OnEnter();
	virtual void OnRelease();
	virtual void OnRemove();
    virtual void OnPrecisionChanged(float fPrecision);
	virtual void OnModify();
	virtual void OnClearModify();	// Post-Update
	virtual void OnCancelTryMove();
	virtual void OnUpdate();
    virtual void OnRender(int iHighlightLevel=0);
	virtual void OnParentToggleDisplayVisible(bool toDisplayVisible);
	virtual void OnParentToggleDisplayLocked(bool toDisplayLock);

	virtual void OnAddChildAfterObj(GObject * pChild, GObject * pAfter){};
	virtual void OnRemoveChild(GObject * pChild, bool bRelease){};

public:
    virtual void CallPrecisionChanged(float fPrecision);
	virtual void CallModify();
    virtual void CallClearModify();
    virtual void CallRedrawModify();
	virtual void CallUpdate();
	virtual void CallRender(int iHighlightLevel=0);
	virtual void CallCancelTryMove();

protected:
	virtual void ToggleTryMoveState(bool bTry);
public:
	void SetCloning(bool bSet){bCloning = bSet;};
	bool operator < (const GObject &right) const { return nID < right.nID; };

public:
	GObject * getParent(){return pParent;};
	int getID(){return nID;};
	list<GObject *> * getChildren(){return &lstChildren;};
	int getChildrenCount(){return lstChildren.size();};

	bool isModified(){return bModified;};
	int getModifyVersion(){return nModifyVersion;};

	GObject * getOlderSibling();
	GObject * getOlderSiblingForChild( GObject* child );
	GObject * getYoungerSibling();
	GObject * getYoungerSiblingForChild( GObject* child );

	bool isAncestorOf( GObject * pObj );
	bool isDescendantOf( GObject * pObj );

	GObject * getNewestChild(){if(!getChildrenCount()){ return NULL;} return lstChildren.front();};
	GObject * getOldestChild(){if(!getChildrenCount()){ return NULL;} return lstChildren.back();};
	int getNonAttributeChildrenCount(){return nNonAttributeChildrenCount;};

	bool isRecDisplayFolded();
	
	bool isDisplayFolded(){return bDisplayFolded;};
	void setDisplayFold(bool toDisplayFold);

	bool isDisplayVisible();
	// Inc does not mean it is actually visible
	bool isRecDisplayVisible();
	// Can only be called by UI
	void setDisplayVisible(bool toDisplayVisible);

	bool isDisplayLocked();
	// Inc does not mean it is actually visible
	bool isRecDisplayLocked();
	// Can only be called by UI
	void setDisplayLock(bool toDisplayLock);

    DWORD getLineColor(int iHighlightLevel=0);
    LineColorSet getLineColorSet(){return lsLineColorSet;};

	// Can only be called by Layer
	virtual void setLineColor(LineColorSet ls);
	virtual void clearLineColor();

	virtual const char * getDisplayName();
	void setDisplayName(const char * str) { if (strlen(str)) { strDisplayName=str; } };

	int getMoveActionID(){return nMoveActionID;};

protected:
	/************************************************************************/
	/* Members                                                              */
	/************************************************************************/

	//////////////////////////////////////////////////////////////////////////
	GOBJM_COPYABLES();

	string strDisplayName;
	LineColorSet lsLineColorSet;

	int nMoveActionID;
	int nUpdateMoveActionID;
	int nNonAttributeChildrenCount;
	int nModifyVersion;

	int nTryState;
	float fTryMove_bx;
	float fTryMove_by;

	int nDisplayState;
	bool bDisplayFolded;
	bool bModified;

	GOBJM_COPYABLESEND();

	//////////////////////////////////////////////////////////////////////////
	GOBJM_NONCOPYABLES();

	list<GObject*> lstChildren;

//	GOBJ_NONCOPYABLESEND(GObject);

	//////////////////////////////////////////////////////////////////////////
	GOBJM_DONOTCOPY();
	int nID;
	GObject * pParent;
	bool bCloning;
	GOBJM_DONOTCOPYEND();
	/************************************************************************/
	/* Static Functions and Members                                         */
	/************************************************************************/
private:
	static GObject * pTreeBase;
protected:
	// Only by MainBaseNode
	void setTreeBase(GObject * pObj){pTreeBase=pObj;};
	GObject * getTreeBase(){return pTreeBase;};
};
