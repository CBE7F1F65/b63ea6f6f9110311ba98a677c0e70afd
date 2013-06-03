#ifndef QTUI_COMMANDPANEL_COMMANDLOG_H
#define QTUI_COMMANDPANEL_COMMANDLOG_H

#include <QPlainTextEdit>

class QTUI_CommandPanel_CommandLog : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit QTUI_CommandPanel_CommandLog(QWidget *parent = 0);

    void AppendCommandLogText(QString str, bool bNewLine=true);
    
signals:
    
public slots:
};

#endif // QTUI_COMMANDPANEL_COMMANDLOG_H
