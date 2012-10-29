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

	QShortcut * copyShortcut = new QShortcut(QKeySequence(tr("Ctrl+C", "Select|Copy")), this);
	QShortcut * pasteShortcut = new QShortcut(QKeySequence(tr("Ctrl+V", "Select|Paste")), this);
	QShortcut * selectAllShortcut = new QShortcut(QKeySequence(tr("Ctrl+A", "Select|SelectAll")), this);
	QShortcut * selectAllThisLayerShortcut = new QShortcut(QKeySequence(tr("Ctrl+Shift+A", "Select|SelectAllThisLayer")), this);

	connect(copyShortcut, SIGNAL(activated()), this, SLOT(SLT_CopyShortcutActivated()));
	connect(pasteShortcut, SIGNAL(activated()), this, SLOT(SLT_PasteShortcutActivated()));
	connect(selectAllShortcut, SIGNAL(activated()), this, SLOT(SLT_SelectAllShortcutActivated()));
	connect(selectAllThisLayerShortcut, SIGNAL(activated()), this, SLOT(SLT_SelectAllThisLayerShortcutActivated()));

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

void Kad_QT::on_action_Command_Line_triggered()
{
    MainInterface::getInstance().OnCommand(COMM_LINE);
}

void Kad_QT::SLT_UndoShortcutActivated()
{
	Command::getInstance().CreateUnDoCommandCommit();
	QMainInterface::getInstance().GetPNodeInfoFloatingTree()->ShowNodeInfo();
}

void Kad_QT::SLT_RedoShortcutActivated()
{
	Command::getInstance().CreateReDoCommandCommit();
	QMainInterface::getInstance().GetPNodeInfoFloatingTree()->ShowNodeInfo();
}

void Kad_QT::on_action_New_triggered()
{
	MainInterface::getInstance().OnCommand(COMM_NEW);
}

void Kad_QT::on_action_Open_triggered()
{
	MainInterface::getInstance().OnCommand(COMM_OPEN);
}

void Kad_QT::on_action_Save_triggered()
{
	MainInterface::getInstance().OnCommand(COMM_SAVE);
}

void Kad_QT::on_action_SaveAs_triggered()
{
	MainInterface::getInstance().OnCommand(COMM_SAVEAS);
}

void Kad_QT::on_action_Exit_triggered()
{
	MainInterface::getInstance().OnCommand(COMM_EXIT);
}

void Kad_QT::SLT_CopyShortcutActivated()
{
	MainInterface::getInstance().OnCommand(COMM_COPY);
}

void Kad_QT::SLT_PasteShortcutActivated()
{
	MainInterface::getInstance().OnCommand(COMM_PASTE);
}

void Kad_QT::SLT_SelectAllShortcutActivated()
{
	MainInterface::getInstance().CallSelectAllNodes(false);
}

void Kad_QT::SLT_SelectAllThisLayerShortcutActivated()
{
	MainInterface::getInstance().CallSelectAllNodes(true);
}
