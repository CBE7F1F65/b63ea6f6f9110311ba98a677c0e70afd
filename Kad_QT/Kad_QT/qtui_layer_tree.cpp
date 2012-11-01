#include "stdafx.h"
#include "qtui_layer_tree.h"
#include "qmaininterface.h"
#include "IconManager.h"
#include "MainInterface.h"
#include "GObjectManager.h"

#include "QTDLG_LayerProperty.h"

#include "QTUI_Layer_Buttons.h"
#include "MarqueeSelect.h"

#define _UILT_ICONSIZE  ICMSIZE_MIDDLE
#define _UILT_LINECOLORSIZE 6
#define _UILT_INDENTATION   16

enum{
    _UILT_COLUMN_TREE,
    _UILT_COLUMN_VISIBLE,
    _UILT_COLUMN_LOCK,
    _UILT_COLUMN_LINECOLOR,
    _UILT_COLUMN_SELECT,
    _UILT_COLUMN_FRAMECOLOR,

    _UILT_COLUMNCOUNT,
};

QTUI_Layer_Tree::QTUI_Layer_Tree(QWidget *parent) :
    QTreeWidget(parent)
{
    QMainInterface::getInstance().SetPLayerTree(this);
    this->setColumnCount(_UILT_COLUMNCOUNT);    // Not working
    this->header()->moveSection(_UILT_COLUMN_TREE, _UILT_COLUMN_LINECOLOR);
    this->setIndentation(_UILT_INDENTATION);

    pDragDropLayer = NULL;
    pDragDropAfter = NULL;

    bInternalSelecting = false;

//    this->setItemDelegate(new UILT_EditDelegate(this));

    AdjustSize();
}

void QTUI_Layer_Tree::RebuildTree(GObject *changebase, GObject *activeitem)
{
    selectednodes.clear();
    QTreeWidgetItem * pBaseItem = FindItemByObj(changebase);
    if (!pBaseItem)
    {
        pBaseItem = this->invisibleRootItem();
    }
    DeleteItemsUnderWidget(pBaseItem);

    BuildChildren(pBaseItem, changebase);

    if (activeitem)
    {
        QTreeWidgetItem *pActive = FindItemByObj(activeitem, pBaseItem);
        if (pActive)
        {
            pPreferredNextSelectItem = activeitem;
            this->setCurrentItem(pActive);
            this->scrollToItem(pActive);
            Reselect();
        }
    }

    /*
    if (activeitem)
    {
        QTreeWidgetItem *pTreeItem[20];

        pTreeItem[0] = new QTreeWidgetItem(invisibleRootItem());
        pTreeItem[0]->setText(_UILT_COLUMN_TREE, activeitem->getDisplayName());
//        QVariant v((uint)changebase);
        pTreeItem[0]->setData(_UILT_COLUMN_TREE, Qt::UserRole, QVariant((uint)changebase));

        for (int i=1; i<20; i++)
        {
            pTreeItem[i] = new QTreeWidgetItem(pTreeItem[i-1]);
            pTreeItem[i]->setText(_UILT_COLUMN_TREE, activeitem->getDisplayName());
        }
        FindItemByObj(changebase);

        AdjustSize();
    }
    */
    AdjustSize();
}

list<GObject *> * QTUI_Layer_Tree::GetActiveNodes()
{
    if (selectednodes.empty() && this->topLevelItem(0))
    {
        GObject * pObj = NULL;
        if (pPreferredNextSelectItem && FindItemByObj(pPreferredNextSelectItem))
        {
            pObj = pPreferredNextSelectItem;
        }
        else
        {
            pObj = GetObjFromItem(this->topLevelItem(0));
            pPreferredNextSelectItem = pObj;
        }
        Q_ASSERT(pObj);
        selectednodes.push_back(pObj);
        Reselect();
    }
    return &selectednodes;
}

GObject *QTUI_Layer_Tree::GetHoveringNode()
{
	QWidget * pParentWidget = this->parentWidget();
	QPoint pt = pParentWidget->mapFromGlobal(QCursor::pos());
	QTreeWidgetItem * pItem = this->itemAt(pt);
	if (pItem)
	{
		return GetObjFromItem(pItem);
	}
    return NULL;
}

bool QTUI_Layer_Tree::GetDragDroppedNodes(GLayer **pLayerNode, GObject **pAfterNode)
{
    if (pLayerNode && pAfterNode)
    {
        *pLayerNode = pDragDropLayer;
        *pAfterNode = pDragDropAfter;
        return true;
    }
    return false;
}

