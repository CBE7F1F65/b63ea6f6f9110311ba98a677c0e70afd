#include "stdafx.h"
#include "qtui_history_table.h"
#include "qmaininterface.h"
#include "IconManager.h"
#include "MainInterface.h"

#define _UIHT_ICONSIZE  ICMSIZE_MIDDLE

enum{
    UIHT_COLUMN_ICON=0,
    UIHT_COLUMN_DESC,
    UIHT_COLUMN_COMMAND,

    _UIHT_COLUMNCOUNT,
};

QTUI_History_Table::QTUI_History_Table(QWidget *parent) :
    QTableWidget(parent)
{
    QMainInterface::getInstance().SetPHistoryTable(this);

    nSelect = 0;
    bInternalProcessing = false;
    setColumnCount(_UIHT_COLUMNCOUNT);
    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(SLT_ItemSelectionChanged()));
}

void QTUI_History_Table::AdjustSize()
{
    int wtotal = this->width();
    this->setColumnWidth(UIHT_COLUMN_ICON, _UIHT_ICONSIZE);
    this->resizeColumnToContents(UIHT_COLUMN_DESC);

    int wlast = wtotal - _UIHT_ICONSIZE - this->columnWidth(UIHT_COLUMN_DESC) - (_UIHT_COLUMNCOUNT-1);
    if (wlast > 0)
    {
        this->setColumnWidth(UIHT_COLUMN_COMMAND, wlast);
    }

    this->resizeRowsToContents();
}

void QTUI_History_Table::resizeEvent(QResizeEvent *e)
{
    QTableWidget::resizeEvent(e);
    AdjustSize();
}

void QTUI_History_Table::AddHistory(QString desc, QString commandstr, int command)
{
    bInternalProcessing = true;

    int nRows = this->rowCount();
    this->setRowCount(nRows+1);

    QTableWidgetItem * descitem = new QTableWidgetItem(desc);
    this->setItem(nRows, UIHT_COLUMN_DESC, descitem);
    QTableWidgetItem * commandstritem = new QTableWidgetItem(commandstr);
    this->setItem(nRows, UIHT_COLUMN_COMMAND, commandstritem);
    AdjustSize();
    SetSelect(nRows);

    bInternalProcessing = false;
}

void QTUI_History_Table::ChangeCurrentHistory(int step)
{
    bInternalProcessing = true;

    int nowrow = this->currentRow();
    SetSelect(nowrow+step);

    bInternalProcessing = false;
}

void QTUI_History_Table::ClearLaterHistory(int ndelete)
{
    bInternalProcessing = true;

    int nowrow = this->currentRow();
    int laterrows = rowCount()-nowrow-1;
    if (ndelete > laterrows)
    {
        Q_ASSERT(false);
        ndelete = laterrows;
    }
    for (int i=0; i<ndelete; i++)
    {
        this->removeRow(nowrow+1);
    }

    bInternalProcessing = false;
}

void QTUI_History_Table::ClearPreviousHistory(int ndelete)
{
    bInternalProcessing = true;

    int nowrow = this->currentRow();
    if (ndelete > nowrow)
    {
        Q_ASSERT(false);
        ndelete = nowrow;
    }
    for (int i=0; i<ndelete; i++)
    {
        this->removeRow(nowrow-ndelete);
    }
    SetSelect(nowrow-ndelete);

    bInternalProcessing = false;
}

void QTUI_History_Table::SetSelect(int torow)
{
    bInternalProcessing = true;

    this->selectRow(torow);
    nSelect = this->currentRow();

    int nRows = this->rowCount();

    QTableWidgetItem * pItem;
    for (int i=0; i<nRows; i++)
    {
        for (int j=0; j<_UIHT_COLUMNCOUNT; j++)
        {
            pItem = this->item(i, j);
            if (pItem)
            {
                if (i <= nSelect)
                {
                    pItem->setTextColor(QColor(Qt::black));
                }
                else
                {
                    pItem->setTextColor(QColor(Qt::gray));
                }
            }
        }
    }

    bInternalProcessing = false;
}

void QTUI_History_Table::SLT_ItemSelectionChanged()
{
    if (bInternalProcessing)
    {
        return;
    }

    bInternalProcessing = true;

    int nowrow = this->currentRow();
    if (nowrow != nSelect)
    {
        MainInterface::getInstance().CallUnDoReDo(nowrow-nSelect);
        SetSelect(nowrow);
    }

    bInternalProcessing = false;
}
