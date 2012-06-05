// UILayerDockablePane.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "hgeSDI.h"
#include "UILayerDockablePane.h"

// UILayerDockablePane

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
END_MESSAGE_MAP()

// UILayerDockablePane ��Ϣ�������

int UILayerDockablePane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	CRect rectDummy;
	rectDummy.SetRectEmpty ();

	if (!m_wndListCtrl.Create(
		WS_CHILD|WS_VISIBLE|WS_BORDER|WS_HSCROLL|WS_VSCROLL|LVS_ALIGNLEFT|LVS_REPORT|LVS_SHOWSELALWAYS|LVS_NOCOLUMNHEADER|LVS_ICON|LVS_EDITLABELS,
		rectDummy, this, IDLB_LISTCTRL))
	{
		return -1;
	}

	return 0;
}

void UILayerDockablePane::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

//	CDockablePane::OnClose();
}

void UILayerDockablePane::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
	m_wndListCtrl.SetWindowPos (NULL, -1, -1, cx, cy,
		SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void UILayerDockablePane::RebuildTree( GObject * changebase, GObject * activeitem )
{
	m_wndListCtrl.RebuildTree(changebase, activeitem);
}

GLayer * UILayerDockablePane::GetActiveLayer()
{
	return m_wndListCtrl.GetActiveLayer();
}

void UILayerDockablePane::SetActiveLayer( GLayer * pLayer )
{
	return m_wndListCtrl.SetActiveLayer(pLayer);
}