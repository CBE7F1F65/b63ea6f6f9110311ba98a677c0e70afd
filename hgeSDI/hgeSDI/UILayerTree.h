#pragma once


// UILayerTree
#include "GObjectManager.h"

class UILayerTree : public CTreeCtrl
{
	DECLARE_DYNAMIC(UILayerTree)

public:
	UILayerTree();
	virtual ~UILayerTree();

	void RebuildTree(GObject * changebase);
	void BuildChildren(GObject * nowbase, HTREEITEM nowht, GObject * changebase);

	HTREEITEM htNowEdit;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


