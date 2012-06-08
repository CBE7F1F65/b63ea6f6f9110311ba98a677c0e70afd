// UIHistoryDockablePane.cpp : 实现文件
//

#include "stdafx.h"
#include "hgeSDI.h"
#include "UIHistoryDockablePane.h"

#include "StringManager.h"
#include "ColorManager.h"
#include "IconManager.h"
#include "Command.h"

#define _HISTORYBUTTON_ICONSIZE	ICMSIZE_MIDDLE
// UIHistoryDockablePane

#define _UIHBBUTTONCOUNT	2

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
	ON_UPDATE_COMMAND_UI_RANGE(ID_UI_BUTTON_HISTORY_BEGIN, ID_UI_BUTTON_HISTORY_END, OnUpdateUIButtons)
	ON_BN_CLICKED(ID_UI_BUTTON_HISTORY_SNAPSHOT, OnSnapShotButtonClicked)
	ON_BN_CLICKED(ID_UI_BUTTON_HISTORY_DELETE, OnDeleteButtonClicked)
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
		WS_CHILD|WS_VISIBLE|WS_BORDER|WS_HSCROLL|WS_VSCROLL|LVS_ALIGNLEFT|LVS_SINGLESEL|LVS_REPORT|LVS_SHOWSELALWAYS|LVS_NOCOLUMNHEADER|LVS_ICON,
		rectDummy, this, IDHB_LISTCTRL))
	{
		return -1;
	}
	if (!m_wndSnapshotListCtrl.Create(
		WS_CHILD|WS_VISIBLE|WS_BORDER|WS_HSCROLL|WS_VSCROLL|LVS_ALIGNLEFT|LVS_SINGLESEL|LVS_REPORT|LVS_SHOWSELALWAYS|LVS_NOCOLUMNHEADER|LVS_ICON|LVS_EDITLABELS,
		rectDummy, this, IDHB_SNAPSHOTLISTCTRL))
	{
		return -1;
	}

	if (!m_wndFakeButtonL.DCreate(ID_UI_BUTTON_HISTORY_FAKEL, this))
	{
		return -1;
	}
	m_wndFakeButtonL.SetButtonEnable(false);
	if (!m_wndFakeButtonR.DCreate(ID_UI_BUTTON_HISTORY_FAKER, this))
	{
		return -1;
	}
	m_wndFakeButtonR.SetButtonEnable(false);

	IconManager * picm = &IconManager::getInstance();

	if (!m_wndSnapshotButton.DCreate(ID_UI_BUTTON_HISTORY_SNAPSHOT, this, 
		StringManager::getInstance().GetToolTipSnapshotName(), 
		_HISTORYBUTTON_ICONSIZE, picm->GetCMDSnapShotIcon(ICMSTATE_NORMAL), picm->GetCMDSnapShotIcon(ICMSTATE_DISABLED)))
	{
		return -1;
	}
	if (!m_wndDeleteButton.DCreate(ID_UI_BUTTON_HISTORY_DELETE, this, 
		StringManager::getInstance().GetToolTipDeleteName(), 
		_HISTORYBUTTON_ICONSIZE, picm->GetCMDDeleteItemIcon(ICMSTATE_NORMAL), picm->GetCMDDeleteItemIcon(ICMSTATE_DISABLED)))
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

	int snapshotheight = cy/4;
	m_wndSnapshotListCtrl.SetWindowPos(NULL, -1, -1, cx, snapshotheight,
		SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);

	int buttonybegin = cy-_HISTORYBUTTON_ICONSIZE;
	int listctrlheight = buttonybegin-snapshotheight;
	m_wndListCtrl.SetWindowPos (NULL, 0, snapshotheight, cx, listctrlheight,
		SWP_NOACTIVATE | SWP_NOZORDER);
	// TODO: 在此处添加消息处理程序代码
	int fakerwidth = _HISTORYBUTTON_ICONSIZE;
	int fakelwidth = cx-_UIHBBUTTONCOUNT*_HISTORYBUTTON_ICONSIZE-fakerwidth;
	int a = _HISTORYBUTTON_ICONSIZE;

	m_wndFakeButtonL.SetWindowPos(NULL, 0, buttonybegin, fakelwidth, a, 
		SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndSnapshotButton.SetWindowPos(NULL, fakelwidth, buttonybegin, a, a, 
		SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndDeleteButton.SetWindowPos(NULL, fakelwidth+a, buttonybegin, a, a, 
		SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndFakeButtonR.SetWindowPos(NULL, fakelwidth+_UIHBBUTTONCOUNT*a, buttonybegin, a, a, 
		SWP_NOACTIVATE | SWP_NOZORDER);
	Invalidate();
}

bool UIHistoryDockablePane::AddHistory( const char * desc, const char * commandstr, int command )
{
	return m_wndListCtrl.AddHistory(desc, commandstr, command);
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

void UIHistoryDockablePane::OnUpdateUIButtons(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_nID != ID_UI_BUTTON_HISTORY_FAKEL && pCmdUI->m_nID != ID_UI_BUTTON_HISTORY_FAKER)
	{
		pCmdUI->Enable();
	}
}

void UIHistoryDockablePane::OnSnapShotButtonClicked()
{
	m_wndSnapshotListCtrl.AddSnapshot();
}

void UIHistoryDockablePane::OnDeleteButtonClicked()
{
	m_wndSnapshotListCtrl.DeleteSnapshot();
}