#include "stdafx.h"
#include "qtui_history_dockablepane.h"
#include "ui_qtui_history_dockablepane.h"
#include "qmaininterface.h"

QTUI_History_DockablePane::QTUI_History_DockablePane(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::QTUI_History_DockablePane)
{
    QMainInterface::getInstance().SetPHistoryDockable(this);
//    ui->setupUi(this);
}

void QTUI_History_DockablePane::OnMainFrameSetupUIDone()
{
    ui->setupUi(this);
}

QTUI_History_DockablePane::~QTUI_History_DockablePane()
{
    delete ui;
}

void QTUI_History_DockablePane::enterEvent( QEvent * e )
{
	setFocus();
}

void QTUI_History_DockablePane::leaveEvent( QEvent * e )
{
	QMainInterface::getInstance().GetPGLView()->setFocus();
}