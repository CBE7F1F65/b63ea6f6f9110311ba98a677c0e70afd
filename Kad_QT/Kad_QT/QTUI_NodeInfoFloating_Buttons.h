#pragma once

#include <QPushButton>
#include <QTreeWidget>
#include "GPiece.h"

/************************************************************************/
/* Transform                                                            */
/************************************************************************/

class QTUI_NodeInfoFloating_TransformButton : public QPushButton
{
	Q_OBJECT
public:
	explicit QTUI_NodeInfoFloating_TransformButton(GLine * pLine);

protected:
	void ChangeState(bool bInitial);

private slots:
	void SLT_ButtonClicked();

protected:
	GLine * pLine;
};

/************************************************************************/
/* SwapBeginEnd                                                         */
/************************************************************************/

class QTUI_NodeInfoFloating_SwapBeginEndButton : public QPushButton
{
	Q_OBJECT
public:
	explicit QTUI_NodeInfoFloating_SwapBeginEndButton(GLine * pLine);

private slots:
	void SLT_ButtonClicked();

protected:
	GLine * pLine;
};

/************************************************************************/
/* Cling                                                                */
/************************************************************************/

class QTUI_NodeInfoFloating_ChangeClingTypeButton : public QPushButton
{
	Q_OBJECT
public:
	explicit QTUI_NodeInfoFloating_ChangeClingTypeButton(GPoint * pPoint, QTreeWidgetItem * pParentItem, int nTextColumn, bool bClingTo);

protected:
	void ChangeState(bool bInitial);

private slots:
	void SLT_ButtonClicked();

protected:
	GPoint * pClingPoint;
	QTreeWidgetItem * pItem;
	bool bTo;
	int nTextColumn;
};

/************************************************************************/
/* Isolate                                                              */
/************************************************************************/

class QTUI_NodeInfoFloating_IsolateButton : public QPushButton
{
	Q_OBJECT
public:
	explicit QTUI_NodeInfoFloating_IsolateButton(GObject * pObj);

private slots:
	void SLT_ButtonClicked();

protected:
	GObject * pObj;
};