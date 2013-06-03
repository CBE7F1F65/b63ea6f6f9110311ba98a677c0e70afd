#include "stdafx.h"
#include "QTDLG_LayerProperty.h"
#include "ui_QTDLG_LayerProperty.h"

QTDLG_LayerProperty::QTDLG_LayerProperty(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QTDLG_LayerProperty)
{
    ui->setupUi(this);
}

QTDLG_LayerProperty::~QTDLG_LayerProperty()
{
    delete ui;
}

void QTDLG_LayerProperty::SetInitalValue(QString strName, LineColorSet ls, bool bVisible, bool bLocked)
{
    ui->LEName->setText(strName);
    ui->LEName->setFocus();
    ui->LEName->setSelection(0, -1);

    QString str;
    str.sprintf("%08x, %08x, %08x, %08x, %08x",
                ls.GetColor(LINECOLOR_NORMAL),
                ls.GetColor(LINECOLOR_ACTIVE),
                ls.GetColor(LINECOLOR_HIGHLIGHT),
                ls.GetColor(LINECOLOR_UISELECT),
                ls.GetColor(LINECOLOR_INDICATING));
    ui->LEColor->setText(str);

    ui->CBVisible->setChecked(bVisible);
    ui->CBLocked->setChecked(bLocked);
}

QString QTDLG_LayerProperty::GetName()
{
    return ui->LEName->text();
}

LineColorSet QTDLG_LayerProperty::GetLineColorSet()
{
    DWORD col[_LINECOLOR_ENDINDEX];
    LineColorSet ls;

    QString str = ui->LEColor->text();
    sscanf_s(str.toUtf8(), "%08x, %08x, %08x, %08x, %08x",
             &col[LINECOLOR_NORMAL],
             &col[LINECOLOR_ACTIVE],
             &col[LINECOLOR_HIGHLIGHT],
             &col[LINECOLOR_UISELECT],
             &col[LINECOLOR_INDICATING]);

    for (int i=0; i<_LINECOLOR_ENDINDEX; i++)
    {
        ls.SetColor(i, col[i]);
    }

    return ls;
}

bool QTDLG_LayerProperty::GetVisible()
{
    return ui->CBVisible->isChecked();
}

bool QTDLG_LayerProperty::GetLocked()
{
    return ui->CBLocked->isChecked();
}