void QTUI_Layer_Tree::dropEvent(QDropEvent *e)
{
    QWidget * pWidget = e->source();
    if (pWidget == this)
    {
//        DropIndicatorPosition dip = this->dropIndicatorPosition();
        if (true)//(dip == QAbstractItemView::OnItem)
        {
            QTreeWidgetItem * pItem = this->itemAt(e->pos());
            GObject * pObj = GetObjFromItem(pItem);
            if (IsObjInSelection(pObj))
            {
                e->ignore();
                return;
            }
            if (pObj->isLayer())
            {
                pDragDropLayer = (GLayer *)pObj;
                pDragDropAfter = NULL;
            }
            else
            {
                pDragDropLayer = (GLayer *)pObj->getLayer();
                pDragDropAfter = pObj;
            }
            MainInterface::getInstance().OnCommand(COMM_REPARENT);
            e->acceptProposedAction();
			Reselect();
            return;
        }
    }
    e->ignore();
}

void QTUI_Layer_Tree::dragMoveEvent(QDragMoveEvent *e)
{
    QTreeWidget::dragMoveEvent(e);
    QTreeWidgetItem * pItem = this->itemAt(e->pos());
    if (pItem && this->dropIndicatorPosition()==QAbstractItemView::OnItem)
    {
        e->acceptProposedAction();
    }
    else
    {
        e->ignore();
    }
}

void QTUI_Layer_Tree::resizeEvent(QResizeEvent *e)
{
    AdjustSize();
}

void QTUI_Layer_Tree::AdjustSize()
{
    int wtotal = this->width();
    this->setColumnWidth(_UILT_COLUMN_VISIBLE, _UILT_ICONSIZE);
    this->setColumnWidth(_UILT_COLUMN_LOCK, _UILT_ICONSIZE);
    this->setColumnWidth(_UILT_COLUMN_LINECOLOR, _UILT_LINECOLORSIZE);
    this->setColumnWidth(_UILT_COLUMN_SELECT, _UILT_ICONSIZE);
    this->setColumnWidth(_UILT_COLUMN_FRAMECOLOR, _UILT_ICONSIZE);

    this->resizeColumnToContents(_UILT_COLUMN_TREE);
    int wauto = this->columnWidth(_UILT_COLUMN_TREE);

    int wlast = wtotal - _UILT_ICONSIZE*4 - _UILT_LINECOLORSIZE - (_UILT_COLUMNCOUNT-1);
    if (wlast > wauto)
    {
        this->setColumnWidth(_UILT_COLUMN_TREE, wlast);
    }
}

QTreeWidgetItem *QTUI_Layer_Tree::FindItemByObj(GObject *pObj, QTreeWidgetItem *pParent)
{
    if (pParent == NULL)
    {
        pParent = this->invisibleRootItem();
    }
    int nCount = pParent->childCount();
    if (!nCount)
    {
        return NULL;
    }
    for (int i=0; i<nCount; i++)
    {
        QTreeWidgetItem *pItem = pParent->child(i);
        if (GetObjFromItem(pItem) == pObj)
        {
            return pItem;
        }
        else
        {
            pItem = FindItemByObj(pObj, pItem);
            if (pItem)
            {
                return pItem;
            }
        }
    }
    return NULL;

    /*
    QList<QTreeWidgetItem*> lst = this->findItems(QString(""), Qt::MatchRecursive|Qt::MatchStartsWith, _UILT_COLUMN_TREE);
    QList<QTreeWidgetItem*>::iterator it;
    for (it=lst.begin(); it!=lst.end(); ++it)
    {
        if ((*it)->data(_UILT_COLUMN_TREE, Qt::UserRole) == (uint)pObj)
        {
            return *it;
        }
    }
    */
}

GObject *QTUI_Layer_Tree::GetObjFromItem(QTreeWidgetItem *pItem)
{
    if (pItem)
    {
        bool bOk = false;
        GObject * pObj = (GObject *)(pItem->data(_UILT_COLUMN_TREE, Qt::UserRole).toUInt(&bOk));
        Q_ASSERT(bOk);
        return pObj;
    }
    Q_ASSERT(false);
    return NULL;
}

