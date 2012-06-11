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

// MainFrm.h : CMainFrame 类的接口
//

#pragma once

#include "UICommandDockablePane.h"
#include "UIPopupMenu.h"
#include "UIFloatingEditBox.h"
#include "UISplitterWnd.h"
#include "UIHistoryDockablePane.h"
#include "UILayerDockablePane.h"

#include "Command.h"
#include "GLayer.h"

class CMainFrame : public CFrameWndEx
{
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 特性
protected:
	UISplitterWnd m_wndSplitter;
public:

	bool SetStatusBarText(int id, LPCTSTR text);
	bool AppendCommandLogText(LPCTSTR text, bool bNewLine=true);
	bool SetCommandText(LPCTSTR text, bool bActivate=true);

	bool AddHistory(const char * desc, const char * commandstr, int command);
	bool ChangeCurrentHistory(int step);
	bool ClearLaterHistory(int ndelete);
	bool ClearPreviousHistory(int ndelete);

	bool RebuildLayerTree(GObject * changebase, GObject * activeitem);

	GLayer * GetActiveLayer();
	GObject * GetActiveNodes( int * pnextfromIndex );
	bool GetDragDropNodes( GLayer ** pLayerNode, GObject ** pAfterNode );
	void SetActiveLayer_Internal( GLayer * pLayer );

// 操作
public:
	void CallShowContextMenu(int x, int y);
	void CallEnableFloatCommand(int vk=0);

// 重写
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
public:
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;
	CMFCCaptionBar    m_wndCaptionBar;

	UICommandDockablePane		m_wndUICommandPane;
	UIPopupMenu			m_wndUIPopupMenu;
	UIFloatingEditBox			m_wndUIFloatingEdit;

	UIHistoryDockablePane		m_wndUIHistoryPane;

	UILayerDockablePane			m_wndUILayerPane;

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnViewCaptionBar();
	afx_msg void OnUpdateViewCaptionBar(CCmdUI* pCmdUI);
	afx_msg void OnOptions();
	afx_msg void OnFilePrint();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

	BOOL CreateCaptionBar();
public:
	afx_msg void OnUpdateEditcommand(CCmdUI *pCmdUI);
	afx_msg void OnUpdateEditcommandlog(CCmdUI *pCmdUI);
	afx_msg void OnUpdateStatusbarPane1(CCmdUI *pCmdUI);
	afx_msg void OnUpdateStatusbarPane2(CCmdUI *pCmdUI);

	afx_msg void OnViewCommandPane();
	afx_msg void OnUpdateViewCommandPane(CCmdUI *pCmdUI);
	afx_msg void OnClose();
	afx_msg void OnViewHistoryPane();
	afx_msg void OnUpdateViewHistoryPane(CCmdUI *pCmdUI);
	afx_msg void OnViewLayerPane();
	afx_msg void OnUpdateViewLayerPane(CCmdUI *pCmdUI);
	afx_msg void OnCommandLayerNew();
	afx_msg void OnCommandNewsublayer();
	afx_msg void OnCommandLine();
	afx_msg void OnCommandDeleteitem();
	afx_msg void OnCommandReparent(UINT nID);
	afx_msg void OnUpdateCommandReparent(CCmdUI *pCmdUI);
};