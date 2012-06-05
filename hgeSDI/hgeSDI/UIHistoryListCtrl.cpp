// UIHistoryListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "hgeSDI.h"
#include "UIHistoryListCtrl.h"
#include "Command.h"
#include "Main.h"
#include "ColorManager.h"
#include "IconManager.h"

// UIHistoryListCtrl

enum{
	IDHBC_MAIN=0,
	IDHBC_COMMANDSTR,
};

IMPLEMENT_DYNAMIC(UIHistoryListCtrl, CListCtrl)

UIHistoryListCtrl::UIHistoryListCtrl()
{
	nSelected = -1;
	topIndex = 0;
}

UIHistoryListCtrl::~UIHistoryListCtrl()
{
}

BEGIN_MESSAGE_MAP(UIHistoryListCtrl, CListCtrl)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &UIHistoryListCtrl::OnNMCustomdraw)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &UIHistoryListCtrl::OnLvnItemchanged)
	ON_NOTIFY_REFLECT(NM_CLICK, &UIHistoryListCtrl::OnNMClick)
	ON_WM_SIZE()
//	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, &UIHistoryListCtrl::OnLvnGetdispinfo)
ON_WM_NCCALCSIZE()
END_MESSAGE_MAP()

// UIHistoryListCtrl 消息处理程序

int UIHistoryListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	ColorManager * pcm = &ColorManager::getInstance();
	SetBkColor(pcm->ARGBToABGR(pcm->GetTextBkColor(COLORMT_LIST, COLORMS_BACKGROUND)) & 0xffffff);
	/*
	LOGFONT lf;
	GetFont()->GetLogFont(&lf);
	lf.lfHeight *= 1.25f;
	m_cfont.CreateFontIndirect(&lf);
	SetFont(&m_cfont);
	*/
	SetExtendedStyle(LVS_EX_FULLROWSELECT);

	InsertColumn(IDHBC_MAIN, "", LVCFMT_LEFT);
	InsertColumn(IDHBC_COMMANDSTR, "", LVCFMT_LEFT);

	IconManager * picm = &IconManager::getInstance();
	SetImageList(picm->GetImageList(ICMSIZE_SMALL), picm->GetImageListType(ICMSIZE_SMALL));

	return 0;
}

void UIHistoryListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
//	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

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
			//if (sub)item was the one clicked, set custom text/background color
			//Un-comment the code segment to allow single cell highlight
			if( (nSelected == nowDrawing ) )
			{
				pCD->clrText	= pcm->ARGBToABGR(pcm->GetTextColor(COLORMT_LIST, COLORMS_ACTIVE))&0xffffff;
				pCD->clrTextBk	= pcm->ARGBToABGR(pcm->GetTextBkColor(COLORMT_LIST, COLORMS_ACTIVE))&0xffffff;
			}
			else if (nSelected > nowDrawing)
			{
				pCD->clrText	= pcm->ARGBToABGR(pcm->GetTextColor(COLORMT_LIST, COLORMS_NONACTIVE))&0xffffff;
				pCD->clrTextBk	= pcm->ARGBToABGR(pcm->GetTextBkColor(COLORMT_LIST, COLORMS_NONACTIVE))&0xffffff;
			}
			else
			{
				pCD->clrText	= pcm->ARGBToABGR(pcm->GetTextColor(COLORMT_LIST, COLORMS_DISABLED))&0xffffff;
				pCD->clrTextBk	= pcm->ARGBToABGR(pcm->GetTextBkColor(COLORMT_LIST, COLORMS_DISABLED))&0xffffff;
			}
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

//			HPEN hpen = CreatePen(PS_SOLID, 2, col);
//			SelectObject(dc.m_hDC, hpen);
//			dc.DrawEdge(&rect, BDR_RAISEDINNER, BF_RECT);
			dc.Draw3dRect(rect.left, rect.top, width, height, col, col);
			/*
			dc.MoveTo(rect.left, rect.top);
			dc.LineTo(rect.right, rect.top);
			dc.MoveTo(rect.left, rect.bottom);
			dc.LineTo(rect.right, rect.bottom);
			*/
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

void UIHistoryListCtrl::OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	EnsureVisible(nSelected, FALSE);
	if (nSelected == topIndex-1)
	{
		PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}
	*pResult = 0;
}

