// UIPopupMenu.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "hgeSDI.h"
#include "UIPopupMenu.h"

#include "Main.h"

// UIPopupMenu

IMPLEMENT_DYNAMIC(UIPopupMenu, CMFCPopupMenu)

UIPopupMenu::UIPopupMenu()
{
}

UIPopupMenu::~UIPopupMenu()
{
}

BEGIN_MESSAGE_MAP(UIPopupMenu, CMFCPopupMenu)
	ON_WM_CREATE()
	ON_WM_CLOSE()
//	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// UIPopupMenu ��Ϣ�������

void UIPopupMenu::PostNcDestroy()
{
	// TODO: �ڴ����ר�ô����/����û���

	if (m_pParentBtn != NULL)
	{
		ASSERT_VALID(m_pParentBtn);
		m_pParentBtn->OnCancelMode();
	}

	if (m_pParentRibbonElement != NULL)
	{
		ASSERT_VALID(m_pParentRibbonElement);
		m_pParentRibbonElement->ClosePopupMenu();
	}
//	CMFCPopupMenu::PostNcDestroy();
}

int UIPopupMenu::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMFCPopupMenu::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������

	MainInterface::getInstance().SetMainViewActive(false, MVINACTIVEREASON_POPUP);
	/*
	MainInterface::getInstance().parentview->GetMainFrame()->
		m_wndUICommandBox.ShowPane(FALSE, FALSE, FALSE);
		*/

	return 0;
}

void UIPopupMenu::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	MainInterface::getInstance().SetMainViewActive(true, MVINACTIVEREASON_POPUP);
	/*
	MainInterface::getInstance().parentview->GetMainFrame()->
		m_wndUICommandBox.ShowPane(TRUE, TRUE, TRUE);
		*/
	CMFCPopupMenu::OnClose();
}

void UIPopupMenu::Hide( bool bForce/*=false*/ )
{
}

//void UIPopupMenu::OnMouseMove(UINT nFlags, CPoint point)
//{
//	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
//	SetWindowPos(NULL, point.x, point.y, -1, -1, SWP_DRAWFRAME);
//	CMFCPopupMenu::OnMouseMove(nFlags, point);
//}