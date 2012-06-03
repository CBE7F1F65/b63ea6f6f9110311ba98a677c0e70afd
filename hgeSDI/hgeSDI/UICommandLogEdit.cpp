// UICommandLogEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "hgeSDI.h"
#include "UICommandLogEdit.h"

// UICommandLogEdit

IMPLEMENT_DYNAMIC(UICommandLogEdit, CEdit)

UICommandLogEdit::UICommandLogEdit()
{
}

UICommandLogEdit::~UICommandLogEdit()
{
}

BEGIN_MESSAGE_MAP(UICommandLogEdit, CEdit)
END_MESSAGE_MAP()

void UICommandLogEdit::AppendCommandLogText( LPCTSTR text, bool bNewLine/*=true*/ )
{
	CString str;
	GetWindowText(str);
	if (str.GetLength() && bNewLine)
	{
		str += "\r\n";
	}
	str += text;
	SetWindowText(str);
	KeepLogLength();
	PostMessage(WM_VSCROLL, SB_BOTTOM,0);
}

void UICommandLogEdit::ClearLog()
{
	SetWindowText("");
}

void UICommandLogEdit::KeepLogLength()
{
#define _UICOMMANDLOG_MAXLENGTH	0x10000
	int nlength = GetWindowTextLength();
	if (nlength > _UICOMMANDLOG_MAXLENGTH)
	{
		CString str;
		GetWindowText(str);
		int nnl = str.Find("\r\n");
		if (nnl >= 0)
		{
			str = str.Right(nlength-nnl-2);
			SetWindowText(str);
		}
	}
}
// UICommandLogEdit 消息处理程序

BOOL UICommandLogEdit::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	if (pMsg->message == WM_KEYDOWN && ::GetKeyState(VK_CONTROL))
	{
		switch (pMsg->wParam)
		{
		case 'Z':
//			Undo();
			return TRUE;
		case 'X':
//			Cut();
			Copy();
			return TRUE;
		case 'C':
			Copy();
			return TRUE;
		case 'V':
//			Paste();
			return TRUE;
		case 'A':
			SetSel(0, -1);
			return TRUE;
		}
	}

	return CEdit::PreTranslateMessage(pMsg);
}