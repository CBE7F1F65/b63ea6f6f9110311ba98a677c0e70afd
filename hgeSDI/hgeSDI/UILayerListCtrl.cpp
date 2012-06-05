// UILayerListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "hgeSDI.h"
#include "UILayerListCtrl.h"

#include "ColorManager.h"
#include "IconManager.h"
#include "GObjectManager.h"

#define _LAYERLIST_ICONSIZE	16
#define _LAYERLIST_LINECOLORSIZE	6

// UILayerListCtrl

enum{
	IDLBC_TREE=0,
	IDLBC_VISIBLE,
	IDLBC_LOCK,
	IDLBC_LINECOLOR,
	IDLBC_LINESELECT,
	IDLBC_FRAMECOLOR,

	_IDLBC_INDEXMAX,
};

IMPLEMENT_DYNAMIC(UILayerListCtrl, CListCtrl)

UILayerListCtrl::UILayerListCtrl()
{
}

UILayerListCtrl::~UILayerListCtrl()
{
}


BEGIN_MESSAGE_MAP(UILayerListCtrl, CListCtrl)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY_REFLECT(NM_CLICK, &UILayerListCtrl::OnNMClick)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &UILayerListCtrl::OnNMCustomdraw)
	ON_NOTIFY_REFLECT(NM_DBLCLK, &UILayerListCtrl::OnNMDblclk)
	ON_WM_NCCALCSIZE()
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGING, &UILayerListCtrl::OnLvnItemchanging)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &UILayerListCtrl::OnLvnItemchanged)
END_MESSAGE_MAP()

void UILayerListCtrl::RebuildTree( GObject * changebase, GObject * activeitem )
{
	SetRedraw(FALSE);
//	DeleteAllItems();
	DeleteItemsUnderObj(changebase);

	GObjectManager * pgm = &GObjectManager::getInstance();

	int nowindex = FindItemByData(changebase);

	if (nowindex < 0)
	{
		nowindex = -1;
	}
	int nowindent = GetItemIndent(nowindex);

	nowindex++;
	BuildChildren(changebase, nowindex, nowindent+1);

	DeSelect(-1);
	int activeindex = FindItemByData(activeitem);
	if (activeindex >= 0)
	{
		AddSelect(activeindex);
		EnsureVisible(activeindex, FALSE);
	}
	else
	{
		ASSERT(true);
	}
	Invalidate();
	SetRedraw();
}

void UILayerListCtrl::DeleteItemsUnderObj( GObject * obj )
{
	int objindex = FindItemByData(obj);
	if (objindex >= 0)
	{
		int objindent = GetItemIndent(objindex);
		objindex++;
		while (GetItemIndent(objindex) > objindent)
		{
			DeleteItem(objindex);
		}
	}
	else
	{
		DeleteAllItems();
	}
}

void UILayerListCtrl::BuildChildren( GObject * nowbase, int & nowindex, int indentlevel )
{
	if (nowbase)
	{
		if (!(nowbase->getChildren()->empty()) && nowbase->getNonAttributeChildrenCount())
		{
			for (list<GObject *>::iterator it=nowbase->getChildren()->begin(); it!=nowbase->getChildren()->end(); ++it)
			{
				bool bFolded = (*it)->isDisplayFolded();

				InsertItem(nowindex, "");
				SetItemData(nowindex, (DWORD_PTR)(*it));

				SetItemVisible(nowindex, (*it)->isDisplayVisible(), (*it)->isRecDisplayVisible());
				SetItemLock(nowindex, (*it)->isDisplayLocked(), (*it)->isRecDisplayLocked());
				SetItemLineColor(nowindex, (*it)->getLineColor());
				SetItemTreeInfo(nowindex, (*it)->getDisplayName(), indentlevel, bFolded);
				SetItemLineSelect(nowindex, (*it)->isSelected());
				SetItemFrameColor(nowindex, (*it)->GetLayer()->getLineColor());
				/*
				SetItem(nowindex, IDLBC_VISIBLE, LVIF_IMAGE, "", 0, 0, 0, 0);
				SetItem(nowindex, IDLBC_LOCK, LVIF_IMAGE, "", 16, 0, 0, 0);
				SetItem(nowindex, IDLBC_LINECOLOR, LVIF_IMAGE, "", 32, 0, 0, 0);
				SetItem(nowindex, IDLBC_TREE, LVIF_TEXT|LVIF_IMAGE|LVIF_INDENT, (*it)->getDisplayName(), 63, 0, 0, 0, indentlevel);
				SetItem(nowindex, IDLBC_SELECT, LVIF_IMAGE, "", 32, 0, 0, 0);
				SetItem(nowindex, IDLBC_FRAMECOLOR, LVIF_IMAGE, "", 16, 0, 0, 0);
				*/


				nowindex++;

				if (!bFolded)
				{
					BuildChildren(*it, nowindex, indentlevel+1);
				}
			}
		}
	}
}


