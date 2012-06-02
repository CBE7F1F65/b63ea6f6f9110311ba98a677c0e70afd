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

// MainFrm.cpp : CMainFrame ���ʵ��
//

#include "stdafx.h"
#include "hgeSDI.h"

#include "MainFrm.h"
#include "StringManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_VIEW_CAPTION_BAR, &CMainFrame::OnViewCaptionBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CAPTION_BAR, &CMainFrame::OnUpdateViewCaptionBar)
	ON_COMMAND(ID_TOOLS_OPTIONS, &CMainFrame::OnOptions)
	ON_COMMAND(ID_FILE_PRINT, &CMainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CMainFrame::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, &CMainFrame::OnUpdateFilePrintPreview)
	ON_UPDATE_COMMAND_UI(ID_EDITCOMMAND, &CMainFrame::OnUpdateEditcommand)
	ON_UPDATE_COMMAND_UI(ID_EDITCOMMANDLOG, &CMainFrame::OnUpdateEditcommandlog)
	ON_UPDATE_COMMAND_UI(ID_STATUSBAR_PANE1, &CMainFrame::OnUpdateStatusbarPane1)
	ON_UPDATE_COMMAND_UI(ID_STATUSBAR_PANE2, &CMainFrame::OnUpdateStatusbarPane2)
	ON_COMMAND(ID_VIEW_COMMAND_PANE, &CMainFrame::OnViewCommandPane)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COMMAND_PANE, &CMainFrame::OnUpdateViewCommandPane)
END_MESSAGE_MAP()

// CMainFrame ����/����

CMainFrame::CMainFrame()
{
//	_CrtSetBreakAlloc(26907);
	// TODO: �ڴ���ӳ�Ա��ʼ������
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLACK);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

//	BOOL bNameValid;
	// ���ڳ־�ֵ�����Ӿ�����������ʽ
	OnApplicationLook(theApp.m_nAppLook);

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	if (!m_wndStatusBar.Create(this))
	{
//		TRACE0("δ�ܴ���״̬��\n");
		return -1;      // δ�ܴ���
	}
	/*
	CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);
	*/
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, StringManager::getInstance().GetStatusPaneLeftName()/*strTitlePane1*/, TRUE), StringManager::getInstance().GetStatusPaneLeftName()/*strTitlePane1*/);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, StringManager::getInstance().GetStatusPaneRightName()/*strTitlePane2*/, TRUE), /*strTitlePane2*/StringManager::getInstance().GetStatusPaneRightName());
	
	RECT rect;
	GetClientRect(&rect);


	int cwidth = rect.right-rect.left;
	int cheight = rect.bottom-rect.top;

	m_wndUICommandPane.Create(StringManager::getInstance().GetCommandPanelName(), this, CRect (0, 0, cwidth, cheight/4),
		TRUE /* Has gripper */, ID_UI_COMMANDPANE,
		WS_CHILD | WS_VISIBLE | CBRS_BOTTOM | CBRS_FLOAT_MULTI);

	m_wndUICommandPane.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndUICommandPane);

	m_wndUIHistoryPane.Create(StringManager::getInstance().GetHistoryPanelName(), this, CRect (0, 0, cwidth/6, cheight/2),
		TRUE /* Has gripper */, ID_UI_HISTORYPANE,
		WS_CHILD | WS_VISIBLE | CBRS_RIGHT | CBRS_FLOAT_MULTI);
	m_wndUIHistoryPane.EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndUIHistoryPane);

	m_wndUILayerPane.Create(StringManager::getInstance().GetLayerPanelName(), this, CRect (0, cheight/2, cwidth/6, cheight/2),
		TRUE /* Has gripper */, ID_UI_LAYERPANE,
		WS_CHILD | WS_VISIBLE | CBRS_BOTTOM | CBRS_FLOAT_MULTI);
	m_wndUILayerPane.EnableDocking(CBRS_ALIGN_ANY);
	m_wndUILayerPane.DockToWindow(&m_wndUIHistoryPane, CBRS_ALIGN_BOTTOM);

	m_wndUIFloatingEdit.Create(WS_CHILD|WS_VISIBLE, CRect(0, 0, 0, 0), this, ID_UI_FLOATINGCOMMAND);
/*	
	m_wndUIFloatingCommand.Create(NULL,
		WS_CHILD | WS_VISIBLE | CBRS_FLOAT_MULTI,
		CRect(0, 0, 0, 0), this, ID_UI_FLOATINGCOMMAND);
	*/	

//	m_wndUIFloatingCommand.ShowWindow(SW_HIDE);

	// ���� Visual Studio 2005 ��ʽͣ��������Ϊ
	CDockingManager::SetDockingMode(DT_SMART);
	// ���� Visual Studio 2005 ��ʽͣ�������Զ�������Ϊ
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// ����������:
	if (!CreateCaptionBar())
	{
//		TRACE0("δ�ܴ���������\n");
		return -1;      // δ�ܴ���
	}

	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	return m_wndSplitter.Create(this,
		2, 2,               // TODO: ��������������
		CSize(10, 10),      // TODO: ������С�����С
		pContext);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return TRUE;
}

