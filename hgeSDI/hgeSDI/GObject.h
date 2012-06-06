#pragma once

#define GOBJ_DISPSTATE_NORMAL				0x00
#define GOBJ_DISPSTATE_INVISIBLE			0x01
#define GOBJ_DISPSTATE_LOCKED				0x02
#define GOBJ_DISPSTATE_RECINVISIBLE			0x10
#define GOBJ_DISPSTATE_RECLOCKED			0x20

class GObject
{
public:
	GObject(void);
	~GObject(void);

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
	int _CallResetID(int resetbase);

	void _ModifyNonAttributeChildrenCount(int countchange);

public:
	virtual int Reparent(GObject * newparent);
	virtual int ReparentAfterObject( GObject * newparent, GObject * afterobj );

	virtual bool isLayer(){return false;};
	virtual GObject * GetLayer();
	virtual GObject * GetBase();
	// ToDo!!
	virtual bool isSelected(){return false;};

	virtual void OnInit();
	virtual void OnEnter();
	virtual void OnRelease();

	// Use Call for Recursive
	virtual void OnModify();
	virtual void OnClearModify();	// Post-Update
	virtual void OnUpdate();
	virtual void OnRender();

	virtual void CallModify();
	virtual void CallClearModify();
	virtual void CallUpdate();
	virtual void CallRender();

	virtual void OnParentToggleDisplayVisible(bool toDisplayVisible);
	virtual void OnParentToggleDisplayLocked(bool toDisplayLock);

	virtual bool canRender();
	virtual bool canUpdate();


	bool operator < (const GObject &right) const
	{
		return nID < right.nID;
	};

public:
	void setMotifyParent(bool bToModify);
	bool isModified(){return bModified;};
	GObject * getParent(){return pParent;};
	int getID(){return nID;};
	list<GObject *> * getChildren(){return &listChildren;};
	int getChildrenCount(){return listChildren.size();};
	GObject * getNewestChild(){if(!getChildrenCount()){ return NULL;} return listChildren.front();};
	GObject * getOldestChild(){if(!getChildrenCount()){ return NULL;} return listChildren.back();};
	int getNonAttributeChildrenCount(){return nNonAttributeChildrenCount;};
	bool isAttributeNode(){return bAttributeNode;};

	bool isRecDisplayFolded();

	void setIsAttributeNode(bool bToAttribute=true);

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

	DWORD getLineColor(){return dwLineColor;};

	// Can only be called by Layer
	virtual void setLineColor(DWORD col);

	virtual const char * getDisplayName();
	void setDisplayName(const char * str)
	{
		if (strlen(str))
		{
			strDisplayName=str;
		}
	};
protected:
	bool bAttributeNode;
	int nNonAttributeChildrenCount;

	string strDisplayName;
	int nDisplayState;

	bool bDisplayFolded;
	DWORD dwLineColor;

	int nID;
	GObject * pParent;
	bool bModified;
	bool bModifyParent;

	list<GObject*> listChildren;
};