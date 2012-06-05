// UICommandBox.cpp : 实现文件
//

#include "stdafx.h"
#include "hgeSDI.h"
#include "UICommandDockablePane.h"

#include "Main.h"

// UICommandBox

IMPLEMENT_DYNAMIC(UICommandDockablePane, CDockablePane)

UICommandDockablePane::UICommandDockablePane()
{
}

UICommandDockablePane::~UICommandDockablePane()
{
}

BEGIN_MESSAGE_MAP(UICommandDockablePane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
//	ON_EN_CHANGE(IDCB_COMMANDEDIT, &UICommandDockablePane::OnEnChangeCommandEdit)
//	ON_NOTIFY(EN_MSGFILTER, IDCB_COMMANDEDIT, &UICommandDockablePane::OnEnMsgFilterCommandEdit)
//	ON_WM_CHAR()
ON_WM_MOUSEHOVER()
ON_WM_MOUSELEAVE()
ON_WM_MOUSEMOVE()
ON_WM_CLOSE()
END_MESSAGE_MAP()

// UICommandBox 消息处理程序

#define UICB_DEFAULTNTABVIEW		3
#define UICB_TABVIEWSTR				"View "
#define UICB_COMMANDEDITPROMPTSTR	"Command: "

int UICommandDockablePane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty ();
	/*
	if (!m_wndCommandEditPrompt.Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_READONLY, rectDummy, this, IDCB_COMMANDEDITPROMPT))
	{
		return -1;
	}
	m_wndCommandEditPrompt.SetWindowText(UICB_COMMANDEDITPROMPTSTR);
	*/
	if (!m_wndCommandEdit.Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_MULTILINE|ES_WANTRETURN|ES_AUTOHSCROLL|WS_VSCROLL, rectDummy, this, IDCB_COMMANDEDIT))
	{
		return -1;
	}
	m_wndCommandEdit.SendMessage(EM_EXLIMITTEXT, 0, -1);

	if (!m_wndCommandLogEdit.Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|WS_VSCROLL|ES_MULTILINE|ES_READONLY, rectDummy, this, IDCB_COMMANDLOGEDIT))
	{
		return -1;
	}
	m_wndCommandLogEdit.SendMessage(EM_EXLIMITTEXT, 0, -1);

	// Create tabs window:
	if (!m_wndTabs.Create (CMFCTabCtrl::STYLE_FLAT, rectDummy, this, IDCB_TABSHOLDER))
	{
		return -1;      // fail to create
	}
	m_wndTabs.SetFlatFrame (FALSE, FALSE);

	// Create list windows.
	// TODO: create your own window here:
	const DWORD dwViewStyle =
		LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

	char tabtitlebuffer[M_STRMAX];
	for (int i=0; i<UICB_DEFAULTNTABVIEW; i++)
	{
		if (!listctrl[i].Create(dwViewStyle, rectDummy, &m_wndTabs, IDCB_TABSBEGIN+i))
		{
			return -1;
		}
		sprintf_s(tabtitlebuffer, M_STRMAX, "%s%d", UICB_TABVIEWSTR, i+1);
		m_wndTabs.AddTab(&(listctrl[i]), tabtitlebuffer, (UINT)-1);
	}

//	m_wndCommandEdit.SetEventMask(m_wndCommandEdit.GetEventMask() | ENM_CHANGE | ENM_KEYEVENTS);

	return 0;
}

void UICommandDockablePane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	m_wndTabs.SetWindowPos (NULL, -1, -1, cx, cy,
		SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	/*
	CHARFORMAT cf;
	m_wndCommandEdit.GetDefaultCharFormat(cf);
	HDC hdc = ::GetDC(NULL);
	int dpi = 72;
	if (hdc)
	{
		dpi = GetDeviceCaps(hdc, LOGPIXELSY);
	}
	int commandheight = cf.yHeight*dpi/1440*2;
	int commandlogheight = cy-m_wndTabs.GetTabsHeight()-commandheight;
	m_wndCommandLogEdit.SetWindowPos(NULL, 0, 0, cx, commandlogheight,
		SWP_NOACTIVATE | SWP_DRAWFRAME);
	m_wndCommandLogEdit.PostMessage(WM_VSCROLL, SB_BOTTOM,0);
	int commandpromptwidth = m_wndCommandEditPrompt.GetWindowTextLength();
	m_wndCommandEditPrompt.GetDefaultCharFormat(cf);
	if (hdc)
	{
		dpi = GetDeviceCaps(hdc, LOGPIXELSX);
	}
	commandpromptwidth *= cf.yHeight*dpi/1440;

	m_wndCommandEditPrompt.SetWindowPos(NULL, 0, commandlogheight, commandpromptwidth, commandheight,
		SWP_NOACTIVATE | SWP_DRAWFRAME);
	m_wndCommandEdit.SetWindowPos(NULL, 0, commandlogheight, cx, commandheight,
		SWP_NOACTIVATE | SWP_DRAWFRAME);
	*/
#define _UICDP_EDITLBEGIN	15
	cy -= m_wndTabs.GetTabsHeight();
	cx -= _UICDP_EDITLBEGIN;
	CString strw = "w";
	CDC * cdc = m_wndCommandEdit.GetDC();
	CSize sizew = cdc->GetTextExtent(strw);
	float commandeditheight = sizew.cy*2;
	m_wndCommandLogEdit.SetWindowPos(NULL, _UICDP_EDITLBEGIN, 0, cx, cy-commandeditheight, SWP_NOACTIVATE|SWP_DRAWFRAME);
	m_wndCommandLogEdit.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	m_wndCommandEdit.SetWindowPos(NULL, _UICDP_EDITLBEGIN, cy-commandeditheight, cx, commandeditheight, SWP_DRAWFRAME);
}

void UICommandDockablePane::AppendCommandLogText( LPCTSTR text, bool bNewLine/*=true*/ )
{
	m_wndCommandLogEdit.AppendCommandLogText(text, bNewLine);
}

void UICommandDockablePane::SetCommandText( LPCTSTR text, bool bActivate/*=true*/ )
{
	m_wndCommandEdit.SetCommandText(text, bActivate);
}

void UICommandDockablePane::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_wndCommandEdit.SetFocus();
	CDockablePane::OnMouseHover(nFlags, point);
}

void UICommandDockablePane::OnMouseLeave()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDockablePane::OnMouseLeave();
}

void UICommandDockablePane::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.hwndTrack = m_hWnd;
	tme.dwFlags = TME_LEAVE|TME_HOVER;
	tme.dwHoverTime = 1;
	TrackMouseEvent(&tme);
	CDockablePane::OnMouseMove(nFlags, point);
}

void UICommandDockablePane::ClearLog()
{
	m_wndCommandLogEdit.ClearLog();
}
void UICommandDockablePane::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

//	CDockablePane::OnClose();
}