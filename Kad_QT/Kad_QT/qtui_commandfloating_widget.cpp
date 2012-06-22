#include "qtui_commandfloating_widget.h"
#include "ui_qtui_commandfloating_widget.h"

QTUI_CommandFloating_Widget::QTUI_CommandFloating_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QTUI_CommandFloating_Widget)
{
    ui->setupUi(this);
}

QTUI_CommandFloating_Widget::~QTUI_CommandFloating_Widget()
{
    delete ui;
}
