#ifndef QTUI_HISTORY_TABLE_H
#define QTUI_HISTORY_TABLE_H

#include <QTableWidget>

class QTUI_History_Table : public QTableWidget
{
    Q_OBJECT
public:
    explicit QTUI_History_Table(QWidget *parent = 0);
    
private:
    void AdjustSize();

protected:
    virtual void resizeEvent(QResizeEvent * e);

public:
    void AddHistory(QString desc, QString commandstr, int command);
    void ChangeCurrentHistory(int step);
    void ClearLaterHistory(int ndelete);
    void ClearPreviousHistory(int ndelete);

private:
    int nSelect;
    bool bInternalProcessing;
    void SetSelect(int torow);

signals:

public slots:
    void SLT_ItemSelectionChanged();
};

#endif // QTUI_HISTORY_TABLE_H
