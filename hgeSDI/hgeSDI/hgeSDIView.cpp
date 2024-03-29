// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和
// MFC C++ 库软件随附的相关电子文档。
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问
// http://msdn.microsoft.com/officeui。
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// hgeSDIView.cpp : ChgeSDIView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "hgeSDI.h"
#endif

#include "hgeSDIDoc.h"
#include "hgeSDIView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ChgeSDIView

IMPLEMENT_DYNCREATE(ChgeSDIView, CView)

BEGIN_MESSAGE_MAP(ChgeSDIView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &ChgeSDIView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()

	ON_WM_SIZE()

	ON_WM_ACTIVATE()
	ON_WM_TIMER()
	ON_WM_MOUSEACTIVATE()
//	ON_WM_MOUSEMOVE()
ON_WM_HSCROLL()
ON_WM_VSCROLL()
ON_WM_CREATE()
END_MESSAGE_MAP()

// ChgeSDIView 构造/析构

ChgeSDIView::ChgeSDIView()
{
	// TODO: 在此处添加构造代码
}

ChgeSDIView::~ChgeSDIView()
{
}

BOOL ChgeSDIView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// ChgeSDIView 绘制

void ChgeSDIView::OnDraw(CDC* /*pDC*/)
{
	ChgeSDIDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}

// ChgeSDIView 打印

void ChgeSDIView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL ChgeSDIView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void ChgeSDIView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void ChgeSDIView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}

void ChgeSDIView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

#include "Main.h"

void ChgeSDIView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
	GetMainFrame()->CallShowContextMenu(point.x, point.y);
	/*
#ifndef SHARED_HANDLERS
	MainInterface::getInstance().SetMainViewActive(false);

	CMenu menu;
	menu.LoadMenu(IDR_POPUP_EDIT);
	HMENU hmenuPopup = ::GetSubMenu(menu.Detach(), 0);
	theApp.GetContextMenuManager()->TrackPopupMenu(hmenuPopup, point.x, point.y, this);

//	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
	MainInterface::getInstance().SetMainViewActive(true);
#endif
	*/
}

// ChgeSDIView 诊断

#ifdef _DEBUG
void ChgeSDIView::AssertValid() const
{
	CView::AssertValid();
}

void ChgeSDIView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

ChgeSDIDoc* ChgeSDIView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(ChgeSDIDoc)));
	return (ChgeSDIDoc*)m_pDocument;
}
#endif //_DEBUG

CMainFrame* ChgeSDIView::GetMainFrame() const
{
	return (CMainFrame*) AfxGetMainWnd();
}

#include "Main.h"
#include "Command.h"

void ChgeSDIView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	RECT rect;
	this->GetClientRect(&rect);
	bool manageloop = MainInterface::getInstance().OnInit(this, rect.right-rect.left, rect.bottom-rect.top);

	if (!manageloop)
	{
		SetTimer(NULL, 0, NULL);
	}
}

void ChgeSDIView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	MainInterface::getInstance().OnResizeWindow(cx, cy);
}

// ChgeSDIView 消息处理程序

void ChgeSDIView::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CView::OnActivate(nState, pWndOther, bMinimized);

	// TODO: 在此处添加消息处理程序代码
	/*
	*/
}

void ChgeSDIView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CView::OnTimer(nIDEvent);
	MainInterface::getInstance().OnUpdateTimer();
}

void ChgeSDIView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	// TODO: 在此添加专用代码和/或调用基类

	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	/*
	if (bActivate)
	{
		MainInterface::getInstance().FocusGain();
	}
	else
	{
		MainInterface::getInstance().FocusLost();
	}
	*/
}

int ChgeSDIView::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	MainInterface::getInstance().OnMouseActivate();
	return CView::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

void ChgeSDIView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: 在此添加专用代码和/或调用基类
}

//void ChgeSDIView::OnMouseMove(UINT nFlags, CPoint point)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	GetMainFrame()->m_wndUIFloatingCommand.SetWindowPos(NULL, point.x+20, point.y+20, 100, 100, SWP_DRAWFRAME);
//	CView::OnMouseMove(nFlags, point);
//}

