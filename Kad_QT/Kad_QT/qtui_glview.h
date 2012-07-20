#ifndef QTUI_GLVIEW_H
#define QTUI_GLVIEW_H

#include <QGLWidget>

class QTUI_GLView : public QGLWidget
{
    Q_OBJECT
public:
    explicit QTUI_GLView(QWidget *parent = 0);
	~QTUI_GLView();

	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();

protected:
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void wheelEvent(QWheelEvent *e);
    virtual void enterEvent(QEvent *e);
    virtual void leaveEvent(QEvent *e);
	virtual bool eventFilter(QObject *target, QEvent *e);
	virtual void focusInEvent ( QFocusEvent * e );
	virtual void focusOutEvent ( QFocusEvent * e );

signals:
    void SIG_OnChar(QString);
    
public slots:
    void SLT_OnUpdate();
    void SLT_HScrollValueChanged(int val);
    void SLT_VScrollValueChanged(int val);

public:
	void OnFrame();
	void OnFrameEnd();

    void OnMainFrameSetupUIDone();

private:
	QTimer * updatetimer;
};

#endif // QTUI_GLVIEW_H
