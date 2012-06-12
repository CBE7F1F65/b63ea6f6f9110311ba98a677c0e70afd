#pragma once

// UILayerDockablePane
#include "UILayerListCtrl.h"
#include "GLayer.h"
#include "UIDockablePaneButton.h"

#define IDLB_LISTCTRL	0

class UILayerDockablePane : public CDockablePane
{
	DECLARE_DYNAMIC(UILayerDockablePane)

public:
	UILayerDockablePane();
	virtual ~UILayerDockablePane();

	void LockTreeChange( bool toLock );
	void CallRebuildTree(GObject * changebase, GObject * activeitem);
	void RebuildTree(GObject * changebase, GObject * activeitem);
	GLayer * GetActiveLayer();
	GObject * GetActiveNodes( int * pnextfromIndex );
	bool GetDragDropNodes( GLayer ** pLayerNode, GObject ** pAfterNode );
	void SetActiveLayer_Internal( GLayer * pLayer );

	UILayerListCtrl m_wndListCtrl;

	UIDockablePaneButton m_wndFakeButtonL;
	UIDockablePaneButton m_wndFakeButtonR;
	UIDockablePaneButton m_wndNewLayerButton;
	UIDockablePaneButton m_wndNewSubLayerButton;
	UIDockablePaneButton m_wndDeleteItemButton;

	void OnNewLayerButtonClicked();
	void OnNewSubLayerButtonClicked();
	void OnDeleteItemButtonClicked();

	void CallReparentSelectedNodes( int op );

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdateUIButtons(CCmdUI *pCmdUI);
};