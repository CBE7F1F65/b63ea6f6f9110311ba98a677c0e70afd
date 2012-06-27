#include "stdafx.h"
#include "qtui_layer_dockablepane.h"
#include "ui_qtui_layer_dockablepane.h"
#include "qmaininterface.h"

QTUI_Layer_DockablePane::QTUI_Layer_DockablePane(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::QTUI_Layer_DockablePane)
{
    QMainInterface::getInstance().SetPLayerDockable(this);
//    ui->setupUi(this);
}

void QTUI_Layer_DockablePane::OnMainFrameSetupUIDone()
{
    ui->setupUi(this);
}

QTUI_Layer_DockablePane::~QTUI_Layer_DockablePane()
{
    delete ui;
}
