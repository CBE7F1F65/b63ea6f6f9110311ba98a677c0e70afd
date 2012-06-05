#pragma once

// UICommandLogEdit

class UICommandLogEdit : public CEdit
{
	DECLARE_DYNAMIC(UICommandLogEdit)

public:
	UICommandLogEdit();
	virtual ~UICommandLogEdit();

	void AppendCommandLogText(LPCTSTR text, bool bNewLine=true);
	void ClearLog();
	void KeepLogLength();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};