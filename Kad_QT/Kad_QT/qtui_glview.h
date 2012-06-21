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

signals:
    
public slots:
	void SIG_OnUpdate();
    
private:
	QTimer * updatetimer;
};

#endif // QTUI_GLVIEW_H
