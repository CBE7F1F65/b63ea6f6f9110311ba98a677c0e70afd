#ifndef QTUI_STATUSBAR_H
#define QTUI_STATUSBAR_H

#include <QStatusBar>

namespace Ui {
class QTUI_StatusBar;
}

class QTUI_StatusBar : public QStatusBar
{
    Q_OBJECT
    
public:
    explicit QTUI_StatusBar(QWidget *parent = 0);
    ~QTUI_StatusBar();

public:
    void UpdateStatusBar();

public slots:
    void SLT_PrecisionChanged(int value);
    void SLT_SnapRangeChanged(int value);

    void SLT_PBSnapGridTriggered(bool bVal);
    void SLT_PBSnapGeometryTriggered(bool bVal);
    void SLT_PBSnapCoordTriggered(bool bVal);
    void SLT_PBSnapContinuityTriggered(bool bVal);
    void SLT_PBShowGridTriggered(bool bVal);
    void SLT_PBShowActualGridTriggered(bool bVal);
    void SLT_PBSnapGeometryCoordTriggered(bool bVal);
    void SLT_PBSnapVirtualCoordTriggered(bool bVal);
    void SLT_PBSnapHandleOnlyTriggered(bool bVal);
    void SLT_PBIsolateModeTriggered(bool bVal);
    void SLT_PBPreviewPrintModeTriggered(bool bVal);
    
protected:
    virtual void resizeEvent(QResizeEvent *e);

private:
    void AdjustSize();

private:
    Ui::QTUI_StatusBar *ui;
};

#endif // QTUI_STATUSBAR_H
