#ifndef QTUI_LAYER_TREE_H
#define QTUI_LAYER_TREE_H

#include <QTreeWidget>
#include "GObject.h"

class QTUI_Layer_Tree : public QTreeWidget
{
    Q_OBJECT
public:
    explicit QTUI_Layer_Tree(QWidget *parent = 0);

public:
    void RebuildTree( GObject * changebase, GObject * activeitem );
    void DeleteItemsUnderObject(GObject * pObj);
    
signals:
    
public slots:
    
};

#endif // QTUI_LAYER_TREE_H
