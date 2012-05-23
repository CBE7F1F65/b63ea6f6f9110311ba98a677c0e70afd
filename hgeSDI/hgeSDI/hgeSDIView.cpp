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

void ChgeSDIView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
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
