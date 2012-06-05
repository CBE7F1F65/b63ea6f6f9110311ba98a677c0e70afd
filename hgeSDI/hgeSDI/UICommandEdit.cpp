// UICommandEdit.cpp : 实现文件
//

#include "stdafx.h"
#include "hgeSDI.h"
#include "UICommandEdit.h"

// UICommandEdit

IMPLEMENT_DYNAMIC(UICommandEdit, CEdit)

UICommandEdit::UICommandEdit()
{
}

UICommandEdit::~UICommandEdit()
{
}

BEGIN_MESSAGE_MAP(UICommandEdit, CEdit)
	ON_CONTROL_REFLECT(EN_CHANGE, &UICommandEdit::OnEnChange)
	ON_WM_CHAR()
END_MESSAGE_MAP()

#include "Main.h"

void UICommandEdit::CommitCommand()
{
	int nlength = GetWindowTextLength();
	if (!nlength)
	{
		return;
	}
	CString str;
	GetWindowText(str);
	MainInterface::getInstance().OnCommitCommand(str.GetBuffer());
}

void UICommandEdit::ClearCommand()
{
	SetWindowText("");
}
void UICommandEdit::SetCommandText( LPCTSTR text, bool bActivate/*=true*/ )
{
	CString str;
	GetWindowText(str);
	str += " ";
	str += text;
	SetWindowText(str.GetBuffer());
	SetSel(0, -1);
	SetSel(-1);
	if (bActivate)
	{
		SetFocus();
	}
}

// UICommandEdit 消息处理程序

void UICommandEdit::OnEnChange()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CEdit::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

void UICommandEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CEdit::OnChar(nChar, nRepCnt, nFlags);
	if (nChar == VK_ESCAPE)
	{
		ClearCommand();
		return;
	}
	else if (nChar == VK_RETURN)
	{
		CommitCommand();
		ClearCommand();
		return;
	}
}

BOOL UICommandEdit::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类if (pMsg-&gt;message == WM_KEYDOWN &amp;&amp; ::GetKeyState(VK_CONTROL) &lt; 0)
	if (pMsg->message == WM_KEYDOWN && (GetKeyState(VK_CONTROL)&0x8000))
	{
		switch (pMsg->wParam)
		{
		case 'Z':
			if (CanUndo())
			{
				Undo();
			}
			return TRUE;
		case 'X':
			Cut();
			return TRUE;
		case 'C':
			Copy();
			return TRUE;
		case 'V':
			Paste();
			return TRUE;
		case 'A':
			SetSel(0, -1);
			return TRUE;
		}
	}

	return CEdit::PreTranslateMessage(pMsg);
}