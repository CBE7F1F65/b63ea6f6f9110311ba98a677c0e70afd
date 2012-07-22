#include "stdafx.h"
#include "qtui_layer_dockablepane.h"
#include "ui_qtui_layer_dockablepane.h"
#include "qmaininterface.h"
#include "GObjectManager.h"

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

void QTUI_Layer_DockablePane::enterEvent(QEvent *e)
{
    GObjectManager::getInstance().SetRenderUILayerIndicators(true);
	setFocus();
}

void QTUI_Layer_DockablePane::leaveEvent(QEvent *e)
{
	GObjectManager::getInstance().SetRenderUILayerIndicators(false);
	QMainInterface::getInstance().GetPGLView()->setFocus();
}