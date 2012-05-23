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

	ON_COMMAND(ID_LINE, &ChgeSDIView::OnLine)
	ON_WM_ACTIVATE()
	ON_WM_TIMER()
	ON_WM_MOUSEACTIVATE()
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

void ChgeSDIView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
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
		SetTimer(NULL, 1, NULL);
	}
}

void ChgeSDIView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	MainInterface::getInstance().OnResizeWindow(cx, cy);
}

// ChgeSDIView 消息处理程序


void ChgeSDIView::OnLine()
{
	// TODO: 在此添加命令处理程序代码
	MainInterface::getInstance().OnCommand(COMM_LINE);
}


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
