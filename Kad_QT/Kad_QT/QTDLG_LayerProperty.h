#ifndef QTDLG_LAYERPROPERTY_H
#define QTDLG_LAYERPROPERTY_H

#include <QDialog>
#include "ColorManager.h"

namespace Ui {
class QTDLG_LayerProperty;
}

class QTDLG_LayerProperty : public QDialog
{
    Q_OBJECT
    
public:
    explicit QTDLG_LayerProperty(QWidget *parent = 0);
    ~QTDLG_LayerProperty();
    
public:
    void SetInitalValue(QString strName, LineColorSet ls, bool bVisible, bool bLocked);
    QString GetName();
    LineColorSet GetLineColorSet();
    bool GetVisible();
    bool GetLocked();

private:
    Ui::QTDLG_LayerProperty *ui;

    QString strName;
    LineColorSet lsLineColor;
    bool bVisible;
    bool bLocked;
};

#endif // QTDLG_LAYERPROPERTY_H
