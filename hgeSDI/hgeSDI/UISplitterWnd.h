#pragma once



// UISplitterWnd

class UISplitterWnd : public CSplitterWnd
{
	DECLARE_DYNCREATE(UISplitterWnd)

public:
	UISplitterWnd();
	virtual ~UISplitterWnd();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};


