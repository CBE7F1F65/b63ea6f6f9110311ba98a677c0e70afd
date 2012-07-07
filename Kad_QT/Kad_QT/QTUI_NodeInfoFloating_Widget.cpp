#include "stdafx.h"
#include "QTUI_NodeInfoFloating_Widget.h"
#include "ui_QTUI_NodeInfoFloating_Widget.h"
#include "qmaininterface.h"

QTUI_NodeInfoFloating_Widget::QTUI_NodeInfoFloating_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QTUI_NodeInfoFloating_Widget)
{
    QMainInterface::getInstance().SetPNodeInfoFloatingWidget(this);
    ui->setupUi(this);

    this->setCursor(Qt::ArrowCursor);

    this->hide();
}

QTUI_NodeInfoFloating_Widget::~QTUI_NodeInfoFloating_Widget()
{
    delete ui;
}

void QTUI_NodeInfoFloating_Widget::ShowWidgetWindow(bool bContext)
{
    QPoint pt = parentWidget()->mapFromGlobal(QCursor::pos());
    if (!bContext)
    {
        pt+=QPoint(15, -15);
    }
    else
    {
        pt+=QPoint(-10, -10);
    }
    move(pt);
    show();
}

void QTUI_NodeInfoFloating_Widget::HideWidgetWindow()
{
    hide();
}

bool QTUI_NodeInfoFloating_Widget::isWidgetWindowHidden()
{
    return isHidden();
}

void QTUI_NodeInfoFloating_Widget::AdjustSize(int w, int h)
{
    if (w >= 0 && h >= 0)
    {
        this->resize(w, h);
    }
    else
    {
        adjustSize();
    }
}
