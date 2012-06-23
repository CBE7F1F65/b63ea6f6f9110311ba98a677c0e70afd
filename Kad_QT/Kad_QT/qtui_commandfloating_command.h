#ifndef QTUI_COMMANDFLOATING_COMMAND_H
#define QTUI_COMMANDFLOATING_COMMAND_H

#include <QLineEdit>

class QTUI_CommandFloating_Command : public QLineEdit
{
    Q_OBJECT
public:
    explicit QTUI_CommandFloating_Command(QWidget *parent = 0);
    
public:
    void OnAppend(QString str);

protected:
    virtual void focusOutEvent(QFocusEvent * e);
    virtual void keyPressEvent(QKeyEvent *e);

private:
    void DoneEdit();

signals:
    
public slots:
    
};

#endif // QTUI_COMMANDFLOATING_COMMAND_H
