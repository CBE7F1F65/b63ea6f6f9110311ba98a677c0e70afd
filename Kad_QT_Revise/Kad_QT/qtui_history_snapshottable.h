#ifndef QTUI_HISTORY_SNAPSHOTTABLE_H
#define QTUI_HISTORY_SNAPSHOTTABLE_H

#include <QTableWidget>

class QTUI_History_SnapshotTable : public QTableWidget
{
    Q_OBJECT
public:
    explicit QTUI_History_SnapshotTable(QWidget *parent = 0);

private:
    void AdjustSize();

protected:
    virtual void resizeEvent(QResizeEvent * e);

private:
    int displayindexcount;
    
signals:
    
public slots:
    void AddSnapshot();
    void DeleteSnapshot();
    void RevertToSnapshot(int nSelected);
};

#endif // QTUI_HISTORY_SNAPSHOTTABLE_H