bool UIHistoryListCtrl::AddHistory( const char * desc, const char * commandstr, int command )
{
	/*
	InsertItem(LVIF_TEXT | LVIF_STATE | LVIF_IMAGE, topIndex, desc,
		0, LVIS_SELECTED, I_IMAGECALLBACK, 0);
		*/
	InsertItem(topIndex, desc, IconManager::getInstance().GetCommandIconIndex(command));
	SetItemText(topIndex, IDHBC_COMMANDSTR, commandstr);
	SetItemState(topIndex, LVIS_SELECTED, LVIS_SELECTED);
	SetSelectionMark(topIndex);
	nSelected++;
	topIndex++;

	SetColumnWidth(IDHBC_MAIN, LVSCW_AUTOSIZE);
	RECT rect;
	GetWindowRect(&rect);
	SetColumnWidth(IDHBC_COMMANDSTR, rect.right-rect.left-GetColumnWidth(IDHBC_MAIN));

	return true;
}

bool UIHistoryListCtrl::ChangeCurrentHistory( int step )
{
	int oldSelected = nSelected;
	nSelected += step;
	SetItemState(oldSelected, ~LVIS_SELECTED, LVIS_SELECTED);
	SetItemState(nSelected, LVIS_SELECTED, LVIS_SELECTED);
	SetSelectionMark(nSelected);
	return true;
}

bool UIHistoryListCtrl::ClearLaterHistory( int ndelete )
{
	for (int i=0; i<ndelete; i++)
	{
		DeleteItem(topIndex-i-1);
	}
	topIndex -= ndelete;
	SetItemState(nSelected, LVIS_SELECTED, LVIS_SELECTED);
	SetSelectionMark(nSelected);

	return true;
}

bool UIHistoryListCtrl::ClearPreviousHistory( int ndelete )
{
	for (int i=0; i<ndelete; i++)
	{
		DeleteItem(0);
		if (nSelected)
		{
			nSelected--;
		}
	}
	topIndex -= ndelete;
	SetItemState(nSelected, LVIS_SELECTED, LVIS_SELECTED);
	SetSelectionMark(nSelected);

	return true;
}

void UIHistoryListCtrl::OnNMClick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	int newSelected = pNMItemActivate->iItem;

	if (newSelected >= 0)
	{
		if (nSelected != newSelected)
		{
			MainInterface::getInstance().CallUnDoReDo(newSelected-nSelected);
			nSelected = newSelected;
		}
	}
	else
	{
		SetItemState(nSelected, LVIS_SELECTED, LVIS_SELECTED);
		SetSelectionMark(nSelected);
	}

	Invalidate();
	*pResult = 0;
}

BOOL UIHistoryListCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && (GetKeyState(VK_CONTROL)&0x8000))
	{
		if (pMsg->wParam == 'C' || pMsg->wParam == 'X')
		{
			CString str = GetItemText(nSelected, IDHBC_COMMANDSTR);
			((CMainFrame *)AfxGetMainWnd())->SetCommandText(str.GetBuffer());
			if ( ::OpenClipboard(NULL) )
			{
				int nlength = str.GetLength()+1;
				HANDLE hMem = ::GlobalAlloc(GMEM_MOVEABLE|GMEM_DDESHARE ,
					nlength);
				if (hMem)
				{
					LPSTR lpStr = (LPSTR)::GlobalLock(hMem);
					strcpy_s(lpStr, nlength, str.GetBuffer());
					::GlobalUnlock(hMem);

					::SetClipboardData( CF_TEXT, hMem );
				}
				::CloseClipboard();
			}
		}
	}

	return CListCtrl::PreTranslateMessage(pMsg);
}

void UIHistoryListCtrl::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);


	SetColumnWidth(IDHBC_MAIN, LVSCW_AUTOSIZE);
	SetColumnWidth(IDHBC_COMMANDSTR, cx-GetColumnWidth(IDHBC_MAIN));
	EnsureVisible(nSelected, FALSE);
	// TODO: 在此处添加消息处理程序代码
}

/*
void UIHistoryListCtrl::OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
//	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	LV_DISPINFO* pdi = (LV_DISPINFO *) pNMHDR;
	if (LVIS_SELECTED == GetItemState (pdi->item.iItem,
		LVIS_SELECTED))
		pdi->item.iImage = 0;
	else
		pdi->item.iImage = pdi->item.iItem;


	*pResult = 0;
}
*/

void UIHistoryListCtrl::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	ModifyStyle( WS_HSCROLL, 0 );
	CListCtrl::OnNcCalcSize(bCalcValidRects, lpncsp);
}
