// UICommandBox.cpp : 实现文件
//

#include "stdafx.h"
#include "hgeSDI.h"
#include "UICommandBox.h"

#include "Main.h"


// UICommandBox

IMPLEMENT_DYNAMIC(UICommandBox, CDockablePane)

UICommandBox::UICommandBox()
{

}

UICommandBox::~UICommandBox()
{
}


BEGIN_MESSAGE_MAP(UICommandBox, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_EN_CHANGE(IDCB_COMMANDEDIT, &UICommandBox::OnEnChangeCommandEdit)
	ON_NOTIFY(EN_MSGFILTER, IDCB_COMMANDEDIT, &UICommandBox::OnEnMsgFilterCommandEdit)
//	ON_WM_CHAR()
ON_WM_MOUSEHOVER()
ON_WM_MOUSELEAVE()
ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()



// UICommandBox 消息处理程序

#define UICB_DEFAULTNTABVIEW		3
#define UICB_TABVIEWSTR				"View "
#define UICB_COMMANDEDITPROMPTSTR	"Command: "

int UICommandBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty ();

	if (!m_wndCommandEditPrompt.Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_READONLY, rectDummy, this, IDCB_COMMANDEDITPROMPT))
	{
		return -1;
	}
	m_wndCommandEditPrompt.SetWindowText(UICB_COMMANDEDITPROMPTSTR);

	if (!m_wndCommandEdit.Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_MULTILINE, rectDummy, this, IDCB_COMMANDEDIT))
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

	m_wndCommandEdit.SetEventMask(m_wndCommandEdit.GetEventMask() | ENM_CHANGE | ENM_KEYEVENTS);

	return 0;
}


void UICommandBox::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	m_wndTabs.SetWindowPos (NULL, -1, -1, cx, cy,
		SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
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
	m_wndCommandEdit.SetWindowPos(NULL, commandpromptwidth, commandlogheight, cx-commandpromptwidth, commandheight,
		SWP_NOACTIVATE | SWP_DRAWFRAME);
}

void UICommandBox::AppendCommandLogText( LPCTSTR text )
{
	CString str;
	m_wndCommandLogEdit.GetWindowText(str);
	str += text;
	str += "\n";
	m_wndCommandLogEdit.SetWindowText(str);
	m_wndCommandLogEdit.PostMessage(WM_VSCROLL, SB_BOTTOM,0);
}

void UICommandBox::OnEnChangeCommandEdit()
{
}

void UICommandBox::OnEnMsgFilterCommandEdit(NMHDR *pNMHDR, LRESULT *pResult)
{
	MSGFILTER *pMsgFilter = reinterpret_cast<MSGFILTER *>(pNMHDR);
	if (pMsgFilter->msg == WM_CHAR)
	{
		if (pMsgFilter->wParam == VK_RETURN)
		{
			CommitCommand();
			ClearCommand();
		}
	}
}
//void UICommandBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	m_wndCommandEdit.SetFocus();
//
//	CDockablePane::OnChar(nChar, nRepCnt, nFlags);
//}


void UICommandBox::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_wndCommandEdit.SetFocus();
	CDockablePane::OnMouseHover(nFlags, point);
}


void UICommandBox::OnMouseLeave()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CDockablePane::OnMouseLeave();
}


void UICommandBox::OnMouseMove(UINT nFlags, CPoint point)
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

void UICommandBox::CommitCommand()
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

void UICommandBox::ClearLog()
{
	m_wndCommandLogEdit.SetWindowText("");
}

void UICommandBox::ClearCommand()
{
	m_wndCommandEdit.SetWindowText("");
}