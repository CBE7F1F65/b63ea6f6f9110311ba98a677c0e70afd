// UIHistoryDockablePane.cpp : 实现文件
//

#include "stdafx.h"
#include "hgeSDI.h"
#include "UIHistoryDockablePane.h"


// UIHistoryDockablePane

IMPLEMENT_DYNAMIC(UIHistoryDockablePane, CDockablePane)

UIHistoryDockablePane::UIHistoryDockablePane()
{

}

UIHistoryDockablePane::~UIHistoryDockablePane()
{
}


BEGIN_MESSAGE_MAP(UIHistoryDockablePane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// UIHistoryDockablePane 消息处理程序




int UIHistoryDockablePane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码


	CRect rectDummy;
	rectDummy.SetRectEmpty ();

	if (!m_wndListCtrl.Create(
		WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_ALIGNLEFT|LVS_SINGLESEL|LVS_REPORT|LVS_SHOWSELALWAYS|LVS_NOCOLUMNHEADER, 
		rectDummy, this, IDHB_LISTCTRL))
	{
		return -1;
	}

	return 0;
}


void UIHistoryDockablePane::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

//	CDockablePane::OnClose();
}


void UIHistoryDockablePane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	m_wndListCtrl.SetWindowPos (NULL, -1, -1, cx, cy,
		SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	// TODO: 在此处添加消息处理程序代码
}

bool UIHistoryDockablePane::AddHistory( const char * desc, const char * commandstr )
{
	return m_wndListCtrl.AddHistory(desc, commandstr);
}

bool UIHistoryDockablePane::ChangeCurrentHistory( int step )
{
	return m_wndListCtrl.ChangeCurrentHistory(step);
}

bool UIHistoryDockablePane::ClearLaterHistory( int ndelete )
{
	return m_wndListCtrl.ClearLaterHistory(ndelete);
}

bool UIHistoryDockablePane::ClearPreviousHistory( int ndelete )
{
	return m_wndListCtrl.ClearPreviousHistory(ndelete);
}