#pragma once

// UIEditBox

class UIFloatingEditBox : public CEdit
{
	DECLARE_DYNAMIC(UIFloatingEditBox)

public:
	UIFloatingEditBox();
	virtual ~UIFloatingEditBox();

	void ClearCommand();
	void CommitCommand();
	void Show(int vk=0);
	void Hide(bool bForce=false);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnEnChange();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};