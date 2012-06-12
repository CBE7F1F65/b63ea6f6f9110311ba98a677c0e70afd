// UILayerDockablePane.cpp : 实现文件
//

#include "stdafx.h"
#include "hgeSDI.h"
#include "UILayerDockablePane.h"
#include "Main.h"

#include "StringManager.h"
#include "ColorManager.h"
#include "IconManager.h"
#include "Command.h"

#define _LAYERBUTTON_ICONSIZE	ICMSIZE_MIDDLE
// UILayerDockablePane


#define _UILBBUTTONCOUNT	3

IMPLEMENT_DYNAMIC(UILayerDockablePane, CDockablePane)

UILayerDockablePane::UILayerDockablePane()
{
}

UILayerDockablePane::~UILayerDockablePane()
{
}

BEGIN_MESSAGE_MAP(UILayerDockablePane, CDockablePane)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI_RANGE(ID_UI_BUTTON_LAYER_BEGIN, ID_UI_BUTTON_LAYER_END, &UILayerDockablePane::OnUpdateUIButtons)
	ON_BN_CLICKED(ID_UI_BUTTON_LAYER_NEWLAYER, OnNewLayerButtonClicked)
	ON_BN_CLICKED(ID_UI_BUTTON_LAYER_NEWSUBLAYER, OnNewSubLayerButtonClicked)
	ON_BN_CLICKED(ID_UI_BUTTON_LAYER_DELETEITEM, OnDeleteItemButtonClicked)
END_MESSAGE_MAP()

// UILayerDockablePane 消息处理程序

int UILayerDockablePane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	CRect rectDummy;
	rectDummy.SetRectEmpty ();

	if (!m_wndListCtrl.Create(
		WS_CHILD|WS_VISIBLE|WS_BORDER|WS_HSCROLL|WS_VSCROLL|LVS_ALIGNLEFT|LVS_REPORT|LVS_SHOWSELALWAYS|LVS_NOCOLUMNHEADER|LVS_ICON|LVS_EDITLABELS,
		rectDummy, this, IDLB_LISTCTRL))
	{
		return -1;
	}
	if (!m_wndFakeButtonL.DCreate(ID_UI_BUTTON_LAYER_FAKEL, this))
	{
		return -1;
	}
	m_wndFakeButtonL.SetButtonEnable(false);
	if (!m_wndFakeButtonR.DCreate(ID_UI_BUTTON_LAYER_FAKER, this))
	{
		return -1;
	}
	m_wndFakeButtonR.SetButtonEnable(false);
	IconManager * picm = &IconManager::getInstance();

	if (!m_wndNewLayerButton.DCreate(ID_UI_BUTTON_LAYER_NEWLAYER, this, 
		StringManager::getInstance().GetCommandDescriptionName(COMM_NEWLAYER), 
		_LAYERBUTTON_ICONSIZE, picm->GetCMDNewLayerIcon(ICMSTATE_NORMAL), picm->GetCMDNewLayerIcon(ICMSTATE_DISABLED)))
	{
		return -1;
	}
	if (!m_wndNewSubLayerButton.DCreate(ID_UI_BUTTON_LAYER_NEWSUBLAYER, this, 
		StringManager::getInstance().GetCommandDescriptionName(COMM_NEWSUBLAYER), 
		_LAYERBUTTON_ICONSIZE, picm->GetCMDNewSubLayerIcon(ICMSTATE_NORMAL), picm->GetCMDNewSubLayerIcon(ICMSTATE_DISABLED)))
	{
		return -1;
	}
	if (!m_wndDeleteItemButton.DCreate(ID_UI_BUTTON_LAYER_DELETEITEM, this, 
		StringManager::getInstance().GetCommandDescriptionName(COMM_DELETEITEM), 
		_LAYERBUTTON_ICONSIZE, picm->GetCMDDeleteItemIcon(ICMSTATE_NORMAL), picm->GetCMDDeleteItemIcon(ICMSTATE_DISABLED)))
	{
		return -1;
	}

	return 0;
}

void UILayerDockablePane::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

//	CDockablePane::OnClose();
}

void UILayerDockablePane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	int ybegin = cy-_LAYERBUTTON_ICONSIZE;
	// TODO: 在此处添加消息处理程序代码
	m_wndListCtrl.SetWindowPos (NULL, -1, -1, cx, ybegin,
		SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);

	int fakerwidth = _LAYERBUTTON_ICONSIZE;
	int fakelwidth = cx-_UILBBUTTONCOUNT*_LAYERBUTTON_ICONSIZE-fakerwidth;
	int a = _LAYERBUTTON_ICONSIZE;

	m_wndFakeButtonL.SetWindowPos(NULL, 0, ybegin, fakelwidth, a, 
		SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndNewLayerButton.SetWindowPos(NULL, fakelwidth, ybegin, a, a, 
		SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndNewSubLayerButton.SetWindowPos(NULL, fakelwidth+a, ybegin, a, a, 
		SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndDeleteItemButton.SetWindowPos(NULL, fakelwidth+2*a, ybegin, a, a, 
		SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndFakeButtonR.SetWindowPos(NULL, fakelwidth+_UILBBUTTONCOUNT*a, ybegin, a, a, 
		SWP_NOACTIVATE | SWP_NOZORDER);
	Invalidate();
}

// void UILayerDockablePane::RebuildTree( GObject * changebase, GObject * activeitem )
// {
// 	m_wndListCtrl.PostMessage(WMUSER_REBUILDTREE, (WPARAM)changebase, (LPARAM)activeitem);
// }
void UILayerDockablePane::RebuildTree( GObject * changebase, GObject * activeitem )
{
	m_wndListCtrl.RebuildTree(changebase, activeitem);
}

GLayer * UILayerDockablePane::GetActiveLayer()
{
	return m_wndListCtrl.GetActiveLayer();
}

void UILayerDockablePane::SetActiveLayer_Internal( GLayer * pLayer )
{
	return m_wndListCtrl.SetActiveLayer_Internal(pLayer);
}

GObject * UILayerDockablePane::GetActiveNodes( int * pnextfromIndex )
{
	return m_wndListCtrl.GetActiveNodes( pnextfromIndex );
}

bool UILayerDockablePane::GetDragDropNodes( GLayer ** pLayerNode, GObject ** pAfterNode )
{
	return m_wndListCtrl.GetDragDropNodes(pLayerNode, pAfterNode);
}

void UILayerDockablePane::OnUpdateUIButtons(CCmdUI *pCmdUI)
{
	if (pCmdUI->m_nID != ID_UI_BUTTON_LAYER_FAKEL && pCmdUI->m_nID != ID_UI_BUTTON_LAYER_FAKER)
	{
		pCmdUI->Enable();
	}
}

void UILayerDockablePane::OnNewLayerButtonClicked()
{
	MainInterface::getInstance().OnCommand(COMM_NEWLAYER);
}

void UILayerDockablePane::OnNewSubLayerButtonClicked()
{
	MainInterface::getInstance().OnCommand(COMM_NEWSUBLAYER);
}

void UILayerDockablePane::OnDeleteItemButtonClicked()
{
	MainInterface::getInstance().OnCommand(COMM_DELETEITEM);
}

void UILayerDockablePane::CallReparentSelectedNodes( int op )
{
	m_wndListCtrl.ReparentSelectedNodes(op);
}

void UILayerDockablePane::LockTreeChange( bool toLock )
{
	m_wndListCtrl.LockTreeChange(toLock);
}