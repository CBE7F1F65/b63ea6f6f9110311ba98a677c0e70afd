#include "qtui_layer_tree.h"
#include "qmaininterface.h"

#define _UILT_ICONSIZE  ICMSIZE_MIDDLE

enum{
    _UILT_COLUMN_VISIBLE,
    _UILT_COLUMN_LOCK,
    _UILT_COLUMN_LINECOLOR,
    _UILT_COLUMN_TREE,
    _UILT_COLUMN_LINESELECT,
    _UILT_COLUMN_FRAMECOLOR,

    _UILT_COLUMNCOUNT,
};

QTUI_Layer_Tree::QTUI_Layer_Tree(QWidget *parent) :
    QTreeWidget(parent)
{
    QMainInterface::getInstance().SetPLayerTree(this);
    this->setColumnCount(_UILT_COLUMNCOUNT);
}

void QTUI_Layer_Tree::RebuildTree(GObject *changebase, GObject *activeitem)
{
    if (activeitem)
    {

        QTreeWidgetItem * pTreeItem = new QTreeWidgetItem(invisibleRootItem());
        pTreeItem->setText(_UILT_COLUMN_TREE, activeitem->getDisplayName());
//        this->addTopLevelItem(pTreeItem);
        /*
        QList<QTreeWidgetItem *> items;
        for (int i = 0; i < 10; ++i)
            items.append(new QTreeWidgetItem((QTreeWidget*)0, QStringList(QString("item: %1").arg(i))));
//        this->insertTopLevelItems(0, items);
        this->addTopLevelItems(items);
        */
    }
}

void QTUI_Layer_Tree::DeleteItemsUnderObject(GObject *pObj)
{
}