void QTUI_Layer_Tree::DeleteItemsUnderWidget(QTreeWidgetItem *pItem)
{
    if (!pItem)
    {
        return;
    }

    int nCount = pItem->childCount();
    for (int i=0; i<nCount; i++)
    {
        delete pItem->child(0);
    }
}

void QTUI_Layer_Tree::BuildChildren(QTreeWidgetItem *pItem, GObject *pObjParent)
{
    if (!pObjParent || !pObjParent->getNonAttributeChildrenCount())
    {
        return;
    }
    for (list<GObject *>::iterator it=pObjParent->getChildren()->begin(); it!=pObjParent->getChildren()->end(); ++it)
    {
        GObject * pObj = *it;
        bool bFolded = pObj->isDisplayFolded();

        QTreeWidgetItem * pNowItem = new QTreeWidgetItem(pItem);
//        pNowItem->setFlags(Qt::ItemIsEditable|pNowItem->flags());
		QPushButton * pSelectionButton = new QTUI_Layer_SelectionButton(pObj);
		this->setItemWidget(pNowItem, _UILT_COLUMN_SELECT, pSelectionButton);
		QPushButton * pVisibleButton = new QTUI_Layer_VisibleButton(pObj);
		this->setItemWidget(pNowItem, _UILT_COLUMN_VISIBLE, pVisibleButton);
		QPushButton * pLockButton = new QTUI_Layer_LockButton(pObj);
		this->setItemWidget(pNowItem, _UILT_COLUMN_LOCK, pLockButton);

        UpdateNodeInfo(pNowItem, pObj);

        BuildChildren(pNowItem, pObj);
    }
    pItem->setExpanded(!pObjParent->isDisplayFolded());
}

bool QTUI_Layer_Tree::IsObjInSelection(GObject *pObj)
{
    if (selectednodes.empty())
    {
        return false;
    }
    for (list<GObject *>::iterator it=selectednodes.begin(); it!=selectednodes.end(); ++it)
    {
        if (pObj == *it)
        {
            return true;
        }
    }
    return false;
}

void QTUI_Layer_Tree::Reselect()
{
    bInternalSelecting = true;

    list<GObject *> lst = selectednodes;
    this->selectionModel()->clearSelection();
    for (list<GObject *>::iterator it=lst.begin(); it!= lst.end(); ++it)
    {
        QTreeWidgetItem * pItem = FindItemByObj(*it);
        this->selectionModel()->select(
                    this->indexFromItem(pItem, _UILT_COLUMN_TREE),
                    QItemSelectionModel::Select);
    }

    bInternalSelecting = false;
}

void QTUI_Layer_Tree::UpdateNodeInfo(QTreeWidgetItem *pItem, GObject *pObj)
{
    SetItemData(pItem, pObj);
    SetItemVisible(pItem, pObj->isDisplayVisible(), pObj->isRecDisplayVisible(), pObj);
    SetItemLock(pItem, pObj->isDisplayLocked(), pObj->isRecDisplayLocked(), pObj);
    SetItemLineColor(pItem, pObj->getLineColor(), pObj);
    SetItemTreeInfo(pItem, pObj->getDisplayName(), pObj->isDisplayFolded(), pObj);
    SetItemLineSelect(pItem, pObj->isSelected(), pObj);
    SetItemFrameColor(pItem, pObj->getLayer()->getLineColor(), pObj);
}

void QTUI_Layer_Tree::SetItemData(QTreeWidgetItem *pItem, GObject *pObj)
{
    if (pItem)
    {
        pItem->setData(_UILT_COLUMN_TREE, Qt::UserRole, (uint)pObj);
    }
}

void QTUI_Layer_Tree::SetItemVisible(QTreeWidgetItem *pItem, bool bDisplayVisible, bool bRecDisplayVisable, GObject * pObj)
{
    if (pItem)
	{
		QTUI_Layer_VisibleButton * pVisibleButton = (QTUI_Layer_VisibleButton *)this->itemWidget(pItem, _UILT_COLUMN_VISIBLE);
		pVisibleButton->setChecked(bDisplayVisible);
    }
}

void QTUI_Layer_Tree::SetItemLock(QTreeWidgetItem *pItem, bool bDisplayLocked, bool bRecDisplayLocked, GObject * pObj)
{
    if (pItem)
    {
		QTUI_Layer_LockButton * pLockButton = (QTUI_Layer_LockButton *)this->itemWidget(pItem, _UILT_COLUMN_LOCK);
		pLockButton->setChecked(bDisplayLocked);
    }
}