// UILayerListCtrl 消息处理程序




int UILayerListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	ColorManager * pcm = &ColorManager::getInstance();
	SetBkColor(pcm->ARGBToABGR(pcm->GetTextBkColor(COLORMT_LIST, COLORMS_BACKGROUND)) & 0xffffff);
	SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES);

	InsertColumn(IDLBC_VISIBLE, "", LVCFMT_LEFT);
	InsertColumn(IDLBC_LOCK, "", LVCFMT_LEFT);
	InsertColumn(IDLBC_LINECOLOR, "", LVCFMT_LEFT);
	InsertColumn(IDLBC_TREE, "", LVCFMT_LEFT);
	InsertColumn(IDLBC_LINESELECT, "", LVCFMT_LEFT);
	InsertColumn(IDLBC_FRAMECOLOR, "", LVCFMT_LEFT);

//	LPINT pnOrder = (LPINT) malloc(_IDLBC_INDEXMAX*sizeof(int));
	int nOrder[_IDLBC_INDEXMAX];
	nOrder[0] = IDLBC_VISIBLE;
	nOrder[1] = IDLBC_LOCK;
	nOrder[2] = IDLBC_LINECOLOR;
	nOrder[3] = IDLBC_TREE;
	nOrder[4] = IDLBC_LINESELECT;
	nOrder[5] = IDLBC_FRAMECOLOR;

	SetColumnOrderArray(_IDLBC_INDEXMAX, nOrder);

	SetColumnWidth(IDLBC_VISIBLE, _LAYERLIST_ICONSIZE);
	SetColumnWidth(IDLBC_LOCK, _LAYERLIST_ICONSIZE);
	SetColumnWidth(IDLBC_LINECOLOR, _LAYERLIST_LINECOLORSIZE);
	SetColumnWidth(IDLBC_LINESELECT, _LAYERLIST_ICONSIZE);
	SetColumnWidth(IDLBC_FRAMECOLOR, _LAYERLIST_ICONSIZE);

	IconManager * picm = &IconManager::getInstance();
	SetImageList(picm->GetImageList(ICMSIZE_SMALL), picm->GetImageListType(ICMSIZE_SMALL));

	return 0;
}


void UILayerListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);

	int otherwidth = 0;
	otherwidth += GetColumnWidth(IDLBC_VISIBLE);
	otherwidth += GetColumnWidth(IDLBC_LOCK);
	otherwidth += GetColumnWidth(IDLBC_LINECOLOR);
	otherwidth += GetColumnWidth(IDLBC_LINESELECT);
	otherwidth += GetColumnWidth(IDLBC_FRAMECOLOR);
	SetColumnWidth(IDLBC_TREE, cx-otherwidth);
//	EnsureVisible(nSelected, FALSE);
	// TODO: 在此处添加消息处理程序代码
}


void UILayerListCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	int newSelected = pNMItemActivate->iItem;

	if (pNMItemActivate->iSubItem == IDLBC_TREE)
	{
		if (newSelected >= 0)
		{
			DeSelectParadox(newSelected);
//			AddSelect();
		}

	}
	Invalidate();
	*pResult = 0;
}


void UILayerListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW *pCD = (NMLVCUSTOMDRAW*)pNMHDR;
	// By default set the return value to do the default behavior.
	*pResult = CDRF_DODEFAULT;

	//obtain row and column of item
	int nowDrawingItem = pCD->nmcd.dwItemSpec;
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
			bool bSelected = (GetItemState(nowDrawingItem, LVIS_SELECTED)&LVIS_SELECTED);

			if (bSelected)
			{
				pCD->clrText	= pcm->ARGBToABGR(pcm->GetTextColor(COLORMT_LIST, COLORMS_ACTIVE))&0xffffff;
				pCD->clrTextBk	= pcm->ARGBToABGR(pcm->GetTextBkColor(COLORMT_LIST, COLORMS_ACTIVE))&0xffffff;
			}
			else
			{
				GObject * pobj = (GObject *)GetItemData(nowDrawingItem);
				if (pobj->isDisplayLocked() || !(pobj->isDisplayVisible()))
				{
					pCD->clrText	= pcm->ARGBToABGR(pcm->GetTextColor(COLORMT_LIST, COLORMS_DISABLED))&0xffffff;
					pCD->clrTextBk	= pcm->ARGBToABGR(pcm->GetTextBkColor(COLORMT_LIST, COLORMS_DISABLED))&0xffffff;
				}
				else
				{
					pCD->clrText	= pcm->ARGBToABGR(pcm->GetTextColor(COLORMT_LIST, COLORMS_EDITABLE))&0xffffff;
					pCD->clrTextBk	= pcm->ARGBToABGR(pcm->GetTextBkColor(COLORMT_LIST, COLORMS_EDITABLE))&0xffffff;
				}
			}
			*pResult =  CDRF_NOTIFYPOSTPAINT;

			//
		}
		break;

	case CDDS_ITEMPOSTPAINT | CDDS_SUBITEM: // Stage four (called for each subitem of the focused item)
		{
			RECT rect;
			GetItemRect(nowDrawingItem, &rect, LVIR_BOUNDS);
			int height = rect.bottom-rect.top;
			int width = rect.right-rect.left;
			CClientDC dc(this);

			DWORD col = pcm->ARGBToABGR(pcm->GetTextBkColor(COLORMT_LIST, COLORMS_FRAME))&0xffffff;

			dc.Draw3dRect(rect.left, rect.top, width, height, col, col);
		}
		break;

	default:// it wasn't a notification that was interesting to us.
		{
			*pResult = CDRF_DODEFAULT;
		}
		break;
	}

//	*pResult = 0;
}


void UILayerListCtrl::OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	*pResult = 0;
}


void UILayerListCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	ModifyStyle( WS_HSCROLL, 0 );
	CListCtrl::OnNcCalcSize(bCalcValidRects, lpncsp);
}

void UILayerListCtrl::AddSelect( int index )
{
	if (index < 0)
	{
		int startindex = -1;
		while (true)
		{
			startindex = GetNextItem(startindex, LVNI_SELECTED);
			if (startindex < 0)
			{
				break;
			}
//			AddSelect(startindex);
			SetItemState(startindex, LVIS_SELECTED, LVIS_SELECTED);
			SetSelectionMark(startindex);
		}
		return;
	}

	SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
	SetSelectionMark(index);

	DeSelectParadox(index);
}

void UILayerListCtrl::DeSelect( int index/*=-1*/ )
{
	if (index < 0)
	{
		int startindex = -1;
		while (true)
		{
			startindex = GetNextItem(startindex, LVNI_SELECTED);
			if (startindex < 0)
			{
				break;
			}
			DeSelect(startindex);
		}
	}
	else
	{
		SetItemState(index, 0, LVIS_SELECTED);
	}
}

int UILayerListCtrl::FindItemByData( GObject * obj )
{
	LVFINDINFO lvFindInfo;
	lvFindInfo.lParam = (DWORD_PTR)obj;
	lvFindInfo.flags = LVFI_PARAM;
	return FindItem(&lvFindInfo);
}

int UILayerListCtrl::GetItemIndent( int index )
{
	LVITEM lvItem;
	lvItem.iItem = index;
	lvItem.iSubItem = IDLBC_TREE;
	lvItem.mask = LVIF_INDENT;
	if (GetItem(&lvItem))
	{
		return lvItem.iIndent;
	}
	return -1;
}

