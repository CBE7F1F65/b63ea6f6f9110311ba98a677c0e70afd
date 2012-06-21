#include "stdafx.h"
#include "kad_qt.h"
#include <QSettings>
#include "StringManager.h"

Kad_QT::Kad_QT(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);

	StringManager * psm = &StringManager::getInstance();
	QSettings settings(psm->GetRegistryKeyName(), psm->GetAppIDName());
	restoreGeometry(settings.value(psm->GetRegistryGeometryName()).toByteArray());
	restoreState(settings.value(psm->GetRegistryWindowStateName()).toByteArray());
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