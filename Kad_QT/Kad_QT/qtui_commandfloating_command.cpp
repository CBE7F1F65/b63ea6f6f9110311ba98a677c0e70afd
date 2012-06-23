#include "qtui_commandfloating_command.h"
#include "qmaininterface.h"
#include "qtui_commandfloating_widget.h"
#include "MainInterface.h"

QTUI_CommandFloating_Command::QTUI_CommandFloating_Command(QWidget *parent) :
    QLineEdit(parent)
{
    QMainInterface::getInstance().SetPCommandFloatingEdit(this);
}

void QTUI_CommandFloating_Command::OnAppend(QString str)
{
    this->insert(str);
    this->setFocus();
}

void QTUI_CommandFloating_Command::focusOutEvent(QFocusEvent *e)
{
    DoneEdit();
    QLineEdit::focusOutEvent(e);
}

void QTUI_CommandFloating_Command::keyPressEvent(QKeyEvent *e)
{
    int ekey = e->key();
    if (ekey == Qt::Key_Return || ekey == Qt::Key_Enter)
    {
        MainInterface::getInstance().OnCommitCommand(this->text().toStdString().c_str());
        DoneEdit();
    }
    else if (ekey == Qt::Key_Escape)
    {
        DoneEdit();
    }
    else
    {
        QLineEdit::keyPressEvent(e);
    }
}

void QTUI_CommandFloating_Command::DoneEdit()
{
    this->clear();
    QMainInterface::getInstance().GetPCommandFloatingWidget()->hide();
    QMainInterface::getInstance().GetPGLView()->setFocus();
}
