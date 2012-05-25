#pragma once

#define IDFC_COMMANDEDIT	1
#define IDFC_COMMANDEDIT2	2
// UIFloatingCommand

#include "UIEditBox.h"

class UIFloatingCommand : public CPane
{
	DECLARE_DYNAMIC(UIFloatingCommand)

public:
	UIFloatingCommand();
	virtual ~UIFloatingCommand();

	UIEditBox m_wndCommandEdit;
	CRichEditCtrl m_wndCommandEdit2;

	/*
	void ClearCommand();
	void CommitCommand();
	*/
	void Show(int vk=0);
	void Hide(bool bForce=false);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEnMsgFilterCommandEdit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnMsgFilterCommandEdit2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnEnRequestResizeCommandEdit(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnClose();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};


