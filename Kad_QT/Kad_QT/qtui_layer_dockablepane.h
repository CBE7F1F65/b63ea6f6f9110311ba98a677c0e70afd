#ifndef QTUI_LAYER_DOCKABLEPANE_H
#define QTUI_LAYER_DOCKABLEPANE_H

#include <QDockWidget>

namespace Ui {
class QTUI_Layer_DockablePane;
}

class QTUI_Layer_DockablePane : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit QTUI_Layer_DockablePane(QWidget *parent = 0);
    ~QTUI_Layer_DockablePane();

protected:
    virtual void enterEvent(QEvent * e);
    virtual void leaveEvent(QEvent * e);

public:
    void OnMainFrameSetupUIDone();

private:
    Ui::QTUI_Layer_DockablePane *ui;
};

#endif // QTUI_LAYER_DOCKABLEPANE_H
