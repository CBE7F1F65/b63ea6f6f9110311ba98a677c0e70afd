#include "stdafx.h"
#include "QTUI_NodeInfoFloating_Widget.h"
#include "ui_QTUI_NodeInfoFloating_Widget.h"
#include "qmaininterface.h"

QTUI_NodeInfoFloating_Widget::QTUI_NodeInfoFloating_Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QTUI_NodeInfoFloating_Widget)
{
    QMainInterface::getInstance().SetPNodeInfoFloatingWidget(this);
    ui->setupUi(this);

    this->setCursor(Qt::ArrowCursor);

    this->hide();
}

QTUI_NodeInfoFloating_Widget::~QTUI_NodeInfoFloating_Widget()
{
    delete ui;
}

void QTUI_NodeInfoFloating_Widget::enterEvent(QEvent * e)
{
    ui->treeWidget->setFocus();
}

void QTUI_NodeInfoFloating_Widget::ShowNodeInfo(GObject *pObj)
{
    if ((this->isHidden() || pDisplyObj!=pObj) && pObj)
    {
        pDisplyObj = pObj;

        this->setFocus();
        QPoint pt = this->parentWidget()->mapFromGlobal(QCursor::pos());
        pt+=QPoint(15, -15);
        this->move(pt);
        this->show();

        QString str;
        str.sprintf("ID: %d\r\n%s", pObj->getID(), pObj->getDisplayName());


        ui->treeWidget->clear();
        QTreeWidgetItem * pTreeItem[4];
        for (int i=0; i<4; i++)
        {
            pTreeItem[i] = new QTreeWidgetItem();

            pTreeItem[i]->setText(0, str);
            ui->treeWidget->addTopLevelItem(pTreeItem[i]);
        }
        QPushButton * pPush = new QPushButton("AAA");
        ui->treeWidget->setItemWidget(pTreeItem[2], 0, pPush);

        int htotal = ui->treeWidget->model()->rowCount() * pPush->size().height() + 80;
        this->resize(this->width(), htotal);
    }
    else if (!pObj)
    {
        pDisplyObj = NULL;
        if (!this->isHidden())
        {
            this->hide();
            QMainInterface::getInstance().GetPGLView()->setFocus();
        }
    }
}
