#pragma once

#include "ColorManager.h"

#define GOBJ_DISPSTATE_NORMAL				0x00
#define GOBJ_DISPSTATE_INVISIBLE			0x01
#define GOBJ_DISPSTATE_LOCKED				0x02
#define GOBJ_DISPSTATE_RECINVISIBLE			0x10
#define GOBJ_DISPSTATE_RECLOCKED			0x20

#define _GOBJ_CLONE_PRE(TNAME)	\
	TNAME * _node = new TNAME();	\
	ASSERT(pNewParent);	\
	ASSERT(pNewParent != this);	\
	ASSERT(!(pNewParent->isDescendantOf(this)));	\
    *_node = *this;	\
    _node->SetCloning(true); \
	_node->Independ();

#define _GOBJ_CLONE_POST_NORET()	\
	pNewParent->AddChild(_node);	\
	for (list<GObject *>::reverse_iterator it=getChildren()->rbegin(); it!=getChildren()->rend(); ++it)	\
	{	\
		(*it)->Clone(_node);	\
    }   \
    _node->SetCloning(false);

#define _GOBJ_CLONE_POST()	\
	_GOBJ_CLONE_POST_NORET();	\
	return true;

#define GOBJTRYSTATE_MOVE_NULL			0x00
#define GOBJTRYSTATE_MOVE_REQUIREUPDATE	0x01
#define GOBJTRYSTATE_MOVE_AFTERUPDATE	0x02

class GPoint;
class GAnchorPoint;
class GHandlePoint;
class GLine;
class GStraightLine;
class GBezierLine;
class GLayer;
class GPiece;

class GObject
{
public:
	GObject(void);
	virtual ~GObject(void);

public:
	virtual bool Clone(GObject * pNewParent)=0;

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

	virtual void Independ();

	virtual GObject * FindNodeByID(int id);

private:
	int _ActualAddChildAfterObj(GObject * child, GObject * afterobj);
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

	// Should be derived:
	virtual bool canMove(){return false;};
public:
	virtual bool MoveTo(float newx, float newy, bool bTry, int moveActionID=-1){DASSERT(false); return false;};
	virtual bool CallMoveTo(float newx, float newy, bool bTry, int moveActionID=-1){return MoveTo(newx, newy, bTry, moveActionID);};
	virtual bool CallMoveByOffset(float xoffset, float yoffset, bool bTry, int moveActionID=-1){return CallMoveTo(getX()+xoffset, getY()+yoffset, bTry, moveActionID);};
	int nMoveActionID;
	int nUpdateMoveActionID;

	virtual bool isLayer(){return false;};
	virtual bool isLine(){return false;};
	virtual bool isPoint(){return false;};
	virtual bool isPiece(){return false;};
	virtual GLine * getLine(){return NULL;};
	virtual GPiece * getPiece(){return NULL;};
	virtual GObject * getEntity(){return this;};
	// A point represent the position
	virtual bool isRepresentablePoint(){return false;};
	virtual bool isRepresentableLine(){return false;};
	virtual bool isRepresentablePiece(){return false;};
	virtual float getX(){DASSERT(false); return 0;};
	virtual float getY(){DASSERT(false); return 0;};
	virtual bool isAttributeNode(){return false;};
	virtual bool isModifyParent(){return true;};
	virtual bool isSlaveToLine(){return false;};
	virtual bool isSlaveToPiece(){return false;};
	virtual bool isHandlePoint(){return false;};
	virtual bool isAnchorPoint(){return false;};
	virtual bool isMidPoint(){return false;};

	virtual bool canAttach(){return false;};
	virtual bool canBeMergedWith(){return false;};
	virtual bool canBeClingTo(){return false;};

	virtual GLayer * GetLayer(bool bIncludingSelf=true);
	virtual GLayer * GetContainerLayer(){return GetLayer(false);};
	virtual GObject * GetBase();
	virtual GObject * GetNonAttributeParentObj();
	// ToDo!!
	virtual bool isSelected(){return false;};

	virtual void OnInit();
	virtual void OnEnter();
	virtual void OnRelease();
	virtual void OnRemove();

    // Use Call for Recursive
    virtual void OnPrecisionChanged(float fPrecision);
	virtual void OnModify();
	virtual void OnClearModify();	// Post-Update
	virtual void OnUpdate();
    virtual void OnRender(int iHighlightLevel=0);

    virtual void CallPrecisionChanged(float fPrecision);
	virtual void CallModify();
    virtual void CallClearModify();
    virtual void CallRedrawModify();
	virtual void CallUpdate();
	virtual void CallRender(int iHighlightLevel=0);

	virtual void OnParentToggleDisplayVisible(bool toDisplayVisible);
	virtual void OnParentToggleDisplayLocked(bool toDisplayLock);

	virtual bool canRender();
	virtual bool canUpdate();

public:
	virtual void ToggleTryMoveState(bool bTry);
public:


	bool operator < (const GObject &right) const
	{
		return nID < right.nID;
	};

public:
	bool isModified(){return bModified;};
	int getModifyVersion(){return nModifyVersion;};
	GObject * getParent(){return pParent;};
	int getID(){return nID;};
	list<GObject *> * getChildren(){return &listChildren;};
	int getChildrenCount(){return listChildren.size();};

	GObject * getOlderSibling();
	GObject * getOlderSiblingForChild( GObject* child );
	GObject * getYoungerSibling();
	GObject * getYoungerSiblingForChild( GObject* child );

	bool isAncestorOf( GObject * pObj );
	bool isDescendantOf( GObject * pObj );

	GObject * getNewestChild(){if(!getChildrenCount()){ return NULL;} return listChildren.front();};
	GObject * getOldestChild(){if(!getChildrenCount()){ return NULL;} return listChildren.back();};
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
	void setDisplayName(const char * str)
	{
		if (strlen(str))
		{
			strDisplayName=str;
		}
	};

protected:
	int nNonAttributeChildrenCount;

	string strDisplayName;
	int nDisplayState;

	bool bDisplayFolded;
	LineColorSet lsLineColorSet;

	int nID;
	GObject * pParent;
	bool bModified;
	int nModifyVersion;

	int nTryState;
	float fTryMove_bx;
	float fTryMove_by;

	list<GObject*> listChildren;
protected:
    bool bCloning;
public:
    void SetCloning(bool bSet){bCloning = bSet;};

private:
	static GObject * pTreeBase;
protected:
	// Only by MainBaseNode
	void setTreeBase(GObject * pObj){pTreeBase=pObj;};
	GObject * getTreeBase(){return pTreeBase;};
};