BOOL CMainFrame::CreateCaptionBar()
{
	if (!m_wndCaptionBar.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, this, ID_VIEW_CAPTION_BAR, -1, TRUE))
	{
//		TRACE0("δ�ܴ���������\n");
		return FALSE;
	}
	/*
	BOOL bNameValid;

	CString strTemp, strTemp2;
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButton(strTemp, ID_TOOLS_OPTIONS, CMFCCaptionBar::ALIGN_LEFT, FALSE);
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON_TIP);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButtonToolTip(strTemp);

	bNameValid = strTemp.LoadString(IDS_CAPTION_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetText(strTemp, CMFCCaptionBar::ALIGN_LEFT);

	m_wndCaptionBar.SetBitmap(IDB_INFO, RGB(255, 255, 255), FALSE, CMFCCaptionBar::ALIGN_LEFT);
	bNameValid = strTemp.LoadString(IDS_CAPTION_IMAGE_TIP);
	ASSERT(bNameValid);
	bNameValid = strTemp2.LoadString(IDS_CAPTION_IMAGE_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetImageToolTip(strTemp, strTemp2);
	*/
	return TRUE;
}

// CMainFrame ���

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame ��Ϣ�������

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(TRUE);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
		m_wndRibbonBar.SetWindows7Look(FALSE);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

void CMainFrame::OnViewCaptionBar()
{
	m_wndCaptionBar.ShowWindow(m_wndCaptionBar.IsVisible() ? SW_HIDE : SW_SHOW);
	RecalcLayout(FALSE);
}

void CMainFrame::OnUpdateViewCaptionBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndCaptionBar.IsVisible());
}

void CMainFrame::OnViewCommandPane()
{
	// TODO: �ڴ���������������
	BOOL visible = m_wndUICommandPane.IsPaneVisible();
	m_wndUICommandPane.ShowPane(visible?FALSE:TRUE, TRUE, FALSE);
//	m_wndUICommandPane.ShowWindow(m_wndUICommandPane.IsVisible() ? SW_HIDE : SW_SHOW);
	RecalcLayout(FALSE);
}

void CMainFrame::OnUpdateViewCommandPane(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->SetCheck(m_wndUICommandPane.IsPaneVisible());
}


void CMainFrame::OnOptions()
{
	CMFCRibbonCustomizeDialog *pOptionsDlg = new CMFCRibbonCustomizeDialog(this, &m_wndRibbonBar);
	ASSERT(pOptionsDlg != NULL);

	pOptionsDlg->DoModal();
	delete pOptionsDlg;
}

void CMainFrame::OnFilePrint()
{
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_PRINT);
	}
}

void CMainFrame::OnFilePrintPreview()
{
	if (IsPrintPreview())
	{
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_CLOSE);  // ǿ�ƹرա���ӡԤ����ģʽ
	}
}

void CMainFrame::OnUpdateFilePrintPreview(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(IsPrintPreview());
}

bool CMainFrame::SetStatusBarText( int id, LPCTSTR text )
{
	CMFCRibbonBaseElement * element = NULL;
	switch (id)
	{
	case IDS_STATUS_PANE1:
		element = m_wndStatusBar.GetElement(0);
		break;
	case IDS_STATUS_PANE2:
		element = m_wndStatusBar.GetExElement(0);
		break;
	}
	if (element)
	{
//		CSize size = GetDC()->GetTextExtent(text);
		element->SetText(text);
		m_wndStatusBar.RecalcLayout();
//		element->Redraw();
		m_wndStatusBar.RedrawWindow();
		return true;
	}
	return false;
}

bool CMainFrame::AppendCommandLogText( LPCTSTR text, bool bNewLine/*=true*/ )
{
	m_wndUICommandPane.AppendCommandLogText(text, bNewLine);
	return true;
}

bool CMainFrame::SetCommandText( LPCTSTR text, bool bActivate/*=true*/ )
{
	m_wndUICommandPane.SetCommandText(text, bActivate);
	return true;
}

void CMainFrame::OnUpdateEditcommand(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->Enable(TRUE);
}


void CMainFrame::OnUpdateEditcommandlog(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->Enable(TRUE);
}


void CMainFrame::OnUpdateStatusbarPane1(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->Enable(TRUE);
}


void CMainFrame::OnUpdateStatusbarPane2(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->Enable(TRUE);
}

void CMainFrame::CallShowContextMenu( int x, int y )
{
	CMenu menu;
	menu.LoadMenu(IDR_POPUP_EDIT);
	HMENU hmenuPopup = ::GetSubMenu(menu.Detach(), 0);
	m_wndUIPopupMenu.SetAutoDestroy(FALSE);
	m_wndUIPopupMenu.Create(this, x, y, hmenuPopup);
}


void CMainFrame::CallEnableFloatCommand( int vk/*=0*/ )
{
	m_wndUIFloatingEdit.Show(vk);
}

bool CMainFrame::AddHistory( const char * desc, const char * commandstr )
{
	return m_wndUIHistoryPane.AddHistory(desc, commandstr);
}

bool CMainFrame::ChangeCurrentHistory( int step )
{
	return m_wndUIHistoryPane.ChangeCurrentHistory(step);
}

bool CMainFrame::ClearLaterHistory( int ndelete )
{
	return m_wndUIHistoryPane.ClearLaterHistory(ndelete);
}

bool CMainFrame::ClearPreviousHistory( int ndelete )
{
	return m_wndUIHistoryPane.ClearPreviousHistory(ndelete);
}