#include "stdafx.h"
#include "QTUI_MarkingFloating_Edit.h"
#include "qmaininterface.h"

QTUI_MarkingFloating_Edit::QTUI_MarkingFloating_Edit(QWidget *parent) :
    QLineEdit(parent)
{
	this->installEventFilter(this);
}

bool QTUI_MarkingFloating_Edit::eventFilter( QObject *pQObj, QEvent *e )
{
	if (e->type() == QEvent::KeyPress)
	{
		QKeyEvent * pke = static_cast<QKeyEvent *>(e);
		if (pke->key() == Qt::Key_Tab)
		{
			OnChangeTabFocus();
			return true;
		}
	}
	return QLineEdit::eventFilter(pQObj, e);
}

void QTUI_MarkingFloating_Edit::OnTabFocus()
{
	this->setFocus();
	this->selectAll();
}

void QTUI_MarkingFloating_Edit::OnChangeTabFocus()
{
	((QTUI_MarkingFloating_Widget *)parentWidget())->OnChangeTabFocus();
}