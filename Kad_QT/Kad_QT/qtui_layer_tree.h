#ifndef QTUI_LAYER_TREE_H
#define QTUI_LAYER_TREE_H

#include <QTreeWidget>
#include "GLayer.h"

class QTUI_Layer_Tree : public QTreeWidget
{
    Q_OBJECT
public:
    explicit QTUI_Layer_Tree(QWidget *parent = 0);

public:
    void RebuildTree( GObject * changebase, GObject * activeitem );
    list<GObject *> * GetActiveNodes();
    bool GetDragDroppedNodes( GLayer ** pLayerNode, GObject ** pAfterNode );

protected:
    virtual void dropEvent(QDropEvent *e);
    virtual void dragMoveEvent(QDragMoveEvent *e);
    virtual void resizeEvent(QResizeEvent * e);
    
private:
    void AdjustSize();
    QTreeWidgetItem * FindItemByObj(GObject * pObj, QTreeWidgetItem * pParent=NULL);
    GObject * GetObjFromItem(QTreeWidgetItem * pItem);
    void DeleteItemsUnderWidget(QTreeWidgetItem * pItem);
    void BuildChildren(QTreeWidgetItem * pItem, GObject * pObjParent);

    bool IsObjInSelection(GObject * pObj);
    void Reselect();

    void SetItemData(QTreeWidgetItem * pItem, GObject * pObj);
    void SetItemVisible(QTreeWidgetItem * pItem, bool bDisplayVisible, bool bRecDisplayVisable, GObject * pObj);
    void SetItemLock(QTreeWidgetItem * pItem, bool bDisplayLocked, bool bRecDisplayLocked, GObject * pObj);
    void SetItemLineColor(QTreeWidgetItem * pItem, DWORD dwLineColor, GObject * pObj);
    void SetItemTreeInfo(QTreeWidgetItem * pItem, QString strDisplayName, bool bFolded, GObject * pObj);
    void SetItemLineSelect(QTreeWidgetItem * pItem, bool bSelected, GObject * pObj);
    void SetItemFrameColor(QTreeWidgetItem * pItem, DWORD dwFrameColor, GObject * pObj);

    list<GObject *> selectednodes;
    GLayer * pDragDropLayer;
    GObject * pDragDropAfter;

signals:
    
public slots:
    void SLT_ItemCollapsed(QTreeWidgetItem * pItem);
    void SLT_ItemExpanded(QTreeWidgetItem * pItem);
    void SLT_NewLayer();
    void SLT_NewSubLayer();
    void SLT_CopyItems();
    void SLT_DeleteItems();
    void SLT_UpdateSelectionNodes();
    
};

#endif // QTUI_LAYER_TREE_H
