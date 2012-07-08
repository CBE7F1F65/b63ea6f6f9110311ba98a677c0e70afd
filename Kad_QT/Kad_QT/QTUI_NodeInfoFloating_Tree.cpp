#include "stdafx.h"
#include "QTUI_NodeInfoFloating_Tree.h"
#include "qmaininterface.h"
#include "qtui_nodeinfofloating_widget.h"
#include "StringManager.h"
#include "MainInterface.h"
#include "Command.h"

#include "IconManager.h"
#include "GObjectManager.h"
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
    pHoveringNode = NULL;

	bInternalChanging = false;
}

void QTUI_NodeInfoFloating_Tree::ShowNodeInfo(GObject *pObj, bool bContext)
{
    QTUI_NodeInfoFloating_Widget * pWidget = (QTUI_NodeInfoFloating_Widget *)this->parentWidget();
    if ((pWidget->isWidgetWindowHidden() || pDisplyObj!=pObj) && pObj)
    {
        pDisplyObj = pObj;
        bContextMode = bContext;

        pWidget->ShowWidgetWindow(bContext);

        Clear();
        UpdateNodeInfo(pObj);
    }
    else if (!pObj/* && !bContextMode*/)
    {
        pDisplyObj = NULL;
        if (!pWidget->isWidgetWindowHidden())
        {
            Clear();
            pWidget->HideWidgetWindow();
            QMainInterface::getInstance().GetPGLView()->setFocus();
        }
    }
}

GObject *QTUI_NodeInfoFloating_Tree::GetHoveringNode()
{
    return pHoveringNode;
}

void QTUI_NodeInfoFloating_Tree::enterEvent( QEvent * e )
{
	GObjectManager::getInstance().SetRenderUILayerIndicators(true);
}

void QTUI_NodeInfoFloating_Tree::leaveEvent( QEvent * e )
{
	GObjectManager::getInstance().SetRenderUILayerIndicators(false);
}

void QTUI_NodeInfoFloating_Tree::mouseMoveEvent(QMouseEvent *e)
{
    QTreeWidgetItem * pItem = this->itemAt(e->pos());

    bool bSet = false;
    if (pItem)
    {
        QTUINIFT_NodeRelationship * pRel = GetRelationshipFromItem(pItem);
        if (pRel)
        {
            pHoveringNode = pRel->pThis;
            bSet = true;
        }
    }
    if (!bSet)
    {
        pHoveringNode = NULL;
    }
    QTreeWidget::mouseMoveEvent(e);
}

void QTUI_NodeInfoFloating_Tree::SLT_AdjustSize()
{
    AdjustSize();
}

