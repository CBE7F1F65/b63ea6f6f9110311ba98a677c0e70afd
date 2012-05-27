#pragma once


// UICommandBox

#include "UICommandEdit.h"
#include "UICommandLogEdit.h"

#define IDCB_COMMANDEDIT		1
#define IDCB_COMMANDLOGEDIT		2
//#define IDCB_COMMANDEDITPROMPT	3
#define IDCB_TABSHOLDER			10
#define IDCB_TABSBEGIN			100

#define UICB_MAXABVIEW		0x10

class UICommandDockablePane : public CDockablePane
{
	DECLARE_DYNAMIC(UICommandDockablePane)

public:
	UICommandDockablePane();
	virtual ~UICommandDockablePane();

protected:
	/*
	CRichEditCtrl m_wndCommandEdit;
	CRichEditCtrl m_wndCommandEditPrompt;
	CRichEditCtrl m_wndCommandLogEdit;
	*/
	UICommandEdit m_wndCommandEdit;
	UICommandLogEdit m_wndCommandLogEdit;

	CMFCTabCtrl	m_wndTabs;
	CListCtrl listctrl[UICB_MAXABVIEW];

	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
//	afx_msg void OnEnChangeCommandEdit();
//	afx_msg void OnEnMsgFilterCommandEdit(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	void AppendCommandLogText(LPCTSTR text, bool bNewLine=true);
	void ClearLog();

	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnClose();
};


