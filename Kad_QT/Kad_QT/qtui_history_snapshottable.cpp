#include "stdafx.h"
#include "qtui_history_snapshottable.h"
#include "IconManager.h"
#include "SnapshotManager.h"

#define _UIHST_ICONSIZE  ICMSIZE_LARGE

enum{
    UIHST_COLUMN_ICON=0,
    UIHST_COLUMN_WARNINGICON,
    UIHST_COLUMN_NAME,
    _UIHST_COLUMNCOUNT,
};

QTUI_History_SnapshotTable::QTUI_History_SnapshotTable(QWidget *parent) :
    QTableWidget(parent)
{
    this->setColumnCount(_UIHST_COLUMNCOUNT);
    displayindexcount=1;
    AdjustSize();
}

void QTUI_History_SnapshotTable::AdjustSize()
{
    int wtotal = this->width();
    this->setColumnWidth(UIHST_COLUMN_ICON, _UIHST_ICONSIZE);
    this->setColumnWidth(UIHST_COLUMN_WARNINGICON, _UIHST_ICONSIZE);

    int wlast = wtotal - _UIHST_ICONSIZE*2 - (_UIHST_COLUMNCOUNT-1);
    if (wlast > 0)
    {
        this->setColumnWidth(UIHST_COLUMN_NAME, wlast);
    }

    this->resizeRowsToContents();
}

void QTUI_History_SnapshotTable::resizeEvent(QResizeEvent *e)
{
    QTableWidget::resizeEvent(e);
    AdjustSize();
}

void QTUI_History_SnapshotTable::AddSnapshot()
{
    int nSnapshot = SnapshotManager::getInstance().AddSnapshot();
    int nRows = this->rowCount();
    Q_ASSERT(nRows == nSnapshot);
    this->setRowCount(nRows+1);

    QTableWidgetItem * pNameItem = new QTableWidgetItem(tr("%1 %2").arg(StringManager::getInstance().GetSnapshotDefaultName()).arg(displayindexcount));
    this->setItem(nRows, UIHST_COLUMN_NAME, pNameItem);
    displayindexcount++;

    this->selectRow(nRows);
    AdjustSize();
}

void QTUI_History_SnapshotTable::DeleteSnapshot()
{
    int nSelected = this->currentRow();
    if (nSelected >= 0)
    {
        SnapshotManager::getInstance().DeleteSnapshot(nSelected);
        this->removeRow(nSelected);
    }
}

void QTUI_History_SnapshotTable::RevertToSnapshot(int nSelected)
{
    SnapshotManager::getInstance().RevertToSnapshot(nSelected);
}
