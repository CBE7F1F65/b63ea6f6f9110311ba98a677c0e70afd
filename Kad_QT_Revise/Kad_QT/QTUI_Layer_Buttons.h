#ifndef QTUI_LAYER_BUTTONS_H
#define QTUI_LAYER_BUTTONS_H

#include <QPushButton>
#include <QTreeWidget>
#include "GPiece.h"

/************************************************************************/
/* Selection                                                            */
/************************************************************************/

class QTUI_Layer_SelectionButton : public QPushButton
{
	Q_OBJECT
public:
	explicit QTUI_Layer_SelectionButton(GObject * pObj);
	void UpdateGUISelection(GObject * pObj, bool bChecked);
	void SetObj(GObject * _pObj){ASSERT(_pObj); pObj=_pObj;};
	
private slots:
	void SLT_ButtonClicked(bool bChecked);

protected:
	GObject * pObj;
};

#endif

/************************************************************************/
/* Visible                                                              */
/************************************************************************/

class QTUI_Layer_VisibleButton : public QPushButton
{
	Q_OBJECT
public:
	explicit QTUI_Layer_VisibleButton(GObject * pObj);
	void SetObj(GObject * _pObj){ASSERT(_pObj); pObj=_pObj;};

private slots:
	void SLT_ButtonClicked(bool bChecked);

protected:
	GObject * pObj;
};

/************************************************************************/
/* Lock                                                                 */
/************************************************************************/

class QTUI_Layer_LockButton : public QPushButton
{
	Q_OBJECT
public:
	explicit QTUI_Layer_LockButton(GObject * pObj);
	void SetObj(GObject * _pObj){ASSERT(_pObj); pObj=_pObj;};

private slots:
		void SLT_ButtonClicked(bool bChecked);

protected:
	GObject * pObj;
};