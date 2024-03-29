#pragma once
#include "GObjectPicker.h"

class PickFilterTemplate
{
public:
	PickFilterTemplate(void);
	~PickFilterTemplate(void);

protected:
	virtual PickFilterCallback Use();
public:
	virtual void Dispose();

	PickFilterCallback pfcb;
	bool bInUse;
};

class PickFilterSingleObj : public PickFilterTemplate
{
public:
	static PickFilterSingleObj& getInstance() { static PickFilterSingleObj instance; return instance; }

private:
	PickFilterSingleObj();
	~PickFilterSingleObj();
	PickFilterSingleObj(PickFilterSingleObj const&);
	void operator=(PickFilterSingleObj const&);

public:
	PickFilterCallback Use(GObject * pObj, bool bRec=true);
	GObject * pFilterObj;
	bool bRecFind;

	static bool staticPickFilterCallback(GObject * pObj);
	bool FilterFunc(GObject * pObj);
}; 

class PickFilterSinglePoint : public PickFilterTemplate
{
public:
	static PickFilterSinglePoint& getInstance() { static PickFilterSinglePoint instance; return instance; }

private:
	PickFilterSinglePoint();
	~PickFilterSinglePoint();
	PickFilterSinglePoint(PickFilterSinglePoint const&);
	void operator=(PickFilterSinglePoint const&);

public:
	
	PickFilterCallback Use(GPoint * pPoint, bool bEntity=true, bool bMerge=true, bool bRec=true);
	GPoint * pFilterPoint;
	bool bMergeFind;
	bool bRecFind;
	bool bEntityFind;

	static bool staticPickFilterCallback(GObject * pObj);
	bool FilterFunc(GObject * pObj);
}; 

class PickFilterOnLine : public PickFilterTemplate
{
public:
	static PickFilterOnLine& getInstance() { static PickFilterOnLine instance; return instance; }

private:
	PickFilterOnLine();
	~PickFilterOnLine();
	PickFilterOnLine(PickFilterOnLine const&);
	void operator=(PickFilterOnLine const&);

public:
	PickFilterCallback Use();

	static bool staticPickFilterCallback(GObject * pObj);
	bool FilterFunc(GObject * pObj);
}; 