#include "qmaininterface.h"

QMainInterface::QMainInterface()
{
    pMainWindow = NULL;
    pGLView = NULL;
    pCommandLog = NULL;
    pCommandEdit = NULL;
    pCommandFloatingWidget = NULL;
    pCommandFloatingEdit = NULL;
}

QMainInterface::~QMainInterface()
{
}
