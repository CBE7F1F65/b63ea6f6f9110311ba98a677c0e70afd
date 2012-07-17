#include "stdafx.h"
#include "QTUI_MarkingFloating_Edit.h"
#include "qmaininterface.h"

QTUI_MarkingFloating_Edit::QTUI_MarkingFloating_Edit(QWidget *parent) :
    QLineEdit(parent)
{
	this->installEventFilter(this);
	bEdited = false;
}

bool QTUI_MarkingFloating_Edit::eventFilter( QObject *pQObj, QEvent *e )
{
	if (e->type() == QEvent::KeyPress)
	{
		QKeyEvent * pke = static_cast<QKeyEvent *>(e);
		if (pke->key() == Qt::Key_Tab)
		{
			DoneEdit(false);
			return true;
		}
		if (pke->key() == Qt::Key_Return || pke->key() == Qt::Key_Enter)
		{
			DoneEdit(true);
			QMainInterface::getInstance().GetPGLView()->setFocus();
			return true;
		}
		if (pke->key() == Qt::Key_Escape)
		{
			QMainInterface::getInstance().GetPGLView()->setFocus();
		}
	}
	return QLineEdit::eventFilter(pQObj, e);
}

void QTUI_MarkingFloating_Edit::OnTabFocus()
{
	this->setFocus();
	this->selectAll();
}

void QTUI_MarkingFloating_Edit::DoneEdit( bool bAccept )
{
	((QTUI_MarkingFloating_Widget *)parentWidget())->OnDoneEdit(bAccept);
	bEdited = false;
}

void QTUI_MarkingFloating_Edit::SetText_External( QString str )
{
	this->setText(str);
	if (this->hasFocus())
	{
		this->selectAll();
	}
	bEdited = false;
}

void QTUI_MarkingFloating_Edit::SLT_TextEdited( QString str )
{
	bEdited = true;
}