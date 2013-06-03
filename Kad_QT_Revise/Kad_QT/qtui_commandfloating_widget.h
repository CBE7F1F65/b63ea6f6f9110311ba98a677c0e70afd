#ifndef QTUI_COMMANDFLOATING_WIDGET_H
#define QTUI_COMMANDFLOATING_WIDGET_H

#include <QWidget>

namespace Ui {
class QTUI_CommandFloating_Widget;
}

class QTUI_CommandFloating_Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit QTUI_CommandFloating_Widget(QWidget *parent = 0);
    ~QTUI_CommandFloating_Widget();

public:
    void OnReceiveTextFromGL(QString str);
    
private:
    Ui::QTUI_CommandFloating_Widget *ui;

};

#endif // QTUI_COMMANDFLOATING_WIDGET_H
