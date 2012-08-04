#include "stdafx.h"
#include "QTUI_NodeInfoFloating_Buttons.h"

#include "MainInterface.h"
#include "IconManager.h"

#include "qmaininterface.h"

#define _UINIFTB_ICONSIZE  ICMSIZE_SMALL

/************************************************************************/
/* Transform                                                            */
/************************************************************************/

QTUI_NodeInfoFloating_TransformButton::QTUI_NodeInfoFloating_TransformButton( GLine * _pLine )
{
	ASSERT(pLine);

	pLine = _pLine;
	this->setCheckable(false);
	this->setMaximumHeight(_UINIFTB_ICONSIZE);
	this->setMinimumHeight(_UINIFTB_ICONSIZE);
	connect(this, SIGNAL(clicked()), this, SLOT(SLT_ButtonClicked()));

	ChangeState(true);
}

void QTUI_NodeInfoFloating_TransformButton::ChangeState( bool bInitial )
{
	bool bNowStraightLine = pLine->isStraightLine();
	StringManager * psm = &StringManager::getInstance();
	if (bNowStraightLine ^ (!bInitial))
	{
		setText(psm->GetNodeInfoToBezierName());
	}
	else
	{
		setText(psm->GetNodeInfoToStrightLineName());
	}
	if (!bInitial)
	{
		MainInterface::getInstance().OnCommandWithParam(
			bNowStraightLine?COMM_TOBEZIER:COMM_TOSTRAIGHTLINE,
			CCCWPARAM_I(pLine->getID()),
			NULL
			);
	}
}

void QTUI_NodeInfoFloating_TransformButton::SLT_ButtonClicked()
{
	ChangeState(false);
}

/************************************************************************/
/* Cling                                                                */
/************************************************************************/

QTUI_NodeInfoFloating_ChangeClingTypeButton::QTUI_NodeInfoFloating_ChangeClingTypeButton( GPoint * pPoint, QTreeWidgetItem * pParentItem, int _nTextColumn, bool bClingTo )
{
	ASSERT(pPoint);
	ASSERT(pParentItem);

	pClingPoint = pPoint;
	pItem = pParentItem;
	bTo = bClingTo;
	nTextColumn = _nTextColumn;

	this->setMaximumWidth(_UINIFTB_ICONSIZE);
	this->setMinimumWidth(_UINIFTB_ICONSIZE);
	this->setMaximumHeight(_UINIFTB_ICONSIZE);
	this->setMinimumHeight(_UINIFTB_ICONSIZE);
	connect(this, SIGNAL(clicked()), this, SLOT(SLT_ButtonClicked()));

	ChangeState(true);
}

void QTUI_NodeInfoFloating_ChangeClingTypeButton::ChangeState( bool bInitial )
{
	GClingInfo cli = *pClingPoint->getClingInfo();
	int nNowClingType = cli.GetClingType();
	int toClingType = nNowClingType;
	switch (nNowClingType)
	{
	case GCLING_PROPORTION:
		toClingType = GCLING_BEGINOFFSET;
		break;
	case GCLING_BEGINOFFSET:
		toClingType = GCLING_ENDOFFSET;
		break;
	case GCLING_ENDOFFSET:
		toClingType = GCLING_PROPORTION;
		break;
	}
	cli.ApplyTypeChange(toClingType);

	StringManager * psm = &StringManager::getInstance();
	this->setToolTip(psm->GetNodeInfoChangeClingTypeName(toClingType));



	if (!bInitial)
	{
		QString str;
		const char * strDisplayName;
		int index = -1;
		if (bTo)
		{
			GLine * pClingLine = cli.GetClingTo();
			index = pClingLine->getID();
			strDisplayName = pClingLine->getDisplayName();
		}
		else
		{
			index = pClingPoint->getID();
			strDisplayName = pClingPoint->getDisplayName();
		}
		str.sprintf("%s: %d, %s: %s (%s: %f)", psm->GetNodeInfoIDName(), index, psm->GetNodeInfoNameName(), strDisplayName, psm->GetNodeInfoClingTypeName(cli.GetClingType()), cli.GetClingVal());
		pItem->setText(nTextColumn, str);

		MainInterface::getInstance().OnCommandWithParam(
			COMM_CLING,
			CCCWPARAM_I(pClingPoint->getID()),
			CCCWPARAM_I(cli.GetClingTo()->getID()),
			CCCWPARAM_I(cli.GetClingType()),
			CCCWPARAM_F(cli.GetClingVal()),
			NULL
			);
	}
}

void QTUI_NodeInfoFloating_ChangeClingTypeButton::SLT_ButtonClicked()
{
	ChangeState(false);
}