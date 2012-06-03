#pragma once


// UILayerListCtrl
#include "GObject.h"

class UILayerListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(UILayerListCtrl)

public:
	UILayerListCtrl();
	virtual ~UILayerListCtrl();

	void RebuildTree( GObject * changebase );
	void BuildChildren(GObject * nowbase, int & nowindex, GObject * changebase);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
};


