#ifndef KAD_QT_H
#define KAD_QT_H

#include <QMainWindow>
#include "ui_kad_qt.h"

class Kad_QT : public QMainWindow
{
	Q_OBJECT

public:
    Kad_QT(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~Kad_QT();

protected:
    virtual void closeEvent(QCloseEvent *event);


private slots:
    void on_action_Command_Line_triggered();

	void SLT_UndoShortcutActivated();
	void SLT_RedoShortcutActivated();

	void SLT_CopyShortcutActivated();
	void SLT_CopyLayerShortcutActivated();
	void SLT_PasteShortcutActivated();
	void SLT_SelectAllShortcutActivated();
	void SLT_SelectAllThisLayerShortcutActivated();

    void on_action_New_triggered();

    void on_action_Open_triggered();

    void on_action_Save_triggered();

    void on_action_SaveAs_triggered();

    void on_action_Exit_triggered();

private:
	Ui::Kad_QTClass ui;
};

#endif // KAD_QT_H
