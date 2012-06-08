#pragma once


// UIHistorySnapshotListCtrl

class UIHistorySnapshotListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(UIHistorySnapshotListCtrl)

public:
	UIHistorySnapshotListCtrl();
	virtual ~UIHistorySnapshotListCtrl();

	void AddSnapshot();
	void DeleteSnapshot();

	int nSelected;
	int topIndex;

	int displayindexcount;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
};


