#pragma once

class GObject
{
public:
	GObject(void);
	~GObject(void);

	virtual int AddChild(GObject * child);
protected:
	virtual void SetParent(GObject * parent);
public:

	virtual void SortChildren();

	virtual int RemoveAllChildren();

	virtual int RemoveChild(int ID);
	virtual int RemoveChild(GObject * child);
	virtual int RemoveFromParent();
protected:
	virtual int _RemoveChild(int ID, bool bRelease=true);
	virtual int _RemoveChild(GObject * child, bool bRelease=true);
	virtual int _RemoveFromParent(bool bRelease=true);
public:
	virtual int Reparent(GObject * newparent);

	virtual void SetID(int ID=-1);
	virtual void OnInit();
	virtual void OnEnter();
	virtual void OnModify();
	virtual void OnClearModify();	// Pre-Update
	virtual void OnUpdate();
	virtual void OnRender();
	virtual void OnRelease();

	virtual void SetMotifyParent(bool bToModify);

	bool operator < (const GObject &right) const
	{
		return ID < right.ID;
	};

	int ID;
	GObject * parent;
	bool bModified;
	bool bModifyParent;

	list<GObject*> children;
};

