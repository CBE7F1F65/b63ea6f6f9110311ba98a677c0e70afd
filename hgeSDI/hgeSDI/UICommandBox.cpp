// UICommandBox.cpp : 实现文件
//

#include "stdafx.h"
#include "hgeSDI.h"
#include "UICommandBox.h"


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
END_MESSAGE_MAP()



// UICommandBox 消息处理程序




int UICommandBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty ();

	if (!m_wndCommandEditPrompt.Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_READONLY, rectDummy, this, 7))
	{
		return -1;
	}
	m_wndCommandEditPrompt.SetWindowText("Command:");

	if (!m_wndCommandEdit.Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_MULTILINE, rectDummy, this, 5))
	{
		return -1;
	}
	m_wndCommandEdit.SendMessage(EM_EXLIMITTEXT, 0, -1);
	if (!m_wndCommandLogEdit.Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL|WS_VSCROLL|ES_MULTILINE|ES_READONLY, rectDummy, this, 6))
	{
		return -1;
	}
	m_wndCommandLogEdit.SendMessage(EM_EXLIMITTEXT, 0, -1);

	// Create tabs window:
	if (!m_wndTabs.Create (CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
	{
		TRACE0("Failed to create output tab window\n");
		return -1;      // fail to create
	}
	m_wndTabs.SetFlatFrame (FALSE, FALSE);

	// Create list windows.
	// TODO: create your own window here:
	const DWORD dwViewStyle =	
		LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

	if (!m_wndList1.Create (dwViewStyle, rectDummy, &m_wndTabs, 2) ||
		!m_wndList2.Create (dwViewStyle, rectDummy, &m_wndTabs, 3) ||
		!m_wndList3.Create (dwViewStyle, rectDummy, &m_wndTabs, 4))
	{
		TRACE0("Failed to create output view\n");
		return -1;      // fail to create
	}

	// Attach list windows to tab:
	m_wndTabs.AddTab (&m_wndList1, _T("Output 1"), (UINT)-1);
	m_wndTabs.AddTab (&m_wndList2, _T("Output 2"), (UINT)-1);
	m_wndTabs.AddTab (&m_wndList3, _T("Output 3"), (UINT)-1);

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