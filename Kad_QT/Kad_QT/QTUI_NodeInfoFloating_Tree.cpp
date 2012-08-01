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

#define _UINIFT_ICONSIZE  ICMSIZE_SMALL
#define _UINIFT_INDENTATION   16

#define _UINIFT_ROWHEIGHT   18  // ToDo!!!!!

enum{
	_UINIFT_PBDATAID_TREEWIDGETITEM,
};

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

    this->setAttribute(Qt::WA_NoBackground, true);

    pDisplyObj = NULL;
    bContextMode = false;
    pHoveringNode = NULL;

	bInternalChanging = false;
	nRelIndex = 0;
}

void QTUI_NodeInfoFloating_Tree::ShowNodeInfo(GObject *pObj, bool bContext)
{
    QTUI_NodeInfoFloating_Widget * pWidget = (QTUI_NodeInfoFloating_Widget *)this->parentWidget();
    if ((pWidget->isWidgetWindowHidden() || pDisplyObj!=pObj) && pObj)
    {
        pDisplyObj = pObj;
        bContextMode = bContext;

		Clear();
		pWidget->ShowWidgetWindow(bContext);
		pHoveringNode = pObj;

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
        QTUIUD_NIFT_NodeRelationship * pRel = GetRelationshipFromItem(pItem);
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
		QTUIUD_NIFT_NodeRelationship * pRel = GetRelationshipFromItem(pItem);
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
                                        COMM_DEMERGE,
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
                                QTUIUD_NIFT_NodeRelationship * pRel = GetRelationshipFromItem(pChildItem);
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
		if (pItem->childCount())
		{
			return;
		}
        QTUIUD_NIFT_NodeRelationship * pRel = GetRelationshipFromItem(pItem);
        if (pRel && pRel->nRelationType >= 0)
        {
            GObject * pObj = pRel->pThis;
            if (pObj)
            {
                UpdateNodeInfo(pObj, pItem);
            }
        }
    }
}

void QTUI_NodeInfoFloating_Tree::SLT_CalculateLengthButtonClicked()
{
	QPushButton * pButton = (QPushButton *)this->sender();
	QTUIUD_NIFT_PushButton * pData = (QTUIUD_NIFT_PushButton *)pButton->userData(_UINIFT_PBDATAID_TREEWIDGETITEM);
	QTreeWidgetItem * pSenderItem = pData->pParentItem;
	if (pSenderItem)
	{
		QTUIUD_NIFT_NodeRelationship * pRel = GetRelationshipFromItem(pSenderItem);
		if (pRel)
		{
			GLine * pLine = (GLine *)pRel->pThis;
			QString str;
			str.sprintf("%s: %f", StringManager::getInstance().GetNodeInfoLengthName(), pLine->getLength());
			pSenderItem->setText(_UINIFT_COLUMN_NAME, str);
		}
	}
}

