// UIDockablePaneButton.cpp : 实现文件
//

#include "stdafx.h"
#include "hgeSDI.h"
#include "UIDockablePaneButton.h"

#include "IconManager.h"


// UIDockablePaneButton

IMPLEMENT_DYNAMIC(UIDockablePaneButton, CButton)

UIDockablePaneButton::UIDockablePaneButton()
{
	m_pToolTip = NULL;
}

UIDockablePaneButton::~UIDockablePaneButton()
{
	if (m_pToolTip)
	{
		delete m_pToolTip;
	}
}


BEGIN_MESSAGE_MAP(UIDockablePaneButton, CButton)
	ON_WM_CREATE()
	ON_WM_DRAWITEM()
//	ON_CONTROL_REFLECT(BN_CLICKED, &UIDockablePaneButton::OnBnClicked)
END_MESSAGE_MAP()



// UIDockablePaneButton 消息处理程序




int UIDockablePaneButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CButton::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pToolTip = new CToolTipCtrl;
	if(!m_pToolTip->Create(this))
	{
		return -1;
	}
	bEnabled = true;

	EnableWindow();
	// TODO:  在此添加您专用的创建代码

	return 0;
}

/*
void UIDockablePaneButton::OnBnClicked()
{
	// TODO: 在此添加控件通知处理程序代码
}
*/

#include "ColorManager.h"
void UIDockablePaneButton::DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/)
{
	CRect rect;
	GetClientRect(&rect);
	CClientDC dc(this);

	bool bclicking = GetState()&0x0004;
	if (!bEnabled)
	{
		bclicking = false;
	}

	ColorManager * pcm = &ColorManager::getInstance();
	DWORD bkcol;
	if (!bclicking)
	{
		bkcol = pcm->ARGBToABGR(pcm->GetTextBkColor(COLORMT_LIST, COLORMS_BACKGROUND))&0xffffff;
	}
	else
	{
		bkcol = pcm->ARGBToABGR(pcm->GetTextBkColor(COLORMT_LIST, COLORMS_NONEEDITABLE))&0xffffff;
	}
	DWORD framecol = pcm->ARGBToABGR(pcm->GetTextBkColor(COLORMT_LIST, COLORMS_FRAME))&0xffffff;

	dc.FillSolidRect(rect, bkcol);

	HICON hIcon = hIconNormal;
	if (!bEnabled)
	{
		hIcon = hIconDisabled;
	}
	dc.DrawIcon(0, 0, hIcon);

	dc.Draw3dRect(rect, framecol, framecol);

	if (bclicking)
	{
		rect.DeflateRect(1, 1, 1, 1);
		dc.Draw3dRect(rect, framecol, framecol);
	}


	// TODO:  添加您的代码以绘制指定项
}

BOOL UIDockablePaneButton::DCreate( int id, CWnd * pParent, const char * tooltip/*=""*/, int icmsize/*=-1*/, int img/*=-1*/, int imgdisabled/*=-1*/ )
{
	if (!Create("", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON|BS_OWNERDRAW, CRect(0, 0, 0, 0), pParent, id))
	{
		return FALSE;
	}


	if (icmsize >= 0)
	{
		CImageList * pImgList = IconManager::getInstance().GetImageList(icmsize);
		if (img >= 0)
		{
			hIconNormal = ImageList_GetIcon(pImgList->m_hImageList, img, ILD_TRANSPARENT);
		}
		if (imgdisabled >= 0)
		{
			hIconDisabled = ImageList_GetIcon(pImgList->m_hImageList, imgdisabled, ILD_TRANSPARENT);
		}
	}

	if (strlen(tooltip))
	{
		m_pToolTip->AddTool(this, tooltip);
		m_pToolTip->Activate(TRUE);
	}
	return TRUE;
}

BOOL UIDockablePaneButton::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (NULL != m_pToolTip)
	{
		m_pToolTip->RelayEvent(pMsg);
	}

	return CButton::PreTranslateMessage(pMsg);
}

void UIDockablePaneButton::SetButtonEnable( bool bEnable/*=true*/ )
{
	bEnabled = bEnable;
	EnableWindow(bEnable);
}