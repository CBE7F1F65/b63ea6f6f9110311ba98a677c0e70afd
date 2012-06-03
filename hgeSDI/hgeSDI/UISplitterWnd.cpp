// UISplitterWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "hgeSDI.h"
#include "UISplitterWnd.h"

// UISplitterWnd

IMPLEMENT_DYNCREATE(UISplitterWnd, CSplitterWnd)

UISplitterWnd::UISplitterWnd()
{
}

UISplitterWnd::~UISplitterWnd()
{
}

BEGIN_MESSAGE_MAP(UISplitterWnd, CSplitterWnd)
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_CREATE()
END_MESSAGE_MAP()

// UISplitterWnd 消息处理程序

void UISplitterWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CSplitterWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void UISplitterWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CSplitterWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

int UISplitterWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CSplitterWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	return 0;
}