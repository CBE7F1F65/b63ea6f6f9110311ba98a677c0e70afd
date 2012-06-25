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
