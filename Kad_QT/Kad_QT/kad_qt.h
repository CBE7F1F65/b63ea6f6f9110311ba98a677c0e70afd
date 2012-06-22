#ifndef KAD_QT_H
#define KAD_QT_H

#include <QtGui/QMainWindow>
#include "ui_kad_qt.h"

class Kad_QT : public QMainWindow
{
	Q_OBJECT

public:
	Kad_QT(QWidget *parent = 0, Qt::WFlags flags = 0);
	~Kad_QT();

protected:
	virtual void closeEvent(QCloseEvent *event);


private slots:
    void on_action_Command_Line_triggered();

private:
	Ui::Kad_QTClass ui;

public:
	static Kad_QT * pMainWindow;
};

#endif // KAD_QT_H