#define _UISCROLL_FAR	1000
#define _UISCROLL_PAGE	100
#define _UISCROLL_NPAGE	10
#define _UISCROLL_LINE	10
#define _UISCROLL_RANGE	1000

void ChgeSDIView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int minpos, maxpos;
	pScrollBar->GetScrollRange(&minpos, &maxpos);

	int lastpos = pScrollBar->GetScrollPos();
	int curpos = lastpos;
	switch (nSBCode)
	{
	case SB_TOP:
		curpos = 0;
		break;
	case SB_BOTTOM:
		curpos = _UISCROLL_FAR;
		break;
	case SB_LINEUP:
		curpos -= _UISCROLL_LINE;
		if (curpos < 0)
		{
			curpos = 0;
		}
		break;
	case SB_LINEDOWN:
		curpos += _UISCROLL_LINE;
		if (curpos > _UISCROLL_RANGE)
		{
			curpos = 0;
		}
		break;
	case SB_PAGEUP:
		curpos -= _UISCROLL_PAGE;
		if (curpos < 0)
		{
			curpos = 0;
		}
		break;
	case SB_PAGEDOWN:
		curpos += _UISCROLL_PAGE;
		if (curpos > _UISCROLL_RANGE)
		{
			curpos = 0;
		}
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		curpos = nPos;
		break;
	case SB_ENDSCROLL:
		pScrollBar->SetScrollPos(_UISCROLL_RANGE/2);
		return;
		break;
//		ChgeSDIView::OnHScroll(SB_THUMBPOSITION/*nSBCode*/, _UISCROLL_RANGE/2/*nPos*/, pScrollBar);
//		return;
	}

	MainInterface::getInstance().OnDoScroll(true, curpos-lastpos, _UISCROLL_RANGE);
	pScrollBar->SetScrollPos(curpos);

	CView::OnHScroll(nSBCode, nPos, pScrollBar);
}

void ChgeSDIView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	int lastpos = pScrollBar->GetScrollPos();
	int curpos = lastpos;
	switch (nSBCode)
	{
	case SB_LEFT:
		curpos = 0;
		break;
	case SB_RIGHT:
		curpos = _UISCROLL_FAR;
		break;
	case SB_LINELEFT:
		curpos -= _UISCROLL_LINE;
		if (curpos < 0)
		{
			curpos = 0;
		}
		break;
	case SB_LINERIGHT:
		curpos += _UISCROLL_LINE;
		if (curpos > _UISCROLL_RANGE)
		{
			curpos = 0;
		}
		break;
	case SB_PAGELEFT:
		curpos -= _UISCROLL_PAGE;
		if (curpos < 0)
		{
			curpos = 0;
		}
		break;
	case SB_PAGERIGHT:
		curpos += _UISCROLL_PAGE;
		if (curpos > _UISCROLL_RANGE)
		{
			curpos = 0;
		}
		break;
	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		curpos = nPos;
		break;
	case SB_ENDSCROLL:
		pScrollBar->SetScrollPos(_UISCROLL_RANGE/2);
		return;
		break;
//		ChgeSDIView::OnVScroll(SB_THUMBPOSITION/*nSBCode*/, _UISCROLL_RANGE/2/*nPos*/, pScrollBar);
//		return;
	}

	MainInterface::getInstance().OnDoScroll(false, curpos-lastpos, _UISCROLL_RANGE);
	pScrollBar->SetScrollPos(curpos);

	CView::OnVScroll(nSBCode, nPos, pScrollBar);
}

int ChgeSDIView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	SCROLLINFO scrinfo;
	scrinfo.cbSize = sizeof(SCROLLINFO);
	scrinfo.nMin = 0;
	scrinfo.nMax = _UISCROLL_RANGE;
	scrinfo.nPage = _UISCROLL_NPAGE;
	scrinfo.nPos = _UISCROLL_RANGE/2;
	scrinfo.nTrackPos = 0;
	scrinfo.fMask = SIF_ALL;

	SetScrollInfo(SB_VERT, &scrinfo);
	SetScrollInfo(SB_HORZ, &scrinfo);

	return 0;
}