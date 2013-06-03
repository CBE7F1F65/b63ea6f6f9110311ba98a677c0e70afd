#include "stdafx.h"
#include "qtui_commandpanel_commandedit.h"
#include "MainInterface.h"
#include "qmaininterface.h"

QTUI_CommandPanel_CommandEdit::QTUI_CommandPanel_CommandEdit(QWidget *parent) :
    QPlainTextEdit(parent)
{
    QMainInterface::getInstance().SetPCommandEdit(this);
}

void QTUI_CommandPanel_CommandEdit::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Tab)
	{
//		this->insertPlainText("\t");
	}
	else
	{
		QPlainTextEdit::keyPressEvent(e);
	}
    if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter)
    {
		QString str = this->toPlainText();
//		str.replace('\t', ' ');
        MainInterface::getInstance().OnCommitCommand(this->toPlainText().toStdString().c_str());
        this->clear();
    }
}