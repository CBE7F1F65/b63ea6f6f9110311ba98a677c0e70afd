#ifndef QTUI_MARKINGFLOATING_WIDGET_H
#define QTUI_MARKINGFLOATING_WIDGET_H

#include <QWidget>

class MarkingUI;

namespace Ui {
class QTUI_MarkingFloating_Widget;
}

class QTUI_MarkingFloating_Widget : public QWidget
{
    Q_OBJECT
    
public:
	explicit QTUI_MarkingFloating_Widget(QWidget *parent = 0);
	QTUI_MarkingFloating_Widget(MarkingUI * p, QWidget *parent = 0);
    ~QTUI_MarkingFloating_Widget();

	void SetEditString( QString str );
	void MoveTo(float x, float y);
	void OnTabFocus();
	void OnChangeTabFocus();
	void OnDoneEdit( bool bAccept);
	void DoCallback( bool bAccept );
	bool isEditable();
	void OnSetEditable( bool bEditable );
	void LockValue();
	void UnlockValue();
	bool IsValueLocked();

	void SetBGColor(DWORD col);

private:
    Ui::QTUI_MarkingFloating_Widget *ui;
	MarkingUI * pMarkingUI;
	QColor bgcol;

public slots:
	void SLT_LockButtonClicked(bool bChecked);
};

#endif // QTUI_MARKINGFLOATING_WIDGET_H