void QTUI_Layer_Tree::SetItemLineColor(QTreeWidgetItem *pItem, DWORD dwLineColor, GObject * pObj)
{
    if (pItem)
    {
    }
}

void QTUI_Layer_Tree::SetItemTreeInfo(QTreeWidgetItem *pItem, QString strDisplayName, bool bFolded, GObject * pObj)
{
    if (pItem)
    {
        pItem->setText(_UILT_COLUMN_TREE, strDisplayName);
        pItem->setToolTip(_UILT_COLUMN_TREE, tr("ID: %1").arg(pObj->getID()));
    }
}

void QTUI_Layer_Tree::SetItemLineSelect(QTreeWidgetItem *pItem, bool bSelected, GObject * pObj)
{
    if (pItem)
    {
    }
}

void QTUI_Layer_Tree::SetItemFrameColor(QTreeWidgetItem *pItem, DWORD dwFrameColor, GObject * pObj)
{
    if (pItem)
    {
    }
}

void QTUI_Layer_Tree::SLT_ItemCollapsed(QTreeWidgetItem *pItem)
{
    GObject * pObj = GetObjFromItem(pItem);
    pObj->setDisplayFold(true);
    AdjustSize();
}

void QTUI_Layer_Tree::SLT_ItemExpanded(QTreeWidgetItem *pItem)
{
    GObject * pObj = GetObjFromItem(pItem);
    pObj->setDisplayFold(false);
    AdjustSize();
}

void QTUI_Layer_Tree::SLT_NewLayer()
{
    MainInterface::getInstance().OnCommand(COMM_NEWLAYER);
}

void QTUI_Layer_Tree::SLT_NewSubLayer()
{
    MainInterface::getInstance().OnCommand(COMM_NEWSUBLAYER);
}

void QTUI_Layer_Tree::SLT_CopyItems()
{
    MainInterface::getInstance().OnCommand(COMM_DUPLICATE);
}

void QTUI_Layer_Tree::SLT_DeleteItems()
{
    if (selectednodes.empty())
    {
        return;
    }
    pPreferredNextSelectItem = selectednodes.front()->getLayer(false);
    MainInterface::getInstance().OnCommand(COMM_DELETEITEM);
    /*
    selectednodes.clear();

    QTreeWidgetItem * pItem = NULL;
    if (pLayer)
    {
        pItem = FindItemByObj(pLayer);
    }

    if (!pItem)
    {
        pItem = this->topLevelItem(0);
        pLayer = GetObjFromItem(pItem)->GetLayer();
    }

    Q_ASSERT(pLayer);
    selectednodes.push_back(pLayer);
    Reselect();
    */
}

void QTUI_Layer_Tree::SLT_UpdateSelectionNodes()
{
    if (bInternalSelecting)
    {
        return;
    }
	if (GObjectManager::getInstance().IsTreeLocked())
	{
		return;
	}
    QList<QTreeWidgetItem *> lst = this->selectedItems();
    if (!lst.size())
    {
        Reselect();
        return;
    }

    QList<QTreeWidgetItem *>::iterator it;

    if (lst.size() == 1)
    {
        selectednodes.clear();
        for (it=lst.begin(); it!=lst.end(); ++it)
        {
            selectednodes.push_back(GetObjFromItem(*it));
        }
    }
    else
    {
        GObject * pParent;
        GObject * pSelf;
        bool bLayer;
        if (selectednodes.empty())
        {
			return;
			/*
            pSelf = GetObjFromItem(lst.front());
            pParent = pSelf->getParent();
			*/
        }
        else
        {
            pSelf = selectednodes.front();
            pParent = pSelf->getParent();
        }
        Q_ASSERT(pSelf);
        Q_ASSERT(pParent);
        bLayer = pSelf->isLayer();
        selectednodes.clear();
        for (it=lst.begin(); it!=lst.end(); ++it)
        {
            GObject * pObj = GetObjFromItem(*it);
            if (pParent == pObj->getParent() && bLayer == pObj->isLayer())
            {
                selectednodes.push_back(pObj);
            }
        }
        Reselect();
    }
}

