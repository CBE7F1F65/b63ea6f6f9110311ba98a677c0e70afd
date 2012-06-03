// UIPopupMenu.cpp : 实现文件
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

// UIPopupMenu 消息处理程序

void UIPopupMenu::PostNcDestroy()
{
	// TODO: 在此添加专用代码和/或调用基类

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

	// TODO:  在此添加您专用的创建代码

	MainInterface::getInstance().SetMainViewActive(false, MVINACTIVEREASON_POPUP);
	/*
	MainInterface::getInstance().parentview->GetMainFrame()->
		m_wndUICommandBox.ShowPane(FALSE, FALSE, FALSE);
		*/

	return 0;
}

void UIPopupMenu::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
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
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	SetWindowPos(NULL, point.x, point.y, -1, -1, SWP_DRAWFRAME);
//	CMFCPopupMenu::OnMouseMove(nFlags, point);
//}