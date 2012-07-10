#ifndef QTUI_NODEINFOFLOATING_TREE_H
#define QTUI_NODEINFOFLOATING_TREE_H

#include <QTreeWidget>
#include "GObject.h"

enum{
	QTUINIFT_RELATIONSHIP_PARENT,
	QTUINIFT_RELATIONSHIP_CHILD,

	_QTUINIFT_RELATIONSHIP_CHECKBEGIN,

	QTUINIFT_RELATIONSHIP_CLINGTO,
	QTUINIFT_RELATIONSHIP_CLINGBY,
	QTUINIFT_RELATIONSHIP_MERGEWITH,
	QTUINIFT_RELATIONSHIP_BINDWITH,
};

class QTUIUD_NIFT_NodeRelationship : public QObjectUserData
{
public:
    QTUIUD_NIFT_NodeRelationship(GObject * pThisObj, GObject * pRelationParentObj, int nRType, float fProportion=0.0f)
    {
        pThis = pThisObj;
        pRelationParent = pRelationParentObj;
        nRelationType = nRType;
        fClingProportion = fProportion;
    };

    GObject * pThis;
    GObject * pRelationParent;
    int nRelationType;
    float fClingProportion;
};

class QTUIUD_NIFT_PushButton : public QObjectUserData
{
public:
	QTUIUD_NIFT_PushButton(QTreeWidgetItem * _pParentItem)
	{
		pParentItem = _pParentItem;
	}

	QTreeWidgetItem * pParentItem;
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

	void SLT_CalculateLengthButtonClicked();
	void SLT_TransformButtonClicked(bool bChecked);

private:
    void Clear();
    void UpdateNodeInfo(GObject * pObj, QTreeWidgetItem * pParent=NULL);
    void AdjustSize();

    QTreeWidgetItem * NewItemWithText(QTreeWidgetItem *pParent, QString str, int relID=-1, bool bChecked=true);
    int NewRelationship(GObject * pObj, GObject * pRelationParent=NULL, int nRelationType=-1, float fProportion=0.0f);

	QPushButton * NewCalculateLengthButton(QString strTooltip, QTreeWidgetItem * pParentItem);
	QPushButton * NewTransformButton(QString str, QTreeWidgetItem * pParentItem, bool bChecked);

    int CalculateTotalHeight(QTreeWidgetItem * pParent=0, int height=0);

    QTUIUD_NIFT_NodeRelationship * GetRelationshipFromItem(QTreeWidgetItem * pItem);

    GObject * pDisplyObj;
    bool bContextMode;

    GObject * pHoveringNode;

	bool bInternalChanging;
	int nRelIndex;
};

#endif // QTUI_NODEINFOFLOATING_TREE_H
