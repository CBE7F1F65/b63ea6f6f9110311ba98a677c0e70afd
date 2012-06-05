#pragma once

// UIPopupMenu

class UIPopupMenu : public CMFCPopupMenu
{
	DECLARE_DYNAMIC(UIPopupMenu)

public:
	UIPopupMenu();
	virtual ~UIPopupMenu();

	void Hide(bool bForce=false);

protected:
	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
//	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};