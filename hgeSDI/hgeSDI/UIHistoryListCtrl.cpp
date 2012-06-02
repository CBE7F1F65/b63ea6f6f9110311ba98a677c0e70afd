// UIHistoryListCtrl.cpp : 实现文件
//

#include "stdafx.h"
#include "hgeSDI.h"
#include "UIHistoryListCtrl.h"
#include "Command.h"
#include "Main.h"
#include "ColorManager.h"


// UIHistoryListCtrl

#define IDHBC_MAIN	0
#define IDHBC_COMMANDSTR	1

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
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, &UIHistoryListCtrl::OnLvnDeleteitem)
	ON_NOTIFY_REFLECT(LVN_INSERTITEM, &UIHistoryListCtrl::OnLvnInsertitem)
	ON_NOTIFY(HDN_ITEMCLICKA, 0, &UIHistoryListCtrl::OnHdnItemclick)
	ON_NOTIFY(HDN_ITEMCLICKW, 0, &UIHistoryListCtrl::OnHdnItemclick)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &UIHistoryListCtrl::OnNMCustomdraw)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, &UIHistoryListCtrl::OnLvnItemchanged)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, &UIHistoryListCtrl::OnLvnColumnclick)
	ON_NOTIFY_REFLECT(NM_CLICK, &UIHistoryListCtrl::OnNMClick)
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, &UIHistoryListCtrl::OnLvnKeydown)
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
END_MESSAGE_MAP()



// UIHistoryListCtrl 消息处理程序




int UIHistoryListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	ColorManager * pcm = &ColorManager::getInstance();
	// TODO:  在此添加您专用的创建代码
	SetBkColor(pcm->ARGBToABGR(pcm->GetTextBkColor(COLORMT_LIST, COLORMS_NONACTIVE)) & 0xffffff);
//	SetTextBkColor(0x909090);

	LOGFONT lf;
	GetFont()->GetLogFont(&lf);
	lf.lfHeight *= 1.5f;
	m_cfont.CreateFontIndirect(&lf);
	SetFont(&m_cfont);

	SetExtendedStyle(LVS_EX_FULLROWSELECT);

	InsertColumn(IDHBC_MAIN, "", LVCFMT_LEFT);
	InsertColumn(IDHBC_COMMANDSTR, "", LVCFMT_LEFT);


	return 0;
}


void UIHistoryListCtrl::OnLvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void UIHistoryListCtrl::OnLvnInsertitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void UIHistoryListCtrl::OnHdnItemclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void UIHistoryListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
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

		}
		break;

	case CDDS_ITEMPOSTPAINT | CDDS_SUBITEM: // Stage four (called for each subitem of the focused item)
		{

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

bool UIHistoryListCtrl::AddHistory( const char * desc, const char * commandstr )
{
	InsertItem(LVIF_TEXT | LVIF_STATE, topIndex, desc,
		0, LVIS_SELECTED, 0, 0);
	SetItemText(topIndex, IDHBC_COMMANDSTR, commandstr);
	SetItemState(topIndex, LVIS_SELECTED, LVIS_SELECTED);
	SetSelectionMark(topIndex);
	nSelected++;
	topIndex++;
	SetColumnWidth(IDHBC_MAIN, LVSCW_AUTOSIZE);
	SetColumnWidth(IDHBC_COMMANDSTR, LVSCW_AUTOSIZE);
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

void UIHistoryListCtrl::OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
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


void UIHistoryListCtrl::OnLvnKeydown(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	*pResult = 0;
}


void UIHistoryListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

//	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}


void UIHistoryListCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

//	CListCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}


BOOL UIHistoryListCtrl::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && ::GetKeyState(VK_CONTROL))
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
