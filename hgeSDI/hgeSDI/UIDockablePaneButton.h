#pragma once


// UIDockablePaneButton


class UIDockablePaneButton : public CButton
{
	DECLARE_DYNAMIC(UIDockablePaneButton)

public:
	UIDockablePaneButton();
	virtual ~UIDockablePaneButton();

	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);

	CToolTipCtrl* m_pToolTip;

	HICON hIconNormal;
	HICON hIconDisabled;
	bool bEnabled;

public:
	virtual BOOL DCreate(int id, CWnd * pParent, const char * tooltip="", int icmsize=-1, int img=-1, int imgdisabled=-1);
	virtual void SetButtonEnable(bool bEnable=true);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
//	afx_msg void OnBnClicked();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};


