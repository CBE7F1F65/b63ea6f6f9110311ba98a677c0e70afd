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
	virtual void keyPressEvent(QKeyEvent * event);
	virtual void keyReleaseEvent(QKeyEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);
	virtual void wheelEvent(QWheelEvent *event);
	virtual void enterEvent(QEvent *event);
	virtual void leaveEvent(QEvent *event);

signals:
    
public slots:
	void SIG_OnUpdate();
	void OnFrame();

private:
	QTimer * updatetimer;

	BYTE keyState[256];
	void ClearKeyState();
};

#endif // QTUI_GLVIEW_H
