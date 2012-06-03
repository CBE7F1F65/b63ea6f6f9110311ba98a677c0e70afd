#pragma once

// UIHistoryListCtrl
#include "Command.h"

class UIHistoryListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(UIHistoryListCtrl)

public:
	UIHistoryListCtrl();
	virtual ~UIHistoryListCtrl();

	bool AddHistory(const char * desc, const char * commandstr, int command);
	bool ChangeCurrentHistory(int step);
	bool ClearLaterHistory(int ndelete);
	bool ClearPreviousHistory(int ndelete);

	int nSelected;
	int topIndex;

protected:
	CFont m_cfont;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
//	afx_msg void OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
};