void QTUI_NodeInfoFloating_Tree::SLT_TransformButtonClicked( bool bChecked )
{
	QPushButton * pButton = (QPushButton *)this->sender();
	QTUIUD_NIFT_PushButton * pData = (QTUIUD_NIFT_PushButton *)pButton->userData(_UINIFT_PBDATAID_TREEWIDGETITEM);
	QTreeWidgetItem * pSenderItem = pData->pParentItem;
	if (pSenderItem)
	{
		QTUIUD_NIFT_NodeRelationship * pRel = GetRelationshipFromItem(pSenderItem);
		if (pRel)
		{
			GLine * pLine = (GLine *)pRel->pThis;
			StringManager * psm = &StringManager::getInstance();
			if (bChecked)
			{
				pButton->setText(psm->GetNodeInfoToStrightLineName());
			}
			else
			{
				pButton->setText(psm->GetNodeInfoToBezierName());
			}
			
			MainInterface::getInstance().OnCommandWithParam(
				bChecked?COMM_TOBEZIER:COMM_TOSTRAIGHTLINE,
				CCCWPARAM_I(pRel->pThis->getID()),
				NULL
				);
		}
	}
}
void QTUI_NodeInfoFloating_Tree::Clear()
{
	pHoveringNode = NULL;
	nRelIndex = 0;
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

	if (pObj->isLine())
	{
		GLine * pLine = (GLine *)pObj;
		int relID = -1;
		QPushButton * pButton = NULL;
//		if (pLine->isLengthCalculated())
//		{
			str.sprintf("%s: %f", psm->GetNodeInfoLengthName(), pLine->getLength());
//		}
//		else
//		{
// 			str.sprintf("%s: %s", psm->GetNodeInfoLengthName(), psm->GetNodeInfoLengthCalculatePromptName());
// 			relID = NewRelationship(pObj);
// 		}
		QTreeWidgetItem * pLengthItem = NewItemWithText(pInfoItem, str, relID);
		/*
		if (relID >= 0)
		{
			pButton = NewButton(0, psm->GetNodeInfoLengthCalculatePromptName(), pLengthItem);
			this->setItemWidget(pLengthItem, _UINIFT_COLUMN_BUTTON, pButton);
		}
		*/
	}

    if (pObj->canAttach())
    {
        if (pObj->isPoint())
        {
            GPoint * pPoint = (GPoint *)pObj;
            GLine * pClingToLine = pPoint->getClingTo();
            if (pClingToLine)
            {
                QTreeWidgetItem * pClingToItem = NewItemWithText(pInfoItem, psm->GetNodeInfoClingToName());

                str.sprintf("%s: %d, %s: %s", strID, pClingToLine->getID(), strName, pClingToLine->getDisplayName());
                NewItemWithText(pClingToItem, str, NewRelationship(pObj, pClingToLine, QTUINIFT_RELATIONSHIP_CLINGTO, pPoint->getClingProportion()));
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
        GAnchorPoint * pAnchor = pHandle->GetAnchor();
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
                    GLine * pLine = pMergedPoint->getLine();
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

	QTreeWidgetItem * pHierarchyItem = NewItemWithText(pParent, psm->GetNodeInfoHierarchyName());

	GObject * pParentObj = pObj->getParent();
	if (pParentObj && pParentObj->getID())
	{
		str.sprintf("%s: %s: %d, %s: %s", psm->GetNodeInfoParentName(), strID, pParentObj->getID(), strName, pParentObj->getDisplayName());
		NewItemWithText(pHierarchyItem, str, NewRelationship(pParentObj, pObj, QTUINIFT_RELATIONSHIP_PARENT));
	}
	if (!pObj->getChildren()->empty())
	{
		QTreeWidgetItem * pChildrenItem = NewItemWithText(pHierarchyItem, psm->GetNodeInfoChildrenName());
		for (list<GObject *>::iterator it=pObj->getChildren()->begin(); it!=pObj->getChildren()->end(); ++it)
		{
			GObject * pChildObj = *it;
			str.sprintf("%s: %d, %s: %s", strID, pChildObj->getID(), strName, pChildObj->getDisplayName());
			NewItemWithText(pChildrenItem, str, NewRelationship(pChildObj, pObj, QTUINIFT_RELATIONSHIP_CHILD));
		}
	}

	/************************************************************************/
	/* Transform                                                            */
	/************************************************************************/

	if (pObj->isLine())
	{
		GLine * pLine = (GLine *)pObj;
		QTreeWidgetItem * pTranslationItem = NewItemWithText(pParent, psm->GetNodeInfoTransformName(), NewRelationship(pObj));

		QTreeWidgetItem * pBezierStraightLineItem = NewItemWithText(pTranslationItem, "");
		QPushButton * pButton=NULL;
		if (pLine->isStraightLine())
		{
			pButton = NewTransformButton(psm->GetNodeInfoToBezierName(), pTranslationItem, false);
		}
		else
		{
			pButton = NewTransformButton(psm->GetNodeInfoToStrightLineName(), pTranslationItem, true);
		}
		this->setItemWidget(pBezierStraightLineItem, _UINIFT_COLUMN_NAME, pButton);
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
		
		if (((QTUIUD_NIFT_NodeRelationship *)userData(relID))->nRelationType > _QTUINIFT_RELATIONSHIP_CHECKBEGIN)
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
    QTUIUD_NIFT_NodeRelationship * _rel = new QTUIUD_NIFT_NodeRelationship(pObj, pRelationParent, nRelationType, fProportion);
	QObjectUserData * pData = userData(nRelIndex);
	if (pData)
	{
		delete pData;
	}
	this->setUserData(nRelIndex, _rel);
	nRelIndex++;
	return nRelIndex-1;
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

QTUIUD_NIFT_NodeRelationship * QTUI_NodeInfoFloating_Tree::GetRelationshipFromItem(QTreeWidgetItem *pItem)
{
    Q_ASSERT(pItem);
    bool bOk;
    int relID = pItem->data(_UINIFT_COLUMN_NAME, Qt::UserRole).toInt(&bOk);
    if (bOk)
    {
        return (QTUIUD_NIFT_NodeRelationship *)userData(relID);
    }
    return NULL;
}

QPushButton * QTUI_NodeInfoFloating_Tree::NewCalculateLengthButton( QString strTooltip, QTreeWidgetItem * pParentItem )
{
	QPushButton * pButton = new QPushButton();
	pButton->setToolTip(strTooltip);
	pButton->setMaximumSize(_UINIFT_ICONSIZE, _UINIFT_ICONSIZE);
	pButton->setMinimumSize(_UINIFT_ICONSIZE, _UINIFT_ICONSIZE);
	connect(pButton, SIGNAL(clicked(bool)), this, SLOT(SLT_CalculateLengthButtonClicked()));
	QTUIUD_NIFT_PushButton * pData = new QTUIUD_NIFT_PushButton(pParentItem);
	pButton->setUserData(_UINIFT_PBDATAID_TREEWIDGETITEM, pData);
	return pButton;
}

QPushButton * QTUI_NodeInfoFloating_Tree::NewTransformButton( QString str, QTreeWidgetItem * pParentItem, bool bChecked )
{
	QPushButton * pButton = new QPushButton();
	pButton->setText(str);
	pButton->setCheckable(true);
	pButton->setChecked(bChecked);
	pButton->setMaximumHeight(_UINIFT_ICONSIZE);
	pButton->setMinimumHeight(_UINIFT_ICONSIZE);
	connect(pButton, SIGNAL(clicked(bool)), this, SLOT(SLT_TransformButtonClicked(bool)));
	QTUIUD_NIFT_PushButton * pData = new QTUIUD_NIFT_PushButton(pParentItem);
	pButton->setUserData(_UINIFT_PBDATAID_TREEWIDGETITEM, pData);
	return pButton;
}
