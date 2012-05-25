// UIFloatingCommand.cpp : 实现文件
//

#include "stdafx.h"
#include "hgeSDI.h"
#include "UIFloatingCommand.h"


#include "Main.h"
// UIFloatingCommand

IMPLEMENT_DYNAMIC(UIFloatingCommand, CPane)

UIFloatingCommand::UIFloatingCommand()
{

}

UIFloatingCommand::~UIFloatingCommand()
{
}


BEGIN_MESSAGE_MAP(UIFloatingCommand, CPane)
	ON_WM_CREATE()
	ON_NOTIFY(EN_MSGFILTER, IDFC_COMMANDEDIT, &UIFloatingCommand::OnEnMsgFilterCommandEdit)
	ON_NOTIFY(EN_MSGFILTER, IDFC_COMMANDEDIT2, &UIFloatingCommand::OnEnMsgFilterCommandEdit2)
	ON_NOTIFY(EN_REQUESTRESIZE, IDFC_COMMANDEDIT, &UIFloatingCommand::OnEnRequestResizeCommandEdit)
//	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_CLOSE()
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()



// UIFloatingCommand 消息处理程序




int UIFloatingCommand::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CPane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty ();
	if (!m_wndCommandEdit.Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_MULTILINE, rectDummy, this, IDFC_COMMANDEDIT))
	{
		return -1;
	}
	m_wndCommandEdit.SendMessage(EM_EXLIMITTEXT, 0, -1);
	if (!m_wndCommandEdit2.Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_MULTILINE, rectDummy, this, IDFC_COMMANDEDIT2))
	{
		return -1;
	}
	m_wndCommandEdit2.SendMessage(EM_EXLIMITTEXT, 0, -1);

//	m_wndCommandEdit.SetEventMask(m_wndCommandEdit.GetEventMask() | ENM_CHANGE | ENM_KEYEVENTS | ENM_REQUESTRESIZE);
	m_wndCommandEdit2.SetEventMask(m_wndCommandEdit2.GetEventMask() | ENM_CHANGE | ENM_KEYEVENTS);

	// TODO:  在此添加您专用的创建代码
//	ShowPane(FALSE, FALSE, FALSE);

	return 0;
}

void UIFloatingCommand::OnEnMsgFilterCommandEdit( NMHDR *pNMHDR, LRESULT *pResult )
{
	MSGFILTER *pMsgFilter = reinterpret_cast<MSGFILTER *>(pNMHDR);
	if (pMsgFilter->msg == WM_CHAR)
	{
		if (pMsgFilter->wParam == VK_RETURN)
		{
//			CommitCommand();
//			ClearCommand();
			Hide();
		}
		else if (pMsgFilter->wParam == VK_ESCAPE)
		{
//			ClearCommand();
			Hide();
			*pResult = NULL;
		}
		else
		{
			PostMessage(EN_REQUESTRESIZE);
//			m_wndCommandEdit.RequestResize();
		}
	}
}

void UIFloatingCommand::OnEnMsgFilterCommandEdit2( NMHDR *pNMHDR, LRESULT *pResult )
{

}

//void UIFloatingCommand::OnMouseMove(UINT nFlags, CPoint point)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	SetWindowPos(NULL, point.x, point.y, 400, 400, SWP_SHOWWINDOW);
//	RedrawWindow();
//
//	CPane::OnMouseMove(nFlags, point);
//}


void UIFloatingCommand::OnSize(UINT nType, int cx, int cy)
{
	CPane::OnSize(nType, cx, cy);

}


void UIFloatingCommand::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	// 不为绘图消息调用 CPane::OnPaint()
}


void UIFloatingCommand::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

//	CPane::OnClose();
}
/*
void UIFloatingCommand::ClearCommand()
{
	m_wndCommandEdit.SetWindowText("");
}

void UIFloatingCommand::CommitCommand()
{
	int nlength = m_wndCommandEdit.GetWindowTextLength();
	if (!nlength)
	{
		return;
	}
	CString str;
	m_wndCommandEdit.GetWindowText(str);
	MainInterface::getInstance().OnCommitCommand(str.GetBuffer());
}
*/
void UIFloatingCommand::Show( int vk/*=0*/ )
{
	POINT pt;
	GetCursorPos(&pt);
	AfxGetMainWnd()->ScreenToClient(&pt);

#define _UIFC_MOUSEOFFSET	15
#define _UIFC_EDITWIDTH		60
#define _UIFC_EDITHEIGHT	20
#define _UIFC_EDITSPACE		0

	SetWindowPos(NULL, pt.x+_UIFC_MOUSEOFFSET, pt.y+_UIFC_MOUSEOFFSET, _UIFC_EDITWIDTH*2+_UIFC_EDITSPACE, _UIFC_EDITHEIGHT, SWP_DRAWFRAME);
	// TODO: 在此处添加消息处理程序代码
	float xbegin = 0;
	float ybegin = 0;
	m_wndCommandEdit.SetWindowPos(NULL, xbegin, ybegin, _UIFC_EDITWIDTH, _UIFC_EDITHEIGHT, SWP_DRAWFRAME);
	xbegin += _UIFC_EDITWIDTH+_UIFC_EDITSPACE;
	m_wndCommandEdit2.SetWindowPos(NULL, xbegin, ybegin, _UIFC_EDITWIDTH, _UIFC_EDITHEIGHT, SWP_DRAWFRAME);

	ShowPane(TRUE, TRUE, FALSE);
	MainInterface::getInstance().SetMainViewActive(false, MVINACTIVEREASON_FLOATINGCOMMAND);

//	ClearCommand();
	m_wndCommandEdit.SetFocus();
	if (vk)
	{
		m_wndCommandEdit.PostMessage(WM_KEYDOWN, vk, 0);
	}
}

void UIFloatingCommand::Hide(bool bForce)
{
//	ClearCommand();
	ShowPane(FALSE, FALSE, FALSE);
	if (!bForce)
	{
		MainInterface::getInstance().SetMainViewActive(true, MVINACTIVEREASON_FLOATINGCOMMAND);
	}
}

void UIFloatingCommand::OnEnRequestResizeCommandEdit( NMHDR *pNMHDR, LRESULT *pResult )
{
	REQRESIZE* prr = (REQRESIZE*)pNMHDR;
	m_wndCommandEdit.SetWindowPos(NULL, 0, 0, prr->rc.right - prr->rc.left, prr->rc.bottom - prr->rc.top, SWP_DRAWFRAME);
	m_wndCommandEdit.RedrawWindow();
	*pResult = NULL;
}

void UIFloatingCommand::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nChar == VK_ESCAPE)
	{
		Hide();
	}
	else if (nChar == VK_RETURN)
	{
		Hide();
	}
	CPane::OnChar(nChar, nRepCnt, nFlags);
}


void UIFloatingCommand::OnKillFocus(CWnd* pNewWnd)
{
	CPane::OnKillFocus(pNewWnd);

//	Hide();
	// TODO: 在此处添加消息处理程序代码
}


void UIFloatingCommand::OnSetFocus(CWnd* pOldWnd)
{
	CPane::OnSetFocus(pOldWnd);

//	Show();
	// TODO: 在此处添加消息处理程序代码
}
