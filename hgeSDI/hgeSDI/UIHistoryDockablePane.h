#pragma once

#include "UIHistoryListCtrl.h"

// UIHistoryDockablePane

#define IDHB_LISTCTRL	0

class UIHistoryDockablePane : public CDockablePane
{
	DECLARE_DYNAMIC(UIHistoryDockablePane)

public:
	UIHistoryDockablePane();
	virtual ~UIHistoryDockablePane();

	bool AddHistory(const char * desc, const char * commandstr);
	bool ChangeCurrentHistory(int step);
	bool ClearLaterHistory(int ndelete);
	bool ClearPreviousHistory(int ndelete);

	UIHistoryListCtrl m_wndListCtrl;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


