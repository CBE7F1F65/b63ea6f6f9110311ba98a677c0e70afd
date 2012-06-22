#ifndef QTUI_COMMANDDOCKABLE_H
#define QTUI_COMMANDDOCKABLE_H

#include <QWidget>

namespace Ui {
class QTUI_CommandDockable;
}

class QTUI_CommandDockable : public QWidget
{
    Q_OBJECT
    
public:
    explicit QTUI_CommandDockable(QWidget *parent = 0);
    ~QTUI_CommandDockable();
    
public slots:

signals:

private:
    Ui::QTUI_CommandDockable *ui;
};

#endif // QTUI_COMMANDDOCKABLE_H
