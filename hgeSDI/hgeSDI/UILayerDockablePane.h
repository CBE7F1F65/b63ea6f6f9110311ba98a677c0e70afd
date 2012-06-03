#pragma once

// UILayerDockablePane
#include "UILayerListCtrl.h"

#define IDLB_LISTCTRL	0

class UILayerDockablePane : public CDockablePane
{
	DECLARE_DYNAMIC(UILayerDockablePane)

public:
	UILayerDockablePane();
	virtual ~UILayerDockablePane();

	void RebuildTree(GObject * changebase);

	UILayerListCtrl m_wndListCtrl;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};