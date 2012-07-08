#ifndef QTUI_NODEINFOFLOATING_TREE_H
#define QTUI_NODEINFOFLOATING_TREE_H

#include <QTreeWidget>
#include "GObject.h"

enum{
    QTUINIFT_RELATIONSHIP_CLINGTO,
    QTUINIFT_RELATIONSHIP_CLINGBY,
	QTUINIFT_RELATIONSHIP_MERGEWITH,
	QTUINIFT_RELATIONSHIP_BINDWITH,

	_QTUINIFT_RELATIONSHIP_NOCHECKBEGIN,
	QTUINIFT_RELATIONSHIP_PARENT,
	QTUINIFT_RELATIONSHIP_CHILD,
};

class QTUINIFT_NodeRelationship{
public:
    QTUINIFT_NodeRelationship(){};
    QTUINIFT_NodeRelationship(GObject * pThisObj, GObject * pRelationParentObj, int nRType, float fProportion=0.0f)
    {
        pThis = pThisObj;
        pRelationParent = pRelationParentObj;
        nRelationType = nRType;
        fClingProportion = fProportion;
    };
    ~QTUINIFT_NodeRelationship(){};

    GObject * pThis;
    GObject * pRelationParent;
    int nRelationType;
    float fClingProportion;
};

class QTUI_NodeInfoFloating_Tree : public QTreeWidget
{
    Q_OBJECT
public:
    explicit QTUI_NodeInfoFloating_Tree(QWidget *parent = 0);

    void ShowNodeInfo(GObject * pObj=NULL, bool bContext=false);
    GObject * GetHoveringNode();

protected:
	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void enterEvent(QEvent * e);
	virtual void leaveEvent(QEvent * e);

signals:

    
public slots:
    void SLT_AdjustSize();
    void SLT_ItemChanged(QTreeWidgetItem * pItem, int nColumn);
    void SLT_ItemClicked(QTreeWidgetItem * pItem, int nColumn);
    void SLT_ItemDoubleClicked(QTreeWidgetItem * pItem, int nColumn);

private:
    void Clear();
    void UpdateNodeInfo(GObject * pObj, QTreeWidgetItem * pParent=NULL);
    void AdjustSize();

    QTreeWidgetItem * NewItemWithText(QTreeWidgetItem *pParent, QString str, int relID=-1, bool bChecked=true);
    int NewRelationship(GObject * pObj, GObject * pRelationParent, int nRelationType, float fProportion=0.0f);

    int CalculateTotalHeight(QTreeWidgetItem * pParent=0, int height=0);

    QTUINIFT_NodeRelationship * GetRelationshipFromItem(QTreeWidgetItem * pItem);

    GObject * pDisplyObj;
    bool bContextMode;

    GObject * pHoveringNode;

	bool bInternalChanging;

    QVector<QTUINIFT_NodeRelationship> vecNodeRelationship;
};

#endif // QTUI_NODEINFOFLOATING_TREE_H
