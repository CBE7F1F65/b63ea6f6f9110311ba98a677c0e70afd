#include "stdafx.h"
#include "qtui_commandfloating_command.h"
#include "qmaininterface.h"
#include "qtui_commandfloating_widget.h"
#include "MainInterface.h"

QTUI_CommandFloating_Command::QTUI_CommandFloating_Command(QWidget *parent) :
    QLineEdit(parent)
{
	lastTime = QTime::currentTime();
	lastc = -1;
	QMainInterface::getInstance().SetPCommandFloatingEdit(this);
}

void QTUI_CommandFloating_Command::OnAppend(QString str)
{
    this->insert(str);
	lastTime = QTime::currentTime();
	lastc = lastc = str.at(str.size()-1).toLower();
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
		return;
    }
    if (ekey == Qt::Key_Escape)
    {
        DoneEdit();
		return;
	}
	QString nowText = e->text();
	if (nowText.size())
	{
		QChar nowc = nowText.at(nowText.size()-1);
		nowc = nowc.toLower();
		QTime nowTime = QTime::currentTime();
		int timeDiff = lastTime.msecsTo(nowTime);
		lastTime = nowTime;

		if (nowc.isLetter())
		{
			if (nowc == lastc)
			{
				if (timeDiff < QApplication::doubleClickInterval())
				{
					MainInterface::getInstance().OnCommitCommand(this->text().toStdString().c_str());
					DoneEdit();
					return;
				}
			}
		}
		lastc = nowc;
	}

	QLineEdit::keyPressEvent(e);
}

void QTUI_CommandFloating_Command::DoneEdit()
{
    this->clear();
    QMainInterface::getInstance().GetPCommandFloatingWidget()->hide();
    QMainInterface::getInstance().GetPGLView()->setFocus();
}
