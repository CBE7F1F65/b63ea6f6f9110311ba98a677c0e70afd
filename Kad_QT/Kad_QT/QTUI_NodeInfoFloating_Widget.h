#ifndef QTUI_NODEINFOFLOATING_WIDGET_H
#define QTUI_NODEINFOFLOATING_WIDGET_H

#include <QWidget>
#include "GObject.h"

namespace Ui {
class QTUI_NodeInfoFloating_Widget;
}

class QTUI_NodeInfoFloating_Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit QTUI_NodeInfoFloating_Widget(QWidget *parent = 0);
    ~QTUI_NodeInfoFloating_Widget();

protected:
//    virtual void mousePressEvent( QMouseEvent *e );
//    virtual void enterEvent(QEvent * e);

public:
    void ShowWidgetWindow(bool bContext);
    void HideWidgetWindow();
    bool isWidgetWindowHidden();
    void AdjustSize(int w=-1, int h=-1);
    
private:
    Ui::QTUI_NodeInfoFloating_Widget *ui;
};

#endif // QTUI_NODEINFOFLOATING_WIDGET_H
