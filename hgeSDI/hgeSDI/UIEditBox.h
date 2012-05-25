#pragma once


// UIEditBox

class UIEditBox : public CEdit
{
	DECLARE_DYNAMIC(UIEditBox)

public:
	UIEditBox();
	virtual ~UIEditBox();

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
};


