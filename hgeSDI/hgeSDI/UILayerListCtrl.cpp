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
	IDLBC_SELECT,
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
END_MESSAGE_MAP()

void UILayerListCtrl::RebuildTree( GObject * changebase )
{
	SetRedraw(FALSE);
	DeleteAllItems();

	GObjectManager * pgm = &GObjectManager::getInstance();

	int nowindex=0;
	InsertItem(0, "");
	SetItem(nowindex, IDLBC_VISIBLE, LVIF_IMAGE, "", 0, 0, 0, 0);
	SetItem(nowindex, IDLBC_LOCK, LVIF_IMAGE, "", 0, 0, 0, 0);
	SetItem(nowindex, IDLBC_LINECOLOR, LVIF_IMAGE, "", 32, 0, 0, 0);
	SetItem(nowindex, IDLBC_TREE, LVIF_TEXT|LVIF_IMAGE, "L", 0, 0, 0, 0);
	SetItem(nowindex, IDLBC_SELECT, LVIF_IMAGE, "", 32, 0, 0, 0);
	SetItem(nowindex, IDLBC_FRAMECOLOR, LVIF_IMAGE, "", 16, 0, 0, 0);
	nowindex++;
	BuildChildren(&(pgm->basenode), nowindex, changebase);

	SetRedraw();
}

void UILayerListCtrl::BuildChildren( GObject * nowbase, int & nowindex, GObject * changebase )
{
	if (nowbase)
	{
		if (!(nowbase->children.empty()))
		{
			for (list<GObject *>::iterator it=nowbase->children.begin(); it!=nowbase->children.end(); ++it)
			{
				InsertItem(nowindex, "");
				SetItem(nowindex, IDLBC_VISIBLE, LVIF_IMAGE, "", 0, 0, 0, 0);
				SetItem(nowindex, IDLBC_LOCK, LVIF_IMAGE, "", 16, 0, 0, 0);
				SetItem(nowindex, IDLBC_LINECOLOR, LVIF_IMAGE, "", 32, 0, 0, 0);
				SetItem(nowindex, IDLBC_TREE, LVIF_TEXT|LVIF_IMAGE|LVIF_INDENT, "L", 63, 0, 0, 0, nowindex);
				SetItem(nowindex, IDLBC_SELECT, LVIF_IMAGE, "", 32, 0, 0, 0);
				SetItem(nowindex, IDLBC_FRAMECOLOR, LVIF_IMAGE, "", 16, 0, 0, 0);
				nowindex++;
				BuildChildren(*it, nowindex, changebase);
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
	SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_SUBITEMIMAGES);

	InsertColumn(IDLBC_VISIBLE, "", LVCFMT_LEFT);
	InsertColumn(IDLBC_LOCK, "", LVCFMT_LEFT);
	InsertColumn(IDLBC_LINECOLOR, "", LVCFMT_LEFT);
	InsertColumn(IDLBC_TREE, "", LVCFMT_LEFT);
	InsertColumn(IDLBC_SELECT, "", LVCFMT_LEFT);
	InsertColumn(IDLBC_FRAMECOLOR, "", LVCFMT_LEFT);

	LPINT pnOrder = (LPINT) malloc(_IDLBC_INDEXMAX*sizeof(int));
	pnOrder[0] = IDLBC_VISIBLE;
	pnOrder[1] = IDLBC_LOCK;
	pnOrder[2] = IDLBC_LINECOLOR;
	pnOrder[3] = IDLBC_TREE;
	pnOrder[4] = IDLBC_SELECT;
	pnOrder[5] = IDLBC_FRAMECOLOR;

	SetColumnOrderArray(_IDLBC_INDEXMAX, pnOrder);
	free(pnOrder);

	SetColumnWidth(IDLBC_VISIBLE, _LAYERLIST_ICONSIZE);
	SetColumnWidth(IDLBC_LOCK, _LAYERLIST_ICONSIZE);
	SetColumnWidth(IDLBC_LINECOLOR, _LAYERLIST_LINECOLORSIZE);
	SetColumnWidth(IDLBC_SELECT, _LAYERLIST_ICONSIZE);
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
	otherwidth += GetColumnWidth(IDLBC_SELECT);
	otherwidth += GetColumnWidth(IDLBC_FRAMECOLOR);
	SetColumnWidth(IDLBC_TREE, cx-otherwidth);
//	EnsureVisible(nSelected, FALSE);
	// TODO: 在此处添加消息处理程序代码
}


void UILayerListCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void UILayerListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW *pCD = (NMLVCUSTOMDRAW*)pNMHDR;
	// By default set the return value to do the default behavior.
	*pResult = CDRF_DODEFAULT;

	//obtain row and column of item
	int nowDrawing = pCD->nmcd.dwItemSpec;
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
			pCD->clrText	= pcm->ARGBToABGR(pcm->GetTextColor(COLORMT_LIST, COLORMS_DISABLED))&0xffffff;
			pCD->clrTextBk	= pcm->ARGBToABGR(pcm->GetTextBkColor(COLORMT_LIST, COLORMS_DISABLED))&0xffffff;
			*pResult =  CDRF_NOTIFYPOSTPAINT;

			//
		}
		break;

	case CDDS_ITEMPOSTPAINT | CDDS_SUBITEM: // Stage four (called for each subitem of the focused item)
		{
			RECT rect;
			GetItemRect(nowDrawing, &rect, LVIR_BOUNDS);
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
