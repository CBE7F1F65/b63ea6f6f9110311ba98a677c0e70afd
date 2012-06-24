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
}

QMainInterface::~QMainInterface()
{
}

void QMainInterface::OnMainFrameSetupUIDone()
{
    pHistoryDockable->OnMainFrameSetupUIDone();
}
