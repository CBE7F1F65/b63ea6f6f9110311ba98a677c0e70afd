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
	
private slots:
	void SLT_ButtonClicked(bool bChecked);

protected:
	GObject * pObj;
};

#endif