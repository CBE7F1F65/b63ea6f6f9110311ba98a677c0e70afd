#include "stdafx.h"
#include "qtui_commanddockable.h"
#include "ui_qtui_commanddockable.h"

QTUI_CommandDockable::QTUI_CommandDockable(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QTUI_CommandDockable)
{
    ui->setupUi(this);
}

QTUI_CommandDockable::~QTUI_CommandDockable()
{
    delete ui;
}
