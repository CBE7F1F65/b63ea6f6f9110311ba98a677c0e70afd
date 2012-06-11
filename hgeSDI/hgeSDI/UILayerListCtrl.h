#pragma once


// UILayerListCtrl
#include "GLayer.h"

enum{
	LBCOMMAND_REPARENT_NONE = 0,
	LBCOMMAND_REPARENT_UPLAYER,
	LBCOMMAND_REPARENT_DOWNLAYER,
	LBCOMMAND_REPARENT_UP,
	LBCOMMAND_REPARENT_DOWN,
};

class UILayerListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(UILayerListCtrl)
public:
	UILayerListCtrl();
	virtual ~UILayerListCtrl();

	void ResetAllName(GObject * base);
	void RebuildTree( GObject * changebase, GObject * activeitem );
	void BuildChildren(GObject * nowbase, int & nowindex, int indentlevel);
	void DeleteItemsUnderObj(GObject * obj);

	void AddSelect(int index=-1);
	void DeSelect(int index=-1);
	void DeSelectParadox(int index);

	GObject * GetObjectByIndex(int index);

	GLayer * GetActiveLayer();
	GObject * GetActiveNodes( int * pnextfromIndex );
	bool GetDragDropNodes( GLayer ** pLayerNode, GObject ** pAfterNode );
	void SetActiveLayer_Internal( GLayer * pLayer );

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

	void ReparentSelectedNodes( int op );

	GLayer * FindPreviousLayer( int index, int * outindex=NULL );
	GLayer * FindNextLayer( int index, int * outindex=NULL );
	GObject * FindPreviousNode( int index, int * outindex=NULL );
	GObject * FindNextNode( int index, int * outindex=NULL );

	bool bDragging;
	GLayer * pDragDropLayer;
	GObject * pDragDropAfter;
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
	afx_msg void OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLvnGetInfoTip(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};


