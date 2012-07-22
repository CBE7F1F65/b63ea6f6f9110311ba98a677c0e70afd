#ifndef QTUI_HISTORY_DOCKABLEPANE_H
#define QTUI_HISTORY_DOCKABLEPANE_H

#include <QDockWidget>

namespace Ui {
class QTUI_History_DockablePane;
}

class QTUI_History_DockablePane : public QDockWidget
{
    Q_OBJECT
    
public:
    explicit QTUI_History_DockablePane(QWidget *parent = 0);
    ~QTUI_History_DockablePane();

protected:
	virtual void enterEvent(QEvent * e);
	virtual void leaveEvent(QEvent * e);

public:
    void OnMainFrameSetupUIDone();
    
private:
    Ui::QTUI_History_DockablePane *ui;
};

#endif // QTUI_HISTORY_DOCKABLEPANE_H
