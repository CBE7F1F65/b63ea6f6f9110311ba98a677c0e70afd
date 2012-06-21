/********************************************************************************
** Form generated from reading UI file 'kad_qt.ui'
**
** Created: Wed Jun 20 21:43:05 2012
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_KAD_QT_H
#define UI_KAD_QT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>
#include <qtui_glview.h>

QT_BEGIN_NAMESPACE

class Ui_Kad_QTClass
{
public:
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QTUI_GLView *widget;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *Kad_QTClass)
    {
        if (Kad_QTClass->objectName().isEmpty())
            Kad_QTClass->setObjectName(QString::fromUtf8("Kad_QTClass"));
        Kad_QTClass->resize(1024, 768);
        centralWidget = new QWidget(Kad_QTClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        widget = new QTUI_GLView(centralWidget);
        widget->setObjectName(QString::fromUtf8("widget"));

        horizontalLayout->addWidget(widget);

        Kad_QTClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(Kad_QTClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1024, 21));
        Kad_QTClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(Kad_QTClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        Kad_QTClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(Kad_QTClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        Kad_QTClass->setStatusBar(statusBar);

        retranslateUi(Kad_QTClass);

        QMetaObject::connectSlotsByName(Kad_QTClass);
    } // setupUi

    void retranslateUi(QMainWindow *Kad_QTClass)
    {
        Kad_QTClass->setWindowTitle(QApplication::translate("Kad_QTClass", "Kad_QT", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Kad_QTClass: public Ui_Kad_QTClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_KAD_QT_H
