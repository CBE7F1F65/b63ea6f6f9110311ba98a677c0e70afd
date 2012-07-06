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
    virtual void enterEvent(QEvent * e);

public:
    void ShowNodeInfo(GObject * pObj=NULL);
    
private:
    Ui::QTUI_NodeInfoFloating_Widget *ui;

    GObject * pDisplyObj;
};

#endif // QTUI_NODEINFOFLOATING_WIDGET_H
