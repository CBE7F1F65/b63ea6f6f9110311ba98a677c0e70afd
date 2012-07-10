#include "stdafx.h"
#include "QTUI_MarkingFloating_Widget.h"
#include "ui_QTUI_MarkingFloating_Widget.h"

QTUI_MarkingFloating_Widget::QTUI_MarkingFloating_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QTUI_MarkingFloating_Widget)
{
    ui->setupUi(this);
}

QTUI_MarkingFloating_Widget::~QTUI_MarkingFloating_Widget()
{
    delete ui;
}

void QTUI_MarkingFloating_Widget::SetEditString( QString str )
{
	ui->lineEdit->setText(str);
	QFontMetrics fm = ui->lineEdit->fontMetrics();
	this->resize(fm.boundingRect(str).width()+ui->pushButton->width()+16, this->height());
}

void QTUI_MarkingFloating_Widget::MoveTo( float x, float y )
{
	if (isHidden())
	{
		show();
	}
	QWidget * pParentWidget = this->parentWidget();
	QPoint pt = pParentWidget->mapFromGlobal(QCursor::pos());
	if (fabsf(pt.x()-x) < 15)
	{
		x = pt.x()+15;
	}
	if (fabsf(pt.y()-y)<15)
	{
		y = pt.y()+15;
	}
	if (x < 0)
	{
		x = 0;
	}
	if (y < 0)
	{
		y = 0;
	}
	if (x+this->width() > pParentWidget->width())
	{
		x = pParentWidget->width()-this->width();
	}
	if (y+this->height() > pParentWidget->height())
	{
		y = pParentWidget->height()-this->height();
	}
	this->move(x, y);
}