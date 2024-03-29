// UIEditBox.cpp : 实现文件
//

#include "stdafx.h"
#include "hgeSDI.h"
#include "UIFloatingEditBox.h"

#include "Main.h"

// UIEditBox

IMPLEMENT_DYNAMIC(UIFloatingEditBox, CEdit)

UIFloatingEditBox::UIFloatingEditBox()
{
}

UIFloatingEditBox::~UIFloatingEditBox()
{
}

BEGIN_MESSAGE_MAP(UIFloatingEditBox, CEdit)
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_CREATE()
	ON_CONTROL_REFLECT(EN_CHANGE, &UIFloatingEditBox::OnEnChange)
END_MESSAGE_MAP()

// UIEditBox 消息处理程序

void UIFloatingEditBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nChar == VK_ESCAPE)
	{
		GetParent()->PostMessage(WM_CHAR, nChar);
		ClearCommand();
		Hide();
		return;
	}
	else if (nChar == VK_RETURN)
	{
		CommitCommand();
		GetParent()->PostMessage(WM_CHAR, nChar);
		ClearCommand();
		Hide();
		return;
	}

	CWnd::OnChar(nChar, nRepCnt, nFlags);
}

void UIFloatingEditBox::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);

	Hide();
	// TODO: 在此处添加消息处理程序代码
}

void UIFloatingEditBox::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);

//	ClearCommand();
	// TODO: 在此处添加消息处理程序代码
}

void UIFloatingEditBox::ClearCommand()
{
	SetWindowText("");
}

void UIFloatingEditBox::CommitCommand()
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

int UIFloatingEditBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	return 0;
}

void UIFloatingEditBox::OnEnChange()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CEdit::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	int nlength = GetWindowTextLength();
	if (nlength)
	{
		CString str;
		CString strm = "mm";
		GetWindowText(str);
		CDC * cdc = GetDC();
		if (cdc)
		{
			CSize size = cdc->GetTextExtent(str);
			CSize sizew = cdc->GetTextExtent(strm);
			float cx = size.cx+sizew.cx;
			SetWindowPos(NULL, -1, -1, cx, size.cy, SWP_NOMOVE|SWP_DRAWFRAME);
		}
	}
}

void UIFloatingEditBox::Show( int vk/*=0*/ )
{
	POINT pt;
	GetCursorPos(&pt);
	AfxGetMainWnd()->ScreenToClient(&pt);

#define _UIFC_MOUSEOFFSET	15
#define _UIFC_EDITWIDTH		60
#define _UIFC_EDITHEIGHT	20
#define _UIFC_EDITSPACE		0

//	SetWindowPos(NULL, pt.x+_UIFC_MOUSEOFFSET, pt.y+_UIFC_MOUSEOFFSET, _UIFC_EDITWIDTH*2+_UIFC_EDITSPACE, _UIFC_EDITHEIGHT, SWP_DRAWFRAME);
	// TODO: 在此处添加消息处理程序代码
	float xbegin = pt.x+_UIFC_MOUSEOFFSET;
	float ybegin = pt.y+_UIFC_MOUSEOFFSET;
	SetWindowPos(NULL, xbegin, ybegin, _UIFC_EDITWIDTH, _UIFC_EDITHEIGHT, SWP_DRAWFRAME);
	ShowWindow(SW_SHOW);

	MainInterface::getInstance().SetMainViewActive(false, MVINACTIVEREASON_FLOATINGCOMMAND);

	ClearCommand();
	SetFocus();
	if (vk)
	{
		PostMessage(WM_KEYDOWN, vk, 0);
	}
}

void UIFloatingEditBox::Hide(bool bForce)
{
	//	ClearCommand();
	ShowWindow(SW_HIDE);
	if (!bForce)
	{
		MainInterface::getInstance().SetMainViewActive(true, MVINACTIVEREASON_FLOATINGCOMMAND);
	}
}

BOOL UIFloatingEditBox::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
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