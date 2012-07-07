#include "stdafx.h"
#include "QTUI_NodeInfoFloating_Tree.h"
#include "qmaininterface.h"
#include "qtui_nodeinfofloating_widget.h"
#include "StringManager.h"

#include "IconManager.h"

#include "GLine.h"

#define _UINIFT_ICONSIZE  ICMSIZE_MIDDLE
#define _UINIFT_INDENTATION   16

#define _UINIFT_ROWHEIGHT   18  // ToDo!!!!!

enum{
    _UINIFT_COLUMN_NAME,
    _UINIFT_COLUMN_BUTTON,

    _UINIFT_COLUMNCOUNT,
};

QTUI_NodeInfoFloating_Tree::QTUI_NodeInfoFloating_Tree(QWidget *parent) :
    QTreeWidget(parent)
{
    QMainInterface::getInstance().SetPNodeInfoFloatingTree(this);

    this->setIndentation(_UINIFT_INDENTATION);

    pDisplyObj = NULL;
    bContextMode = false;
}

void QTUI_NodeInfoFloating_Tree::ShowNodeInfo(GObject *pObj, bool bContext)
{
    QTUI_NodeInfoFloating_Widget * pWidget = (QTUI_NodeInfoFloating_Widget *)this->parentWidget();
    if ((pWidget->isWidgetWindowHidden() || pDisplyObj!=pObj) && pObj)
    {
        pDisplyObj = pObj;
        bContextMode = bContext;

        pWidget->ShowWidgetWindow(bContext);

        this->clear();
        UpdateNodeInfo(pObj);
    }
    else if (!pObj/* && !bContextMode*/)
    {
        pDisplyObj = NULL;
        if (!pWidget->isWidgetWindowHidden())
        {
            pWidget->HideWidgetWindow();
            QMainInterface::getInstance().GetPGLView()->setFocus();
        }
    }
}

void QTUI_NodeInfoFloating_Tree::SLT_AdjustSize()
{
    AdjustSize();
}

void QTUI_NodeInfoFloating_Tree::SLT_ItemClicked(QTreeWidgetItem *pItem, int nColumn)
{
    if (pItem->childCount())
    {
        if (pItem->isExpanded())
        {
            pItem->setExpanded(false);
        }
        else
        {
            pItem->setExpanded(true);
        }
    }
}

void QTUI_NodeInfoFloating_Tree::SLT_ItemDoubleClicked(QTreeWidgetItem *pItem, int nColumn)
{
    if (nColumn == _UINIFT_COLUMN_NAME)
    {
        bool bOk;
        GObject * pObj = (GObject *)(pItem->data(_UINIFT_COLUMN_NAME, Qt::UserRole).toUInt(&bOk));
        if (bOk && pObj)
        {
            UpdateNodeInfo(pObj, pItem);
        }
    }
}

void QTUI_NodeInfoFloating_Tree::UpdateNodeInfo(GObject *pObj, QTreeWidgetItem *pParent)
{
    if (!pParent)
    {
        pParent = this->invisibleRootItem();
    }

    StringManager * psm = &StringManager::getInstance();

    QTreeWidgetItem * pInfoItem = NewItemWithText(pParent, psm->GetNodeInfoInfoName());

    QString str;

    const char * strID = psm->GetNodeInfoIDName();
    const char * strName = psm->GetNodeInfoNameName();

    str.sprintf("%s: %d", strID, pObj->getID());
    NewItemWithText(pInfoItem, str);

    str.sprintf("%s: %s", strName, pObj->getDisplayName());
    NewItemWithText(pInfoItem, str);

    str.sprintf("%s: (%f, %f)", psm->GetNodeInfoPositionName(), pObj->getX(), pObj->getY());
    NewItemWithText(pInfoItem, str);

    if (pObj->canAttach())
    {
        if (pObj->isPoint())
        {
            GPoint * pPoint = (GPoint *)pObj;
            GObject * pClingToObj = pPoint->getClingTo();
            if (pClingToObj)
            {
                QTreeWidgetItem * pClingToItem = NewItemWithText(pInfoItem, psm->GetNodeInfoClingToName());

                str.sprintf("%s: %d, %s: %s", strID, pClingToObj->getID(), strName, pClingToObj->getDisplayName());
                NewItemWithText(pClingToItem, str, pClingToObj);
            }
            list<GPoint *> * plMergeWith = pPoint->getMergeWith();
            if (!plMergeWith->empty())
            {
                QTreeWidgetItem * pMergeWithItem = NewItemWithText(pInfoItem, psm->GetNodeInfoMergeWithName());
                for (list<GPoint *>::iterator it=plMergeWith->begin(); it!=plMergeWith->end(); ++it)
                {
                    GPoint * pMergedPoint = *it;

                    str.sprintf("%s: %d, %s: %s", strID, pMergedPoint->getID(), strName, pMergedPoint->getDisplayName());
                    NewItemWithText(pMergeWithItem, str, pMergedPoint);
                }
            }
        }
    }
    if (pObj->canBeClingTo())
    {
        if (pObj->isLine())
        {
            GLine * pLine = (GLine *)pObj;
            list<GPoint *> * plClingBy = pLine->getClingBy();
            if (!plClingBy->empty())
            {
                QTreeWidgetItem * pClingByItem = NewItemWithText(pInfoItem, psm->GetNodeInfoClingByName());
                for (list<GPoint *>::iterator it=plClingBy->begin(); it!=plClingBy->end(); ++it)
                {
                    GPoint * pClingPoint = *it;

                    str.sprintf("%s: %d, %s: %s", strID, pClingPoint->getID(), strName, pClingPoint->getDisplayName());
                    NewItemWithText(pClingByItem, str, pClingPoint);
                }
            }
        }
    }

    pInfoItem->setExpanded(true);
    AdjustSize();
}

void QTUI_NodeInfoFloating_Tree::AdjustSize()
{
    int nColumn = this->columnCount();
    int nTotalWidth = 0;
    for (int i=0; i<nColumn; i++)
    {
        this->resizeColumnToContents(i);
        nTotalWidth += this->columnWidth(i);
    }

    int nTotalHeight = CalculateTotalHeight();

    QTUI_NodeInfoFloating_Widget * pWidget = (QTUI_NodeInfoFloating_Widget *)this->parentWidget();
    this->adjustSize();
    pWidget->AdjustSize(nTotalWidth, nTotalHeight);
}

QTreeWidgetItem *QTUI_NodeInfoFloating_Tree::NewItemWithText(QTreeWidgetItem *pParent, QString str, GObject *pObj)
{
    QTreeWidgetItem * pItem = new QTreeWidgetItem(pParent);

    pItem->setText(_UINIFT_COLUMN_NAME, str);
    if (pObj)
    {
        pItem->setData(_UINIFT_COLUMN_NAME, Qt::UserRole, (uint)pObj);
    }

    return pItem;
}

int QTUI_NodeInfoFloating_Tree::CalculateTotalHeight(QTreeWidgetItem *pParent, int height)
{
    if (!pParent)
    {
        pParent = this->invisibleRootItem();
    }
    else if (!pParent->isExpanded())
    {
        return height;
    }
    int n = pParent->childCount();
    for (int i=0; i<n; i++)
    {
        height += _UINIFT_ROWHEIGHT;
        height = CalculateTotalHeight(pParent->child(i), height);
    }
    return height;
}
