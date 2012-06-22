#include "stdafx.h"
#include "qtui_commandpanel_commandedit.h"
#include "MainInterface.h"

QTUI_CommandPanel_CommandEdit::QTUI_CommandPanel_CommandEdit(QWidget *parent) :
    QPlainTextEdit(parent)
{
}

void QTUI_CommandPanel_CommandEdit::keyPressEvent(QKeyEvent *e)
{
    QPlainTextEdit::keyPressEvent(e);
    if (e->key() == Qt::Key_Return)
    {
        MainInterface::getInstance().OnCommitCommand(this->toPlainText().toStdString().c_str());
        this->clear();
    }
}
