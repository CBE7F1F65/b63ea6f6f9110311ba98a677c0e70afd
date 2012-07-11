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
				DASSERT(true);
				return false;
			}
		}
	}
	lstMarkingFloating.push_back(pWidget);
	return true;
}

QTUI_MarkingFloating_Widget * QMainInterface::GetNextPMarkingWidget( QTUI_MarkingFloating_Widget * pWidget/*=NULL*/ )
{
	if (lstMarkingFloating.empty())
	{
		return NULL;
	}
	if (!pWidget)
	{
		return lstMarkingFloating.front();
	}
	bool bReturnNext = false;
	for (list<QTUI_MarkingFloating_Widget *>::iterator it=lstMarkingFloating.begin(); it!=lstMarkingFloating.end(); ++it)
	{
		if (bReturnNext)
		{
			return *it;
		}
		if (*it == pWidget)
		{
			bReturnNext = true;
		}
	}
	return lstMarkingFloating.front();
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
	DASSERT(true);
	return false;
}