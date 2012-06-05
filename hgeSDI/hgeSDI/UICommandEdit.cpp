// UICommandEdit.cpp : ʵ���ļ�
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

// UICommandEdit ��Ϣ�������

void UICommandEdit::OnEnChange()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CEdit::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}

void UICommandEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

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
	// TODO: �ڴ����ר�ô����/����û���if (pMsg-&gt;message == WM_KEYDOWN &amp;&amp; ::GetKeyState(VK_CONTROL) &lt; 0)
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