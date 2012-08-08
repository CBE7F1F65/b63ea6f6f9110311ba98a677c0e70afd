#include "stdafx.h"
#include "QTUI_Layer_Buttons.h"
#include "IconManager.h"
#include "MarqueeSelect.h"

#include "qmaininterface.h"

#define _UILTB_ICONSIZE  ICMSIZE_SMALL

/************************************************************************/
/* Selection                                                            */
/************************************************************************/

QTUI_Layer_SelectionButton::QTUI_Layer_SelectionButton( GObject * _pObj )
{
	ASSERT(_pObj);
	pObj = _pObj;

	this->setMaximumWidth(_UILTB_ICONSIZE);
	this->setMinimumWidth(_UILTB_ICONSIZE);
	this->setMaximumHeight(_UILTB_ICONSIZE);
	this->setMinimumHeight(_UILTB_ICONSIZE);

	this->setCheckable(true);

	connect(this, SIGNAL(clicked(bool)), this, SLOT(SLT_ButtonClicked(bool)));
}

void QTUI_Layer_SelectionButton::SLT_ButtonClicked( bool bChecked )
{
	if (bChecked)
	{
		MarqueeSelect::getInstance().AddSelect(pObj);
		QMainInterface::getInstance().GetPLayerTree()->OnButtonSelect(pObj, true);
	}
	else
	{
		MarqueeSelect::getInstance().DeSelect(pObj);
		QMainInterface::getInstance().GetPLayerTree()->OnButtonSelect(pObj, false);
	}
}

void QTUI_Layer_SelectionButton::UpdateGUISelection( GObject * _pObj, bool bChecked )
{
	if (pObj == _pObj)
	{
		setChecked(bChecked);
	}
}