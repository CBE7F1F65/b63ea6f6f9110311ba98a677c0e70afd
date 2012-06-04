#pragma once


// UILayerListCtrl
#include "GLayer.h"

class UILayerListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(UILayerListCtrl)

public:
	UILayerListCtrl();
	virtual ~UILayerListCtrl();

	void RebuildTree( GObject * changebase, GObject * activeitem );
	void BuildChildren(GObject * nowbase, int & nowindex, int indentlevel);
	void DeleteItemsUnderObj(GObject * obj);

	void AddSelect(int index=-1);
	void DeSelect(int index=-1);
	void DeSelectParadox(int index);

	GLayer * GetActiveLayer();

	int FindItemByData(GObject * obj);
	int GetItemIndent(int index);

	void SetItemVisible(int index, bool bVisible, bool bRecVisible);
	void SetItemLock(int index, bool bLock, bool bRecLock);
	void SetItemLineColor(int index, DWORD col);
	void SetItemTreeInfo(int index, const char * name, int indent, bool bFolded);
	void SetItemLineSelect(int index, bool toSelect);
	void SetItemFrameColor(int index, DWORD col);

	void FoldItem(int index);
	void ExpandItem(int index);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg void OnLvnItemchanging(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
};


