#ifndef QTUI_NODEINFOFLOATING_TREE_H
#define QTUI_NODEINFOFLOATING_TREE_H

#include <QTreeWidget>
#include "GObject.h"

class QTUI_NodeInfoFloating_Tree : public QTreeWidget
{
    Q_OBJECT
public:
    explicit QTUI_NodeInfoFloating_Tree(QWidget *parent = 0);

    void ShowNodeInfo(GObject * pObj=NULL, bool bContext=false);

signals:

    
public slots:
    void SLT_AdjustSize();
    void SLT_ItemClicked(QTreeWidgetItem * pItem, int nColumn);
    void SLT_ItemDoubleClicked(QTreeWidgetItem * pItem, int nColumn);

private:
    void UpdateNodeInfo(GObject * pObj, QTreeWidgetItem * pParent=NULL);
    void AdjustSize();

    QTreeWidgetItem * NewItemWithText(QTreeWidgetItem *pParent, QString str, GObject * pObj=NULL);

    int CalculateTotalHeight(QTreeWidgetItem * pParent=0, int height=0);

    GObject * pDisplyObj;
    bool bContextMode;
};

#endif // QTUI_NODEINFOFLOATING_TREE_H