void UILayerListCtrl::DeSelectParadox( int index )
{
	int selindent = GetItemIndent(index);
	int selectedindex = GetNextItem(-1, LVNI_SELECTED);
	if (selectedindex == index)
	{
		selectedindex = GetNextItem(selectedindex, LVNI_SELECTED);
	}
	if (selectedindex >= 0)
	{
		if (GetItemIndent(selectedindex) != selindent)
		{
			DeSelect();
			AddSelect(index);
		}
	}

}


void UILayerListCtrl::OnLvnItemchanging(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	DWORD dwPos = GetMessagePos();
	POINTS pts = MAKEPOINTS(dwPos);
	CPoint pt(pts.x, pts.y);

	ScreenToClient(&pt);

	UINT uiFlags = 0;
	int iHit = HitTest(pt, &uiFlags);

	if(uiFlags & LVHT_NOWHERE)
	{
		*pResult = 1;	
	}
}

#include "Main.h"
#include <cstring>
void UILayerListCtrl::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	if (pNMLV->uChanged & LVIF_STATE)
	{
		CString str;
		if (pNMLV->uNewState & LVNI_SELECTED)
		{
			str.Format("Selected: %d, %d", pNMLV->iItem, pNMLV->iSubItem);
			MainInterface::getInstance().CallAppendCommandLogText(str.GetBuffer());
		}
		else if (pNMLV->uOldState & LVNI_SELECTED)
		{
			str.Format("DeSelected: %d, %d", pNMLV->iItem, pNMLV->iSubItem);
			MainInterface::getInstance().CallAppendCommandLogText(str.GetBuffer());
		}
		// do stuff...
	}
	*pResult = 0;
}

GLayer * UILayerListCtrl::GetActiveLayer()
{
	int index = GetNextItem(-1, LVNI_SELECTED);
	if (index < 0)
	{
		return NULL;
//		index = 0;
	}
	GObject * pobj = (GObject *)GetItemData(index);
	ASSERT(pobj);
	GLayer * pLayer = (GLayer *)pobj->GetLayer();
	ASSERT(pLayer);
	return pLayer;
}

void UILayerListCtrl::SetActiveLayer( GLayer * pLayer )
{
	int index = FindItemByData(pLayer);
	if (index < 0)
	{
		index = 0;
	}
//	ASSERT(index>=0);
	DeSelect();
	AddSelect(index);
}


void UILayerListCtrl::SetItemVisible( int index, bool bVisible, bool bRecVisible )
{
	int nImg=IconManager::getInstance().GetLBVisibleIcon(bVisible, bRecVisible);
	SetItem(index, IDLBC_VISIBLE, LVIF_IMAGE, NULL, nImg, 0, 0, 0L);
}

void UILayerListCtrl::SetItemLock( int index, bool bLock, bool bRecLock )
{
	int nImg=IconManager::getInstance().GetLBLockedIcon(bLock, bRecLock);
	SetItem(index, IDLBC_LOCK, LVIF_IMAGE, NULL, nImg, 0, 0, 0L);
}

void UILayerListCtrl::SetItemLineColor( int index, DWORD col )
{
}

void UILayerListCtrl::SetItemTreeInfo( int index, const char * name, int indent, bool bFolded )
{
	int nImg=IconManager::getInstance().GetLBFoldExpandIcon(bFolded);
	SetItem(index, IDLBC_TREE, LVIF_TEXT|LVIF_IMAGE|LVIF_INDENT, name, nImg, 0, 0, 0L, indent);
}

void UILayerListCtrl::SetItemLineSelect( int index, bool toSelect )
{
	int nImg=IconManager::getInstance().GetLBLineSelectIcon(toSelect);
	SetItem(index, IDLBC_LINESELECT, LVIF_IMAGE, NULL, nImg, 0, 0, 0L);
}

void UILayerListCtrl::SetItemFrameColor( int index, DWORD col )
{

}

void UILayerListCtrl::FoldItem( int index )
{

}

void UILayerListCtrl::ExpandItem( int index )
{

}