// UILayerTree.cpp : 实现文件
//

#include "stdafx.h"
#include "hgeSDI.h"
#include "UILayerTree.h"

// UILayerTree

IMPLEMENT_DYNAMIC(UILayerTree, CTreeCtrl)

UILayerTree::UILayerTree()
{
	htNowEdit = TVI_ROOT;
}

UILayerTree::~UILayerTree()
{
}


BEGIN_MESSAGE_MAP(UILayerTree, CTreeCtrl)
	ON_WM_CREATE()
END_MESSAGE_MAP()



// UILayerTree 消息处理程序




int UILayerTree::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码

	return 0;
}

void UILayerTree::RebuildTree( GObject * changebase )
{
	DeleteAllItems();

	htNowEdit = TVI_ROOT;
	GObjectManager * pgm = &GObjectManager::getInstance();
	HTREEITEM htroot = InsertItem(pgm->basenode.GetTypeName(), TVI_ROOT);

	BuildChildren(&(pgm->basenode), htroot, changebase);
//	Expand(htroot, TVE_EXPAND);

	EnsureVisible(htNowEdit);

}

void UILayerTree::BuildChildren( GObject * nowbase, HTREEITEM nowht, GObject * changebase )
{
	if (nowbase)
	{
		if (!(nowbase->children.empty()))
		{
			for (list<GObject *>::iterator it=nowbase->children.begin(); it!=nowbase->children.end(); ++it)
			{
				HTREEITEM leafht = InsertItem((*it)->GetTypeName(), nowht);
				if (changebase == (*it))
				{
					htNowEdit = leafht;
				}
				BuildChildren(*it, leafht, changebase);
//				Expand(leafht, TVE_EXPAND);
			}
		}
	}
}