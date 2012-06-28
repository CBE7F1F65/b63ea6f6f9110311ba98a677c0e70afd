#include "stdafx.h"
#include "QTUI_StatusBar.h"
#include "ui_QTUI_StatusBar.h"
#include "qmaininterface.h"
#include "MainInterface.h"

#include "GUICoordinate.h"
#include "GObjectPicker.h"

QTUI_StatusBar::QTUI_StatusBar(QWidget *parent) :
    QStatusBar(parent),
    ui(new Ui::QTUI_StatusBar)
{
    QMainInterface::getInstance().SetPStatusBar(this);
    ui->setupUi(this);
    AdjustSize();
}

QTUI_StatusBar::~QTUI_StatusBar()
{
    delete ui;
}

void QTUI_StatusBar::UpdateStatusBar()
{
    HGE * hge = MainInterface::getInstance().hge;
    float fps = hge->Timer_GetFPS(30);

    QString str;
    str.sprintf("  FPS: %f", fps);
    ui->labelFPS->setText(str);

    GUICoordinate * pguic = &GUICoordinate::getInstance();
    str.sprintf("X: %f,    ", pguic->GetCursorX_C());
    ui->labelXPos->setText(str);
    str.sprintf("Y: %f,    ", pguic->GetCursorY_C());
    ui->labelYPos->setText(str);
    str.sprintf("Scale: %f", pguic->GetScale());
    ui->labelScale->setText(str);

    int nPrecision = MainInterface::getInstance().GetPrecisionInt();
    int nuiPrecision = ui->spinPrecision->value();
    if (nuiPrecision != nPrecision)
    {
        ui->spinPrecision->setValue(nPrecision);
    }

    GObjectPicker * pgp = &GObjectPicker::getInstance();
    if (ui->PBSnapGrid->isChecked() != pgp->isSnapToGrid())
    {
        ui->PBSnapGrid->setChecked(pgp->isSnapToGrid());
    }
    if (ui->PBSnapGeometry->isChecked() != pgp->isSnapToGeometry())
    {
        ui->PBSnapGeometry->setChecked(pgp->isSnapToGeometry());
    }
    if (ui->PBSnapContinuity->isChecked() != pgp->isSnapToContinuity())
    {
        ui->PBSnapContinuity->setChecked(pgp->isSnapToContinuity());
    }
    if (ui->PBSnapCoord->isChecked() != pgp->isSnapToCoord())
    {
        ui->PBSnapCoord->setChecked(pgp->isSnapToCoord());
    }

    if (ui->PBShowGrid->isChecked() != pguic->isShowGrid())
    {
        ui->PBShowGrid->setChecked(pguic->isShowGrid());
    }
}

void QTUI_StatusBar::SLT_PrecisionChanged(int value)
{
    MainInterface::getInstance().SetPrecisionInt(value);
}

void QTUI_StatusBar::SLT_PBSnapGridTriggered(bool bVal)
{
    GObjectPicker::getInstance().SetSnapTo(GOPSNAP_GRID, bVal);
}

void QTUI_StatusBar::SLT_PBSnapGeometryTriggered(bool bVal)
{
    GObjectPicker::getInstance().SetSnapTo(GOPSNAP_GEOMETRY, bVal);
}

void QTUI_StatusBar::SLT_PBSnapCoordTriggered(bool bVal)
{
    GObjectPicker::getInstance().SetSnapTo(GOPSNAP_COORD, bVal);
}

void QTUI_StatusBar::SLT_PBSnapContinuityTriggered(bool bVal)
{
    GObjectPicker::getInstance().SetSnapTo(GOPSNAP_CONTINUITY, bVal);
}

void QTUI_StatusBar::SLT_PBShowGridTriggered(bool bVal)
{
    GUICoordinate::getInstance().SetShowGrid(bVal);
}

void QTUI_StatusBar::resizeEvent(QResizeEvent *e)
{
    QStatusBar::resizeEvent(e);
    AdjustSize();
}

void QTUI_StatusBar::AdjustSize()
{
    int wtotal = this->width();
    int wlast = wtotal - 24;
    ui->ContainerWidget->setGeometry(0, 0, wlast, 24);
}
