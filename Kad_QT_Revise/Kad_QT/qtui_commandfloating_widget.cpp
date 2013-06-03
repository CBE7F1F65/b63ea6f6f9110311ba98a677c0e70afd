#include "stdafx.h"
#include "qtui_commandfloating_widget.h"
#include "ui_qtui_commandfloating_widget.h"
#include "qmaininterface.h"

QTUI_CommandFloating_Widget::QTUI_CommandFloating_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QTUI_CommandFloating_Widget)
{
    QMainInterface::getInstance().SetPCommandFloatingWidget(this);
    ui->setupUi(this);
    this->hide();
}

QTUI_CommandFloating_Widget::~QTUI_CommandFloating_Widget()
{
    delete ui;
}

void QTUI_CommandFloating_Widget::OnReceiveTextFromGL(QString str)
{
    if (this->isHidden())
    {
        QPoint pt = this->parentWidget()->mapFromGlobal(QCursor::pos());
        pt+=QPoint(15, 15);
        this->move(pt);
        this->show();
        QMainInterface::getInstance().GetPCommandFloatingEdit()->OnAppend(str);
    }
}
