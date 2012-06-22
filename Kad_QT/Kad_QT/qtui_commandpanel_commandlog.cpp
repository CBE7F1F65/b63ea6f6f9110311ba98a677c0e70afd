#include "stdafx.h"
#include "qtui_commandpanel_commandlog.h"

QTUI_CommandPanel_CommandLog::QTUI_CommandPanel_CommandLog(QWidget *parent) :
    QPlainTextEdit(parent)
{
}

void QTUI_CommandPanel_CommandLog::AppendCommandLogText(QString str, bool bNewLine)
{
    if (bNewLine)
	{
		this->appendPlainText(str);
    }
	else
	{
		this->insertPlainText(str);
	}
}
