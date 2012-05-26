// UIEditBox.cpp : ʵ���ļ�
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



// UIEditBox ��Ϣ�������




void UIFloatingEditBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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
	// TODO: �ڴ˴������Ϣ����������
}


void UIFloatingEditBox::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);

//	ClearCommand();
	// TODO: �ڴ˴������Ϣ����������
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

	// TODO:  �ڴ������ר�õĴ�������

	return 0;
}


void UIFloatingEditBox::OnEnChange()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CEdit::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int nlength = GetWindowTextLength();
	if (nlength)
	{
		CString str;
		CString strw = "w";
		GetWindowText(str);
		CDC * cdc = GetDC();
		if (cdc)
		{
			CSize size = cdc->GetTextExtent(str);
			CSize sizew = cdc->GetTextExtent(strw);
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
	// TODO: �ڴ˴������Ϣ����������
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
	// TODO: �ڴ����ר�ô����/����û���

	if (pMsg->message == WM_KEYDOWN && ::GetKeyState(VK_CONTROL))
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
