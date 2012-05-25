// UIEditBox.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "hgeSDI.h"
#include "UIEditBox.h"

#include "Main.h"

// UIEditBox

IMPLEMENT_DYNAMIC(UIEditBox, CWnd)

UIEditBox::UIEditBox()
{

}

UIEditBox::~UIEditBox()
{
}


BEGIN_MESSAGE_MAP(UIEditBox, CEdit)
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_CREATE()
	ON_CONTROL_REFLECT(EN_CHANGE, &UIEditBox::OnEnChange)
END_MESSAGE_MAP()



// UIEditBox ��Ϣ�������




void UIEditBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
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


void UIEditBox::OnKillFocus(CWnd* pNewWnd)
{
	CWnd::OnKillFocus(pNewWnd);

	Hide();
	// TODO: �ڴ˴������Ϣ����������
}


void UIEditBox::OnSetFocus(CWnd* pOldWnd)
{
	CWnd::OnSetFocus(pOldWnd);

//	ClearCommand();
	// TODO: �ڴ˴������Ϣ����������
}

void UIEditBox::ClearCommand()
{
	SetWindowText("");
}

void UIEditBox::CommitCommand()
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

int UIEditBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������

	return 0;
}


void UIEditBox::OnEnChange()
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

void UIEditBox::Show( int vk/*=0*/ )
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

void UIEditBox::Hide(bool bForce)
{
	//	ClearCommand();
	ShowWindow(SW_HIDE);
	if (!bForce)
	{
		MainInterface::getInstance().SetMainViewActive(true, MVINACTIVEREASON_FLOATINGCOMMAND);
	}
}