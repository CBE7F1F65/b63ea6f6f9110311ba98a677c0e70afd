#include "stdafx.h"
#include "qmaininterface.h"

QMainInterface::QMainInterface()
{
    pMainWindow = NULL;
    pGLView = NULL;
    pCommandLog = NULL;
    pCommandEdit = NULL;
    pCommandDockable = NULL;
    pCommandFloatingWidget = NULL;
    pCommandFloatingEdit = NULL;
    pHistoryDockable = NULL;
    pHistoryTable = NULL;
    pLayerDockable = NULL;
    pLayerTree = NULL;
    pStatusBar = NULL;
    pNodeInfoFloatingWidget = NULL;
    pNodeInfoFloatingTree = NULL;
}

QMainInterface::~QMainInterface()
{
}

void QMainInterface::OnMainFrameSetupUIDone()
{
    pHistoryDockable->OnMainFrameSetupUIDone();
    pLayerDockable->OnMainFrameSetupUIDone();
    pGLView->OnMainFrameSetupUIDone();
}

bool QMainInterface::RegisterPMarkingWidget( QTUI_MarkingFloating_Widget * pWidget )
{
	if (!lstMarkingFloating.empty())
	{
		for (list<QTUI_MarkingFloating_Widget *>::iterator it=lstMarkingFloating.begin(); it!=lstMarkingFloating.end(); ++it)
		{
			if (*it == pWidget)
			{
				DASSERT(false);
				return false;
			}
		}
	}
	lstMarkingFloating.push_back(pWidget);
	return true;
}

QTUI_MarkingFloating_Widget * QMainInterface::GetNextPMarkingWidget( QTUI_MarkingFloating_Widget * pWidget/*=NULL*/, bool bEditableOnly/*=true*/ )
{
	if (lstMarkingFloating.empty())
	{
		return NULL;
	}
	bool bReturnNext = false;
	if (!pWidget)
	{
		bReturnNext = true;
	}
	QTUI_MarkingFloating_Widget * pFirst = NULL;
	for (list<QTUI_MarkingFloating_Widget *>::iterator it=lstMarkingFloating.begin(); it!=lstMarkingFloating.end(); ++it)
	{
		QTUI_MarkingFloating_Widget * pIt = *it;
		if (bReturnNext)
		{
			if (!bEditableOnly || pIt->isEditable())
			{
				return pIt;
			}
		}
		if (pIt == pWidget)
		{
			bReturnNext = true;
		}
		if (!pFirst)
		{
			if (!bEditableOnly || pIt->isEditable())
			{
				pFirst = pIt;
			}
		}
	}
	return pFirst;
}

bool QMainInterface::UnregisterPMarkingWidget( QTUI_MarkingFloating_Widget * pWidget/*=NULL*/ )
{
	if (!pWidget)
	{
	}
	for (list<QTUI_MarkingFloating_Widget *>::iterator it=lstMarkingFloating.begin(); it!=lstMarkingFloating.end(); ++it)
	{
		if (*it == pWidget)
		{
			lstMarkingFloating.erase(it);
			return true;
		}
	}
	DASSERT(false);
	return false;
}