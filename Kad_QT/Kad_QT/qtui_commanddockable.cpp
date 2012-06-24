#include "stdafx.h"
#include "qtui_commanddockable.h"
#include "ui_qtui_commanddockable.h"
#include "qmaininterface.h"

QTUI_CommandDockable::QTUI_CommandDockable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QTUI_CommandDockable)
{
    QMainInterface::getInstance().SetPCommandDockable(this);
    ui->setupUi(this);
}

QTUI_CommandDockable::~QTUI_CommandDockable()
{
    delete ui;
}
