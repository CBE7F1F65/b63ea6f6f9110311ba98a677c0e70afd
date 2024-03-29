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

// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "hgeSDI.h"

#include "MainFrm.h"
#include "StringManager.h"
#include "Main.h"
#include "GObjectManager.h"

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
	ON_WM_CLOSE()
	ON_COMMAND(ID_VIEW_HISTORY_PANE, &CMainFrame::OnViewHistoryPane)
	ON_UPDATE_COMMAND_UI(ID_VIEW_HISTORY_PANE, &CMainFrame::OnUpdateViewHistoryPane)
	ON_COMMAND(ID_VIEW_LAYER_PANE, &CMainFrame::OnViewLayerPane)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LAYER_PANE, &CMainFrame::OnUpdateViewLayerPane)
	ON_COMMAND(ID_COMMAND_NEWLAYER, &CMainFrame::OnCommandLayerNew)
	ON_COMMAND(ID_COMMAND_NEWSUBLAYER, &CMainFrame::OnCommandNewsublayer)
	ON_COMMAND(ID_COMMAND_LINE, &CMainFrame::OnCommandLine)
	ON_COMMAND(ID_COMMAND_DELETEITEM, &CMainFrame::OnCommandDeleteitem)
	ON_COMMAND_RANGE(ID_COMMAND_REPARENT, ID_COMMAND_REPARENT_END, &CMainFrame::OnCommandReparent)
	ON_UPDATE_COMMAND_UI_RANGE(ID_COMMAND_REPARENT, ID_COMMAND_REPARENT_END, &CMainFrame::OnUpdateCommandReparent)
END_MESSAGE_MAP()

// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
//	_CrtSetBreakAlloc(41586);
	// TODO: 在此添加成员初始化代码
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLACK);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	MainInterface::getInstance().OnPreInit();

//	BOOL bNameValid;
	// 基于持久值设置视觉管理器和样式
	OnApplicationLook(theApp.m_nAppLook);

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	if (!m_wndStatusBar.Create(this))
	{
//		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
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

	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// 创建标题栏:
	if (!CreateCaptionBar())
	{
//		TRACE0("未能创建标题栏\n");
		return -1;      // 未能创建
	}

	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	return m_wndSplitter.Create(this,
		2, 2,               // TODO: 调整行数和列数
		CSize(10, 10),      // TODO: 调整最小窗格大小
		pContext);
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return TRUE;
}

BOOL CMainFrame::CreateCaptionBar()
{
	if (!m_wndCaptionBar.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, this, ID_VIEW_CAPTION_BAR, -1, TRUE))
	{
//		TRACE0("未能创建标题栏\n");
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

// CMainFrame 诊断

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

// CMainFrame 消息处理程序

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
		PostMessage(WM_COMMAND, AFX_ID_PREVIEW_CLOSE);  // 强制关闭“打印预览”模式
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
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnUpdateEditcommandlog(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnUpdateStatusbarPane1(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(TRUE);
}

void CMainFrame::OnUpdateStatusbarPane2(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
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

bool CMainFrame::AddHistory( const char * desc, const char * commandstr, int command )
{
	return m_wndUIHistoryPane.AddHistory(desc, commandstr, command);
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

void CMainFrame::LockTreeChange( bool toLock )
{
	m_wndUILayerPane.LockTreeChange(toLock);
}

bool CMainFrame::RebuildLayerTree( GObject * changebase, GObject * activeitem )
{
	m_wndUILayerPane.RebuildTree(changebase,activeitem);
	return true;
}

GObject * CMainFrame::GetActiveNodes( int * pnextfromIndex )
{
	return m_wndUILayerPane.GetActiveNodes(pnextfromIndex);
}

bool CMainFrame::GetDragDropNodes( GLayer ** pLayerNode, GObject ** pAfterNode )
{
	return m_wndUILayerPane.GetDragDropNodes(pLayerNode, pAfterNode);
}

void CMainFrame::SetActiveLayer_Internal( GLayer * pLayer )
{
	return m_wndUILayerPane.SetActiveLayer_Internal(pLayer);
}

void CMainFrame::ChangeNode( GObject * pObj )
{
	m_wndUILayerPane.ChangeNode(pObj);
}

void CMainFrame::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	MainInterface::getInstance().Exit();

	CFrameWndEx::OnClose();
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
	// TODO: 在此添加命令处理程序代码
	BOOL visible = m_wndUICommandPane.IsPaneVisible();
	m_wndUICommandPane.ShowPane(visible?FALSE:TRUE, TRUE, FALSE);
	RecalcLayout(FALSE);
}

void CMainFrame::OnUpdateViewCommandPane(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_wndUICommandPane.IsPaneVisible());
}

void CMainFrame::OnViewHistoryPane()
{
	// TODO: 在此添加命令处理程序代码
	BOOL visible = m_wndUIHistoryPane.IsPaneVisible();
	m_wndUIHistoryPane.ShowPane(visible?FALSE:TRUE, TRUE, FALSE);
	RecalcLayout(FALSE);
}


void CMainFrame::OnUpdateViewHistoryPane(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_wndUIHistoryPane.IsPaneVisible());
}


void CMainFrame::OnViewLayerPane()
{
	// TODO: 在此添加命令处理程序代码
	BOOL visible = m_wndUILayerPane.IsPaneVisible();
	m_wndUILayerPane.ShowPane(visible?FALSE:TRUE, TRUE, FALSE);
	RecalcLayout(FALSE);
}


void CMainFrame::OnUpdateViewLayerPane(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(m_wndUILayerPane.IsPaneVisible());
}


void CMainFrame::OnCommandLayerNew()
{
	// TODO: 在此添加命令处理程序代码
	MainInterface::getInstance().OnCommand(COMM_NEWLAYER);
//	GObjectManager::getInstance().NewLayer(GObjectManager::getInstance().GetActiveLayer());
}


void CMainFrame::OnCommandNewsublayer()
{
	// TODO: 在此添加命令处理程序代码
	MainInterface::getInstance().OnCommand(COMM_NEWSUBLAYER);
//	GObjectManager::getInstance().NewSubLayer(GObjectManager::getInstance().GetActiveLayer());
}


void CMainFrame::OnCommandLine()
{
	// TODO: 在此添加命令处理程序代码
	MainInterface::getInstance().OnCommand(COMM_LINE);
}

void CMainFrame::OnCommandDeleteitem()
{
	// TODO: 在此添加命令处理程序代码
	MainInterface::getInstance().OnCommand(COMM_DELETEITEM);
}

#include "UILayerListCtrl.h"
afx_msg void CMainFrame::OnCommandReparent( UINT nID )
{
	// TODO: 在此添加命令处理程序代码
	int op = LBCOMMAND_REPARENT_NONE;
	switch (nID)
	{
	case ID_COMMAND_REPARENT_UPLAYER:
		op = LBCOMMAND_REPARENT_UPLAYER;
		break;
	case ID_COMMAND_REPARENT_DOWNLAYER:
		op = LBCOMMAND_REPARENT_DOWNLAYER;
		break;
	case ID_COMMAND_REPARENT_UP:
		op = LBCOMMAND_REPARENT_UP;
		break;
	case ID_COMMAND_REPARENT_DOWN:
		op = LBCOMMAND_REPARENT_DOWN;
		break;
	}
	m_wndUILayerPane.CallReparentSelectedNodes(op);
}


void CMainFrame::OnUpdateCommandReparent(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(TRUE);
}