void QTUI_NodeInfoFloating_Tree::SLT_ItemChanged(QTreeWidgetItem *pItem, int nColumn)
{
	if (bInternalChanging)
	{
		return;
	}
	bInternalChanging = true;
    if (nColumn == _UINIFT_COLUMN_NAME)
    {
		QTUINIFT_NodeRelationship * pRel = GetRelationshipFromItem(pItem);
        if (pRel)
        {
            if (pItem->flags()&Qt::ItemIsUserCheckable)
            {
                int checkstate = pItem->checkState(_UINIFT_COLUMN_NAME);

                GObject * pObj = pRel->pThis;
                GObject * pRelationParent = pRel->pRelationParent;
                int nRelationType = pRel->nRelationType;
                MainInterface * pmain = &MainInterface::getInstance();
                switch (nRelationType)
                {
                case QTUINIFT_RELATIONSHIP_CLINGTO:
                {
                    GPoint * pPoint = (GPoint *)pObj;
                    if (checkstate == Qt::Unchecked)
                    {
                        if (pPoint->isClingTo(pRelationParent))
                        {
                            pmain->OnCommandWithParam(
                                        COMM_CLING,
                                        CCCWPARAM_I(pObj->getID()),
                                        CCCWPARAM_I(-1),
                                        CCCWPARAM_F(0.0f),
                                        NULL);
                        }
                    }
                    else
                    {
                        if (!pPoint->isClingTo(pRelationParent))
                        {
                            pmain->OnCommandWithParam(
                                        COMM_CLING,
                                        CCCWPARAM_I(pObj->getID()),
                                        CCCWPARAM_I(pRelationParent->getID()),
                                        CCCWPARAM_F(pRel->fClingProportion),
                                        NULL);
                        }
                    }
                }
                    break;
                case QTUINIFT_RELATIONSHIP_CLINGBY:
                {
                    GPoint * pPoint = (GPoint *)pRelationParent;
                    if (checkstate == Qt::Unchecked)
                    {
                        if (pPoint->isClingTo(pObj))
                        {
                            pmain->OnCommandWithParam(
                                        COMM_CLING,
                                        CCCWPARAM_I(pRelationParent->getID()),
                                        CCCWPARAM_I(-1),
                                        CCCWPARAM_F(0.0f),
                                        NULL);
                        }
                    }
                    else
                    {
                        if (!pPoint->isClingTo(pObj))
                        {
                            pmain->OnCommandWithParam(
                                        COMM_CLING,
                                        CCCWPARAM_I(pRelationParent->getID()),
                                        CCCWPARAM_I(pObj->getID()),
                                        CCCWPARAM_F(pRel->fClingProportion),
                                        NULL);
                        }
                    }
                }
                    break;
                case QTUINIFT_RELATIONSHIP_MERGEWITH:
                {
                    GPoint * pPoint = (GPoint *)pObj;
                    GPoint * pAnotherPoint = (GPoint *)pRelationParent;
                    if (checkstate == Qt::Unchecked)
                    {
                        if (pPoint->isMergeWith(pAnotherPoint))
                        {
                            pmain->OnCommandWithParam(
                                        COMM_SEPARATE,
                                        CCCWPARAM_I(pPoint->getID()),
                                        CCCWPARAM_I(pAnotherPoint->getID()),
                                        NULL);
                        }
                    }
                    else
                    {
                        if (!pPoint->isMergeWith(pAnotherPoint))
                        {
                            pmain->OnCommandWithParam(
                                        COMM_MERGE,
                                        CCCWPARAM_I(pPoint->getID()),
                                        CCCWPARAM_I(pAnotherPoint->getID()),
                                        NULL);
                        }
                    }
                }
                    break;
                case QTUINIFT_RELATIONSHIP_BINDWITH:
                {
                    GHandlePoint * pPoint = (GHandlePoint *)pObj;
                    GHandlePoint * pAnotherPoint = (GHandlePoint *)pRelationParent;
                    if (checkstate == Qt::Unchecked)
                    {
                        if (pPoint->isBindWith(pAnotherPoint))
                        {
                            pmain->OnCommandWithParam(
                                        COMM_BINDHANDLE,
                                        CCCWPARAM_I(pPoint->getID()),
                                        CCCWPARAM_I(-1),
                                        NULL);
                        }
                    }
                    else
                    {
                        if (!pPoint->isBindWith(pAnotherPoint))
                        {
                            int nChildren = pItem->parent()->childCount();
                            for (int i=0; i<nChildren; i++)
                            {
                                QTreeWidgetItem * pChildItem = pItem->parent()->child(i);
                                QTUINIFT_NodeRelationship * pRel = GetRelationshipFromItem(pChildItem);
                                if (pRel)
                                {
                                    if (pRel->pRelationParent == pPoint->getBindWith())
                                    {
                                        pChildItem->setData(_UINIFT_COLUMN_NAME, Qt::CheckStateRole, Qt::Unchecked);
                                    }
                                }
                            }

                            pmain->OnCommandWithParam(
                                        COMM_BINDHANDLE,
                                        CCCWPARAM_I(pPoint->getID()),
                                        CCCWPARAM_I(pAnotherPoint->getID()),
                                        NULL);
                        }
                    }
                }
                    break;
                }
            }
        }
    }
	bInternalChanging = false;
}

