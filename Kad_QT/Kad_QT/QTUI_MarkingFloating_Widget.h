#ifndef QTUI_MARKINGFLOATING_WIDGET_H
#define QTUI_MARKINGFLOATING_WIDGET_H

#include <QWidget>

namespace Ui {
class QTUI_MarkingFloating_Widget;
}

class QTUI_MarkingFloating_Widget : public QWidget
{
    Q_OBJECT
    
public:
    explicit QTUI_MarkingFloating_Widget(QWidget *parent = 0);
    ~QTUI_MarkingFloating_Widget();

	void SetEditString( QString str );
	void MoveTo(float x, float y);
	void OnTabFocus();
	void OnChangeTabFocus();
	
private:
    Ui::QTUI_MarkingFloating_Widget *ui;
};

#endif // QTUI_MARKINGFLOATING_WIDGET_H
