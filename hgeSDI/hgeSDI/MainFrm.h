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

// MainFrm.h : CMainFrame ��Ľӿ�
//

#pragma once

#include "UICommandDockablePane.h"
#include "UIPopupMenu.h"
#include "UIFloatingEditBox.h"
#include "UISplitterWnd.h"

class CMainFrame : public CFrameWndEx
{
	
protected: // �������л�����
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// ����
protected:
	UISplitterWnd m_wndSplitter;
public:

	bool SetStatusBarText(int id, LPCTSTR text);
	bool AppendCommandLogText(LPCTSTR text, bool bNewLine=true);

// ����
public:
	void CallShowContextMenu(int x, int y);
	void CallEnableFloatCommand(int vk=0);

// ��д
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// ʵ��
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // �ؼ���Ƕ���Ա
public:
	CMFCRibbonBar     m_wndRibbonBar;
	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;
	CMFCCaptionBar    m_wndCaptionBar;

	UICommandDockablePane		m_wndUICommandPane;
	UIPopupMenu			m_wndUIPopupMenu;
	UIFloatingEditBox			m_wndUIFloatingEdit;

// ���ɵ���Ϣӳ�亯��
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
};


