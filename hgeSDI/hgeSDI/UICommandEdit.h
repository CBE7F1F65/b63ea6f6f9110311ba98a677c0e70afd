#pragma once

// UICommandEdit

class UICommandEdit : public CEdit
{
	DECLARE_DYNAMIC(UICommandEdit)

public:
	UICommandEdit();
	virtual ~UICommandEdit();

	void CommitCommand();
	void ClearCommand();
	void SetCommandText(LPCTSTR text, bool bActivate=true);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnEnChange();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};