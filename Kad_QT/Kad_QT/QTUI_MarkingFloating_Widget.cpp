#include "stdafx.h"
#include "QTUI_MarkingFloating_Widget.h"
#include "ui_QTUI_MarkingFloating_Widget.h"
#include "qmaininterface.h"
#include "MarkingObject.h"

QTUI_MarkingFloating_Widget::QTUI_MarkingFloating_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QTUI_MarkingFloating_Widget)
{
	ui->setupUi(this);
	ASSERT(false);
}

QTUI_MarkingFloating_Widget::QTUI_MarkingFloating_Widget(MarkingUI * p, QWidget *parent) :
	QWidget(parent),
	ui(new Ui::QTUI_MarkingFloating_Widget)
{
	ui->setupUi(this);
	ui->pushButton->setCursor(Qt::ArrowCursor);
	pMarkingUI = p;
	SetBGColor(p->getBGColor());
	QMainInterface::getInstance().RegisterPMarkingWidget(this);		// Editable?
}

QTUI_MarkingFloating_Widget::~QTUI_MarkingFloating_Widget()
{
	QMainInterface::getInstance().UnregisterPMarkingWidget(this);
    delete ui;
}

void QTUI_MarkingFloating_Widget::SetEditString( QString str )
{
	ui->lineEdit->SetText_External(str);
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

	int w = this->width();
	int h = this->height();
	x -= w/2;
	y -= h/2;

	if (fabsf(pt.y()-y)<h*1.5f)
	{
		y = pt.y()+h*1.5f;
	}
	if (x < 0)
	{
		x = 0;
	}
	if (y < 0)
	{
		y = 0;
	}
	if (x+w > pParentWidget->width())
	{
		x = pParentWidget->width()-w;
	}
	if (y+h > pParentWidget->height())
	{
		y = pParentWidget->height()-h;
	}

	this->move(x, y);
}

void QTUI_MarkingFloating_Widget::OnTabFocus()
{
	ui->lineEdit->OnTabFocus();
}

void QTUI_MarkingFloating_Widget::OnChangeTabFocus()
{
	QMainInterface::getInstance().GetNextPMarkingWidget(this)->OnTabFocus();
}

void QTUI_MarkingFloating_Widget::OnDoneEdit( bool bAccept )
{
	QString str = ui->lineEdit->text();
	if (ui->lineEdit->IsEdited())
	{
		pMarkingUI->OnDoneEdit(ui->lineEdit->text());
		if (!str.length())
		{
			UnlockValue();
		}
		else
		{
			LockValue();
		}
		DoCallback(bAccept);
	}
	if (!bAccept)
	{
		OnChangeTabFocus();
	}
}

void QTUI_MarkingFloating_Widget::DoCallback( bool bAccept )
{
	if (!pMarkingUI->DoCallback(bAccept))
	{
		UnlockValue();
	}
}

bool QTUI_MarkingFloating_Widget::isEditable()
{
	return pMarkingUI->isEditable();
}

void QTUI_MarkingFloating_Widget::OnSetEditable( bool bEditable )
{
	ui->lineEdit->SetReadOnly(!bEditable);
}

void QTUI_MarkingFloating_Widget::LockValue()
{
	ui->pushButton->setChecked(true);
}

void QTUI_MarkingFloating_Widget::UnlockValue()
{
	ui->pushButton->setChecked(false);
}

bool QTUI_MarkingFloating_Widget::IsValueLocked()
{
	return ui->pushButton->isChecked();
}

void QTUI_MarkingFloating_Widget::SLT_LockButtonClicked( bool bChecked )
{
	if (bChecked)
	{
		LockValue();
	}
	else
	{
		UnlockValue();
	}
}

void QTUI_MarkingFloating_Widget::SetBGColor( DWORD col )
{
	bgcol.setRgba(col);
	QPalette pal = palette();
	pal.setColor(backgroundRole(), bgcol);
	setPalette(pal);
}