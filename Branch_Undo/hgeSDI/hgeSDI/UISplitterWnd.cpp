// UISplitterWnd.cpp : ʵ���ļ�
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

// UISplitterWnd ��Ϣ�������

void UISplitterWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CSplitterWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

void UISplitterWnd::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CSplitterWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

int UISplitterWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CSplitterWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������

	return 0;
}