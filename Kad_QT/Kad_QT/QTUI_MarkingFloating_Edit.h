#ifndef QTUI_MARKINGFLOATING_EDIT_H
#define QTUI_MARKINGFLOATING_EDIT_H

#include <QLineEdit>

class QTUI_MarkingFloating_Edit : public QLineEdit
{
    Q_OBJECT
public:
    explicit QTUI_MarkingFloating_Edit(QWidget *parent = 0);

protected:
	virtual bool eventFilter(QObject *pQObj, QEvent *e);

public:
	void OnTabFocus();
	void DoneEdit(bool bAccept);
	void SetText_External(QString str);

	bool IsEdited(){return bEdited;};

private:
	bool bEdited;
    
signals:
    
public slots:
	void SLT_TextEdited(QString str);
    
};

#endif // QTUI_MARKINGFLOATING_EDIT_H
