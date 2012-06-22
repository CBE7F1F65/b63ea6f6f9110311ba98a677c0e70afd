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
    virtual void keyPressEvent(QKeyEvent * e);
    virtual void keyReleaseEvent(QKeyEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void wheelEvent(QWheelEvent *e);
    virtual void enterEvent(QEvent *e);
    virtual void leaveEvent(QEvent *e);

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
