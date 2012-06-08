// UIHistorySnapshotListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "hgeSDI.h"
#include "UIHistorySnapshotListCtrl.h"

#include "ColorManager.h"
#include "IconManager.h"
#include "SnapshotManager.h"

#define _HISTORYSSICONSIZE	ICMSIZE_LARGE

enum{
	IDHBSSC_NAME=0,
	IDHBSSC_ICON,

	_IDHBSSC_INDEXMAX,
};
// UIHistorySnapShotListCtrl

IMPLEMENT_DYNAMIC(UIHistorySnapshotListCtrl, CListCtrl)

UIHistorySnapshotListCtrl::UIHistorySnapshotListCtrl()
{
	nSelected = -1;
	topIndex = 0;
	displayindexcount=1;
}

UIHistorySnapshotListCtrl::~UIHistorySnapshotListCtrl()
{
}


BEGIN_MESSAGE_MAP(UIHistorySnapshotListCtrl, CListCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &UIHistorySnapshotListCtrl::OnNMCustomdraw)
	ON_NOTIFY_REFLECT(NM_CLICK, &UIHistorySnapshotListCtrl::OnNMClick)
	ON_WM_SIZE()
	ON_WM_NCCALCSIZE()
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &UIHistorySnapshotListCtrl::OnLvnItemchanged)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, &UIHistorySnapshotListCtrl::OnLvnEndlabeledit)
END_MESSAGE_MAP()



// UIHistorySnapShotListCtrl 消息处理程序




int UIHistorySnapshotListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	ColorManager * pcm = &ColorManager::getInstance();
	SetBkColor(pcm->ARGBToABGR(pcm->GetTextBkColor(COLORMT_LIST, COLORMS_BACKGROUND)) & 0xffffff);

	SetExtendedStyle(LVS_EX_FULLROWSELECT);

	InsertColumn(IDHBSSC_NAME, "", LVCFMT_LEFT, 0, IDHBSSC_NAME);
	InsertColumn(IDHBSSC_ICON, "", LVCFMT_LEFT, 0, IDHBSSC_ICON);

	IconManager * picm = &IconManager::getInstance();
	SetImageList(picm->GetImageList(_HISTORYSSICONSIZE), picm->GetImageListType(_HISTORYSSICONSIZE));
	
	int nOrder[_IDHBSSC_INDEXMAX];
	nOrder[0] = IDHBSSC_ICON;
	nOrder[1] = IDHBSSC_NAME;
	SetColumnOrderArray(_IDHBSSC_INDEXMAX, nOrder);
	

	return 0;
}

void UIHistorySnapshotListCtrl::AddSnapshot()
{
	int nSnapshot = SnapshotManager::getInstance().AddSnapshot();

	ASSERT(nSnapshot==topIndex);

	InsertItem(topIndex, "");
	CString str;
	str.Format("%s %d", StringManager::getInstance().GetSnapshotDefaultName(), displayindexcount);

	displayindexcount++;
	SetItemText(topIndex, IDHBSSC_NAME, str.GetBuffer());
	int nImg=IconManager::getInstance().GetBlankIcon();
	SetItem(topIndex, IDHBSSC_ICON, LVIF_IMAGE, NULL, nImg, 0, 0, 0L);

	EnsureVisible(topIndex, FALSE);
	topIndex++;
}

void UIHistorySnapshotListCtrl::DeleteSnapshot()
{
	if (nSelected >= 0)
	{
		SnapshotManager::getInstance().DeleteSnapshot(nSelected);
		DeleteItem(nSelected);
		topIndex--;
	}
}

void UIHistorySnapshotListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW *pCD = (NMLVCUSTOMDRAW*)pNMHDR;
	// TODO: 在此添加控件通知处理程序代码
	*pResult = CDRF_DODEFAULT;

	//obtain row and column of item
	int nowDrawingItem = pCD->nmcd.dwItemSpec;
	int nowDrawingSubItem = pCD->iSubItem;
	//	int iCol = pCD->iSubItem;

	//Remove standard highlighting of selected (sub)item.
	pCD->nmcd.uItemState = CDIS_DEFAULT;

	ColorManager * pcm = &ColorManager::getInstance();

	switch( pCD->nmcd.dwDrawStage )
	{
	case  CDDS_PREPAINT:  // First stage (for the whole control)
		{
			*pResult = CDRF_NOTIFYITEMDRAW;
		}
		break;

	case CDDS_ITEMPREPAINT:
		{
			*pResult = CDRF_NOTIFYSUBITEMDRAW;
		}
		break;

	case  CDDS_ITEMPREPAINT | CDDS_SUBITEM : // Stage three
		{
			if( (nSelected == nowDrawingItem ) )
			{
				pCD->clrText	= pcm->ARGBToABGR(pcm->GetTextColor(COLORMT_LIST, COLORMS_ACTIVE))&0xffffff;
				pCD->clrTextBk	= pcm->ARGBToABGR(pcm->GetTextBkColor(COLORMT_LIST, COLORMS_ACTIVE))&0xffffff;
			}
			else
			{
				pCD->clrText	= pcm->ARGBToABGR(pcm->GetTextColor(COLORMT_LIST, COLORMS_NONACTIVE))&0xffffff;
				pCD->clrTextBk	= pcm->ARGBToABGR(pcm->GetTextBkColor(COLORMT_LIST, COLORMS_NONACTIVE))&0xffffff;
			}
			*pResult =  CDRF_NOTIFYPOSTPAINT;
		}
		break;
	case CDDS_ITEMPOSTPAINT | CDDS_SUBITEM: // Stage four (called for each subitem of the focused item)
		{
			CRect nowDrawingRect;
			GetSubItemRect(nowDrawingItem, nowDrawingSubItem, LVIR_BOUNDS, nowDrawingRect);
			CClientDC dc(this);

			DWORD col = pcm->ARGBToABGR(pcm->GetTextBkColor(COLORMT_LIST, COLORMS_FRAME))&0xffffff;

			dc.Draw3dRect(nowDrawingRect, col, col);
		}
		break;
	}
}


void UIHistorySnapshotListCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	int newSelected = pNMItemActivate->iItem;

	if (newSelected >= 0)
	{
		if (nSelected != newSelected)
		{
			nSelected = newSelected;
		}
		if (SnapshotManager::getInstance().NeedLoad(newSelected))
		{
			ASSERT(true);
		}
		SnapshotManager::getInstance().RevertToSnapshot(newSelected);
	}

	Invalidate();
	*pResult = 0;
}


void UIHistorySnapshotListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	SetColumnWidth(IDHBSSC_ICON, _HISTORYSSICONSIZE);
	SetColumnWidth(IDHBSSC_NAME, cx-_HISTORYSSICONSIZE);

	PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
}


void UIHistorySnapshotListCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	ModifyStyle( WS_HSCROLL, 0 );
	CListCtrl::OnNcCalcSize(bCalcValidRects, lpncsp);
}


void UIHistorySnapshotListCtrl::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (pNMLV->uChanged & LVIF_STATE)
	{
		if (pNMLV->uNewState == LVIS_SELECTED)
		{
			nSelected = pNMLV->iItem;
		}
	}
	*pResult = 0;
}


void UIHistorySnapshotListCtrl::OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (pDispInfo->item.iSubItem == IDHBSSC_NAME)
	{
		if (strlen(pDispInfo->item.pszText))
		{
			SetItemText(pDispInfo->item.iItem, IDHBSSC_NAME, pDispInfo->item.pszText);
		}
	}
	*pResult = 0;
}
