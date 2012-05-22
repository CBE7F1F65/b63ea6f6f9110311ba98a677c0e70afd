#pragma once


// UICommandBox

class UICommandBox : public CDockablePane
{
	DECLARE_DYNAMIC(UICommandBox)

public:
	UICommandBox();
	virtual ~UICommandBox();

protected:
	CRichEditCtrl m_wndCommandEdit;
	CRichEditCtrl m_wndCommandEditPrompt;
	CRichEditCtrl m_wndCommandLogEdit;

	CMFCTabCtrl	m_wndTabs;

	CListCtrl	m_wndList1;
	CListCtrl	m_wndList2;
	CListCtrl	m_wndList3;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	void AppendCommandLogText(LPCTSTR text);
};


