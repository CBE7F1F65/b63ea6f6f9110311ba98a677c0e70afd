#pragma once

#include "UIHistoryListCtrl.h"
#include "UIDockablePaneButton.h"
#include "UIHistorySnapShotListCtrl.h"

// UIHistoryDockablePane

#define IDHB_LISTCTRL			0
#define IDHB_SNAPSHOTLISTCTRL	1

class UIHistoryDockablePane : public CDockablePane
{
	DECLARE_DYNAMIC(UIHistoryDockablePane)

public:
	UIHistoryDockablePane();
	virtual ~UIHistoryDockablePane();

	bool AddHistory(const char * desc, const char * commandstr, int command);
	bool ChangeCurrentHistory(int step);
	bool ClearLaterHistory(int ndelete);
	bool ClearPreviousHistory(int ndelete);

	UIDockablePaneButton m_wndFakeButtonL;
	UIDockablePaneButton m_wndFakeButtonR;
	UIDockablePaneButton m_wndSnapshotButton;
	UIDockablePaneButton m_wndDeleteButton;

	void OnSnapShotButtonClicked();
	void OnDeleteButtonClicked();

	UIHistoryListCtrl m_wndListCtrl;
	UIHistorySnapshotListCtrl m_wndSnapshotListCtrl;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdateUIButtons(CCmdUI *pCmdUI);
};