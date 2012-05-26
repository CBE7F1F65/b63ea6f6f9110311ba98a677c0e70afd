// ��� MFC ʾ��Դ������ʾ���ʹ�� MFC Microsoft Office Fluent �û����� 
// (��Fluent UI��)����ʾ�������ο���
// ���Բ��䡶Microsoft ������ο����� 
// MFC C++ ������渽����ص����ĵ���
// ���ơ�ʹ�û�ַ� Fluent UI ����������ǵ����ṩ�ġ�
// ��Ҫ�˽��й� Fluent UI ��ɼƻ�����ϸ��Ϣ�������  
// http://msdn.microsoft.com/officeui��
//
// ��Ȩ����(C) Microsoft Corporation
// ��������Ȩ����

// hgeSDIView.cpp : ChgeSDIView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
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
	// ��׼��ӡ����
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
//	ON_WM_MOUSEMOVE()
ON_WM_HSCROLL()
ON_WM_VSCROLL()
ON_WM_CREATE()
END_MESSAGE_MAP()

// ChgeSDIView ����/����

ChgeSDIView::ChgeSDIView()
{
	// TODO: �ڴ˴���ӹ������

}

ChgeSDIView::~ChgeSDIView()
{
}

BOOL ChgeSDIView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// ChgeSDIView ����

void ChgeSDIView::OnDraw(CDC* /*pDC*/)
{
	ChgeSDIDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
}


// ChgeSDIView ��ӡ


void ChgeSDIView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL ChgeSDIView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void ChgeSDIView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void ChgeSDIView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
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


// ChgeSDIView ���

#ifdef _DEBUG
void ChgeSDIView::AssertValid() const
{
	CView::AssertValid();
}

void ChgeSDIView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

ChgeSDIDoc* ChgeSDIView::GetDocument() const // �ǵ��԰汾��������
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

// ChgeSDIView ��Ϣ�������


void ChgeSDIView::OnLine()
{
	// TODO: �ڴ���������������
	MainInterface::getInstance().OnCommand(COMM_LINE);
}


void ChgeSDIView::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CView::OnActivate(nState, pWndOther, bMinimized);

	// TODO: �ڴ˴������Ϣ����������
	/*
	*/
}


void ChgeSDIView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CView::OnTimer(nIDEvent);
	MainInterface::getInstance().OnUpdateTimer();
	
}


void ChgeSDIView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	// TODO: �ڴ����ר�ô����/����û���

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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	MainInterface::getInstance().OnMouseActivate();
	return CView::OnMouseActivate(pDesktopWnd, nHitTest, message);
}


void ChgeSDIView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: �ڴ����ר�ô����/����û���
}


//void ChgeSDIView::OnMouseMove(UINT nFlags, CPoint point)
//{
//	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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

	// TODO:  �ڴ������ר�õĴ�������
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