void QTUI_NodeInfoFloating_Tree::SLT_ItemClicked(QTreeWidgetItem *pItem, int nColumn)
{
    if (pItem->childCount())
    {
        if (!GetRelationshipFromItem(pItem))
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
}

void QTUI_NodeInfoFloating_Tree::SLT_ItemDoubleClicked(QTreeWidgetItem *pItem, int nColumn)
{
    if (nColumn == _UINIFT_COLUMN_NAME)
    {
        QTUINIFT_NodeRelationship * pRel = GetRelationshipFromItem(pItem);
        if (pRel)
        {
            GObject * pObj = pRel->pThis;
            if (pObj)
            {
                UpdateNodeInfo(pObj, pItem);
            }
        }
    }
}

void QTUI_NodeInfoFloating_Tree::Clear()
{
    vecNodeRelationship.clear();
    this->clear();
}

void QTUI_NodeInfoFloating_Tree::UpdateNodeInfo(GObject *pObj, QTreeWidgetItem *pParent)
{
    if (!pParent)
    {
        pParent = this->invisibleRootItem();
    }
    pParent->takeChildren();

    StringManager * psm = &StringManager::getInstance();

	QString str;
	const char * strID = psm->GetNodeInfoIDName();
	const char * strName = psm->GetNodeInfoNameName();

	/************************************************************************/
	/* Info                                                                 */
	/************************************************************************/

    QTreeWidgetItem * pInfoItem = NewItemWithText(pParent, psm->GetNodeInfoInfoName());

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
                NewItemWithText(pClingToItem, str, NewRelationship(pObj, pClingToObj, QTUINIFT_RELATIONSHIP_CLINGTO, pPoint->getClingProportion()));
            }
            list<GPoint *> * plMergeWith = pPoint->getMergeWith();
            if (!plMergeWith->empty())
            {
                QTreeWidgetItem * pMergeWithItem = NewItemWithText(pInfoItem, psm->GetNodeInfoMergeWithName());

                for (list<GPoint *>::iterator it=plMergeWith->begin(); it!=plMergeWith->end(); ++it)
                {
                    GPoint * pMergedPoint = *it;

                    str.sprintf("%s: %d, %s: %s", strID, pMergedPoint->getID(), strName, pMergedPoint->getDisplayName());
                    NewItemWithText(pMergeWithItem, str, NewRelationship(pMergedPoint, pObj, QTUINIFT_RELATIONSHIP_MERGEWITH));
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
                    NewItemWithText(pClingByItem, str, NewRelationship(pObj, pClingPoint, QTUINIFT_RELATIONSHIP_CLINGBY, pClingPoint->getClingProportion()));
                }
            }
        }
    }
    if (pObj->isHandlePoint())
    {
        GHandlePoint * pHandle = (GHandlePoint *)pObj;
        GAnchorPoint * pAnchor = (GAnchorPoint *)pHandle->GetAnchor();
        list<GPoint *> * plMergeWith = pAnchor->getMergeWith();
        GHandlePoint * pBindHandle = pHandle->getBindWith();

        if (pBindHandle || !plMergeWith->empty())
        {
            QTreeWidgetItem * pBindWithItem = NewItemWithText(pInfoItem, psm->GetNodeInfoBindWithName());

            for (list<GPoint *>::iterator it=plMergeWith->begin(); it!=plMergeWith->end(); ++it)
            {
                GPoint * pMergedPoint = *it;
                if (pMergedPoint->isAnchorPoint())
                {
                    GLine * pLine = (GLine *)pMergedPoint->getLine();
                    if (!pLine->isStraightLine())
                    {
                        GHandlePoint * pMergedHandle = ((GAnchorPoint *)pMergedPoint)->GetHandle();

                        str.sprintf("%s: %d, %s: %s", strID, pMergedHandle->getID(), strName, pMergedHandle->getDisplayName());
                        NewItemWithText(pBindWithItem, str, NewRelationship(pObj, pMergedHandle, QTUINIFT_RELATIONSHIP_BINDWITH), pMergedHandle==pBindHandle);
                    }
                }
            }
        }
    }

    pInfoItem->setExpanded(true);

	/************************************************************************/
	/* Hierarchy                                                            */
	/************************************************************************/

	QTreeWidgetItem * pHierarchy = NewItemWithText(pParent, psm->GetNodeInfoHierarchyName());

	GObject * pParentObj = pObj->getParent();
	if (pParentObj && pParentObj->getID())
	{
		str.sprintf("%s: %s: %d, %s: %s", psm->GetNodeInfoParentName(), strID, pParentObj->getID(), strName, pParentObj->getDisplayName());
		NewItemWithText(pHierarchy, str, NewRelationship(pParentObj, pObj, QTUINIFT_RELATIONSHIP_PARENT));
	}
	if (!pObj->getChildren()->empty())
	{
		QTreeWidgetItem * pChildrenItem = NewItemWithText(pParent, psm->GetNodeInfoChildrenName());
		for (list<GObject *>::iterator it=pObj->getChildren()->begin(); it!=pObj->getChildren()->end(); ++it)
		{
			GObject * pChildObj = *it;
			str.sprintf("%s: %d, %s: %s", strID, pChildObj->getID(), strName, pChildObj->getDisplayName());
			NewItemWithText(pChildrenItem, str, NewRelationship(pChildObj, pObj, QTUINIFT_RELATIONSHIP_CHILD));
		}
	}

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

	nTotalHeight += 8;

    QTUI_NodeInfoFloating_Widget * pWidget = (QTUI_NodeInfoFloating_Widget *)this->parentWidget();
    pWidget->AdjustSize(nTotalWidth, nTotalHeight);
}

QTreeWidgetItem *QTUI_NodeInfoFloating_Tree::NewItemWithText(QTreeWidgetItem *pParent, QString str, int relID, bool bChecked)
{
    QTreeWidgetItem * pItem = new QTreeWidgetItem();

    pItem->setText(_UINIFT_COLUMN_NAME, str);
    if (relID >= 0)
    {
        pItem->setData(_UINIFT_COLUMN_NAME, Qt::UserRole, relID);
		if (vecNodeRelationship[relID].nRelationType < _QTUINIFT_RELATIONSHIP_NOCHECKBEGIN)
		{
			pItem->setData(_UINIFT_COLUMN_NAME, Qt::CheckStateRole, bChecked?Qt::Checked:Qt::Unchecked);
		}
//        pItem->setFlags(pItem->flags()|Qt::ItemIsUserCheckable);
    }
    pParent->addChild(pItem);
	if (bContextMode)
	{
		pItem->setExpanded(true);
	}

    return pItem;
}

int QTUI_NodeInfoFloating_Tree::NewRelationship(GObject *pObj, GObject *pRelationParent, int nRelationType, float fProportion)
{
    QTUINIFT_NodeRelationship _rel(pObj, pRelationParent, nRelationType, fProportion);
    vecNodeRelationship.push_back(_rel);
    return vecNodeRelationship.size()-1;
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

QTUINIFT_NodeRelationship * QTUI_NodeInfoFloating_Tree::GetRelationshipFromItem(QTreeWidgetItem *pItem)
{
    Q_ASSERT(pItem);
    bool bOk;
    int relID = pItem->data(_UINIFT_COLUMN_NAME, Qt::UserRole).toInt(&bOk);
    if (bOk)
    {
        return &(vecNodeRelationship[relID]);
    }
    return NULL;
}