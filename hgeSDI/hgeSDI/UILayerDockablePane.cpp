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

	if (!m_wndTreeCtrl.Create(
		WS_CHILD|WS_VISIBLE|WS_BORDER|WS_TABSTOP|TVS_EDITLABELS|TVS_HASBUTTONS|TVS_HASLINES|TVS_SHOWSELALWAYS|TVS_SINGLEEXPAND|TVS_LINESATROOT, 
		rectDummy, this, IDLB_TREECTRL))
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
	m_wndTreeCtrl.SetWindowPos (NULL, -1, -1, cx, cy,
		SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void UILayerDockablePane::RebuildTree( GObject * changebase )
{
	m_wndTreeCtrl.RebuildTree(changebase);
}