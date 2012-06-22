/********************************************************************************
** Form generated from reading UI file 'qtui_commanddockable.ui'
**
** Created: Thu Jun 21 22:32:17 2012
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTUI_COMMANDDOCKABLE_H
#define UI_QTUI_COMMANDDOCKABLE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QSplitter>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <qtui_commandpanel_commandedit.h>
#include <qtui_commandpanel_commandlog.h>

QT_BEGIN_NAMESPACE

class Ui_QTUI_CommandDockable
{
public:
    QVBoxLayout *verticalLayout;
    QSplitter *splitter;
    QTUI_CommandPanel_CommandLog *CommandPanel_CommandLog;
    QTUI_CommandPanel_CommandEdit *plainTextEdit;

    void setupUi(QWidget *QTUI_CommandDockable)
    {
        if (QTUI_CommandDockable->objectName().isEmpty())
            QTUI_CommandDockable->setObjectName(QString::fromUtf8("QTUI_CommandDockable"));
        QTUI_CommandDockable->resize(888, 131);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(QTUI_CommandDockable->sizePolicy().hasHeightForWidth());
        QTUI_CommandDockable->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(QTUI_CommandDockable);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        splitter = new QSplitter(QTUI_CommandDockable);
        splitter->setObjectName(QString::fromUtf8("splitter"));
        splitter->setOrientation(Qt::Vertical);
        CommandPanel_CommandLog = new QTUI_CommandPanel_CommandLog(splitter);
        CommandPanel_CommandLog->setObjectName(QString::fromUtf8("CommandPanel_CommandLog"));
        QFont font;
        font.setPointSize(11);
        CommandPanel_CommandLog->setFont(font);
        CommandPanel_CommandLog->viewport()->setProperty("cursor", QVariant(QCursor(Qt::IBeamCursor)));
        CommandPanel_CommandLog->setMouseTracking(true);
        CommandPanel_CommandLog->setReadOnly(true);
        CommandPanel_CommandLog->setTextInteractionFlags(Qt::TextSelectableByKeyboard|Qt::TextSelectableByMouse);
        splitter->addWidget(CommandPanel_CommandLog);
        plainTextEdit = new QTUI_CommandPanel_CommandEdit(splitter);
        plainTextEdit->setObjectName(QString::fromUtf8("plainTextEdit"));
        plainTextEdit->setMinimumSize(QSize(0, 30));
        plainTextEdit->setFont(font);
        plainTextEdit->viewport()->setProperty("cursor", QVariant(QCursor(Qt::IBeamCursor)));
        plainTextEdit->setMouseTracking(true);
        splitter->addWidget(plainTextEdit);

        verticalLayout->addWidget(splitter);


        retranslateUi(QTUI_CommandDockable);

        QMetaObject::connectSlotsByName(QTUI_CommandDockable);
    } // setupUi

    void retranslateUi(QWidget *QTUI_CommandDockable)
    {
        QTUI_CommandDockable->setWindowTitle(QApplication::translate("QTUI_CommandDockable", "Form", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class QTUI_CommandDockable: public Ui_QTUI_CommandDockable {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTUI_COMMANDDOCKABLE_H
