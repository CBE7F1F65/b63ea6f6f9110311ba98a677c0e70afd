#include "stdafx.h"
#include "kad_qt.h"
#include <QSettings>
#include "StringManager.h"
#include "MainInterface.h"
#include "qmaininterface.h"
#include "Command.h"

#define _UISCROLL_FAR	1000
#define _UISCROLL_PAGE	100
#define _UISCROLL_NPAGE	10
#define _UISCROLL_LINE	10
#define _UISCROLL_RANGE	1000

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


	QShortcut * undoShortcut = new QShortcut(QKeySequence(tr("Ctrl+Z", "Edit|Undo")), this);
	QShortcut * redoShortcut_1 = new QShortcut(QKeySequence(tr("Ctrl+Shift+Z", "Edit|Redo")), this);
	QShortcut * redoShortcut_2 = new QShortcut(QKeySequence(tr("Ctrl+Y", "Edit|Redo")), this);
	connect(undoShortcut, SIGNAL(activated()), this, SLOT(SLT_UndoShortcutActivated()));
	connect(redoShortcut_1, SIGNAL(activated()), this, SLOT(SLT_RedoShortcutActivated()));
	connect(redoShortcut_2, SIGNAL(activated()), this, SLOT(SLT_RedoShortcutActivated()));

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

void Kad_QT::SLT_UndoShortcutActivated()
{
	Command::getInstance().CreateUnDoCommandCommit();
}

void Kad_QT::SLT_RedoShortcutActivated()
{
	Command::getInstance().CreateReDoCommandCommit();
}