void QTUI_Layer_Tree::SLT_ItemDoubleClicked(QTreeWidgetItem *pItem, int column)
{
    QTDLG_LayerProperty * pDlg = new QTDLG_LayerProperty();

    GObject * pObj = GetObjFromItem(pItem);
    pDlg->SetInitalValue(pObj->getDisplayName(), pObj->getLineColorSet(), pObj->isDisplayVisible(), pObj->isDisplayLocked());
    int ret = pDlg->exec();
    if (ret == QDialog::Accepted)
    {
        QString str = pDlg->GetName();
		if (str != pObj->getDisplayName())
		{
			str.insert(0, "\"");
			str.insert(str.length(), "\"");
			string stdstr = str.toStdString();
			MainInterface::getInstance().OnCommandWithParam(
				COMM_SETNODENAME, 
				CCCWPARAM_I(pObj->getID()),
				CCCWPARAM_S(&stdstr),
				NULL);
		}
        pObj->setLineColor(pDlg->GetLineColorSet());
        pObj->setDisplayVisible(pDlg->GetVisible());
        pObj->setDisplayLock(pDlg->GetLocked());
        UpdateNodeInfo(pItem, pObj);
    }
    delete pDlg;
}

void QTUI_Layer_Tree::OnFrameUpdate()
{
	list <GObject *> * plstGUISelected = MarqueeSelect::getInstance().OnGetSelectedNodes();
	if (*plstGUISelected == lstLastSelectedNodes)
	{
		return;
	}
	else
	{
		lstLastSelectedNodes = *plstGUISelected;
	}
	DeselectButtons();

	if (!plstGUISelected->empty())
	{
		bool bCleared = false;
		list<GObject *> lsttemp;
		for (list<GObject *>::iterator it=plstGUISelected->begin(); it!=plstGUISelected->end(); ++it)
		{
			GObject * pObj = *it;
			QTreeWidgetItem * pItem = FindItemByObj(pObj);
			if (pItem)
			{
				QTUI_Layer_SelectionButton * pSelectionButton = (QTUI_Layer_SelectionButton *)this->itemWidget(pItem, _UILT_COLUMN_SELECT);
				if (!pSelectionButton->isChecked())
				{
					pSelectionButton->UpdateGUISelection(pObj, true);
					if (!bCleared)
					{
						selectednodes.clear();
						if (!lsttemp.empty())
						{
							for (list<GObject *>::iterator jt=lsttemp.begin(); jt!=lsttemp.end(); ++jt)
							{
								selectednodes.push_back(*jt);
							}
						}
						bCleared = true;
					}
					selectednodes.push_back(pObj);
				}
				else if (!bCleared)
				{
					lsttemp.push_back(pObj);
				}
			}
		}
		if (bCleared)
		{
			Reselect();
		}
	}
}

void QTUI_Layer_Tree::DeselectButtons( QTreeWidgetItem * pParent/*=NULL*/ )
{
	if (!pParent)
	{
		pParent = this->invisibleRootItem();
	}

	int nCount = pParent->childCount();
	if (!nCount)
	{
		return;
	}
	for (int i=0; i<nCount; i++)
	{
		QTreeWidgetItem *pItem = pParent->child(i);
		if (pItem)
		{
			((QTUI_Layer_SelectionButton *)this->itemWidget(pItem, _UILT_COLUMN_SELECT))->UpdateGUISelection(GetObjFromItem(pItem), false);
			DeselectButtons(pItem);
		}
	}
	return;
}

void QTUI_Layer_Tree::OnButtonSelect( GObject * pObj, bool bSelected )
{
	ASSERT(pObj);
	QTreeWidgetItem * pItem = FindItemByObj(pObj);
	ASSERT(pItem);
	ChangeChildrenButtonSelect(pItem, bSelected);

	selectednodes.clear();
	selectednodes.push_back(pObj);
	Reselect();
}

void QTUI_Layer_Tree::ChangeChildrenButtonSelect( QTreeWidgetItem * pParent, bool bSelect )
{
	ASSERT(pParent);
	int nCount = pParent->childCount();
	if (!nCount)
	{
		return;
	}
	for (int i=0; i<nCount; i++)
	{
		QTreeWidgetItem *pItem = pParent->child(i);
		if (pItem)
		{
			QTUI_Layer_SelectionButton * pSelectionButton = ((QTUI_Layer_SelectionButton *)this->itemWidget(pItem, _UILT_COLUMN_SELECT));
			if (pSelectionButton->isChecked() != bSelect)
			{
				pSelectionButton->click();
			}
			ChangeChildrenButtonSelect(pItem, bSelect);
		}
	}
}