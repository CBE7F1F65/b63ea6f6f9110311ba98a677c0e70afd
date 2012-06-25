#include "stdafx.h"
#include "kad_qt.h"
#include <QSettings>
#include "StringManager.h"
#include "MainInterface.h"
#include "qmaininterface.h"

Kad_QT::Kad_QT(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
    QMainInterface::getInstance().SetPMainWindow(this);

	ui.setupUi(this);

    QMainInterface::getInstance().OnMainFrameSetupUIDone();

    StringManager * psm = &StringManager::getInstance();
    QSettings settings(psm->GetRegistryKeyName(), psm->GetAppIDName());
    restoreGeometry(settings.value(psm->GetRegistryGeometryName()).toByteArray());
    restoreState(settings.value(psm->GetRegistryWindowStateName()).toByteArray());

    this->installEventFilter(this);
}

Kad_QT::~Kad_QT()
{

}

void Kad_QT::closeEvent( QCloseEvent *event )
{
	StringManager * psm = &StringManager::getInstance();
	QSettings settings(psm->GetRegistryKeyName(), psm->GetAppIDName());
	settings.setValue(psm->GetRegistryGeometryName(), saveGeometry());
	settings.setValue(psm->GetRegistryWindowStateName(), saveState());
    QWidget::closeEvent(event);
}

bool Kad_QT::eventFilter(QObject *target, QEvent *e)
{
    return QMainWindow::eventFilter(target, e);
}

void Kad_QT::on_action_Command_Line_triggered()
{
    MainInterface::getInstance().OnCommand(COMM_LINE);
}
