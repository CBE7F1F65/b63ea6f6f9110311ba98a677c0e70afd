#include "stdafx.h"
#include "qtui_glview.h"
#include "MainInterface.h"
#include "qmaininterface.h"

using namespace Qt;

HGE * hge=MainInterface::getInstance().hge;

QTUI_GLView::QTUI_GLView(QWidget *parent)
	: QGLWidget(parent)
{
    QMainInterface::getInstance().SetPGLView(this);

	updatetimer = NULL;
	ClearKeyState();
}

QTUI_GLView::~QTUI_GLView()
{
	if (updatetimer)
	{
		delete updatetimer;
	}
	MainInterface::getInstance().Exit();
}

void QTUI_GLView::initializeGL()
{
    QGLWidget::initializeGL();

	MainInterface::getInstance().OnPreInit();
	MainInterface::getInstance().OnInit(this, this->width(), this->height());
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glEnable(GL_BLEND);
	if (!updatetimer)
	{
		updatetimer = new QTimer(this);
        connect( updatetimer, SIGNAL(timeout()), SLOT(SLT_OnUpdate()) );
        updatetimer->start( 4 );
    }
}

void QTUI_GLView::resizeGL( int w, int h )
{
	MainInterface::getInstance().OnResizeWindow(w, h);
    QGLWidget::resizeGL(w, h);
}

void QTUI_GLView::paintGL()
{
	MainInterface::getInstance().Render();
    QGLWidget::paintGL();
}

void QTUI_GLView::SLT_OnUpdate()
{
	if (hge)
	{
		float fps = hge->Timer_GetFPS(30);
		QString str = QString("%1").arg(fps);
		this->window()->setWindowTitle(str);
	}

	updateGL();

	MainInterface::getInstance().OnUpdateTimer();
	MainInterface::getInstance().hge->System_Log("s");
//	ClearKeyState();
}

void QTUI_GLView::keyPressEvent( QKeyEvent * e )
{
    int ekey = e->key();

    keyState[ekey&0xff] = 1;

    MainInterface * pmain = &MainInterface::getInstance();
    if (e->modifiers() & Qt::ControlModifier)
    {
        if (ekey == Key_Z)
        {
            pmain->OnUnDo();
        }
        else if (ekey == Key_Y)
        {
            pmain->OnReDo();
        }
    }

    if (ekey != Qt::Key_Backspace && ekey != Qt::Key_Delete && ekey != Qt::Key_Space && ekey != Qt::Key_Escape)
    {
        if (!e->text().isEmpty())
        {
            QMainInterface::getInstance().GetPCommandFloatingWidget()->OnReceiveTextFromGL(e->text());
        }
    }
    QGLWidget::keyPressEvent(e);
}

void QTUI_GLView::keyReleaseEvent( QKeyEvent * e )
{
    keyState[e->key()&0xff] = 0;
    QGLWidget::keyReleaseEvent(e);
}

void QTUI_GLView::mouseMoveEvent( QMouseEvent *e )
{
	if (hge)
	{
        hge->Input_SetMousePos(e->x(), e->y());
	}
    QGLWidget::mouseMoveEvent(e);
}

void QTUI_GLView::mousePressEvent( QMouseEvent *e )
{
	setFocus();
	if (hge)
	{
        hge->Input_SetDIMouseKey(e->button());
	}
    QGLWidget::mousePressEvent(e);
}

void QTUI_GLView::mouseReleaseEvent( QMouseEvent *e )
{
	if (hge)
	{
        hge->Input_SetDIMouseKey(e->button(), false);
	}
    QGLWidget::mouseReleaseEvent(e);

}

void QTUI_GLView::wheelEvent( QWheelEvent *e )
{
    QGLWidget::wheelEvent(e);
}

void QTUI_GLView::enterEvent( QEvent *e )
{
    setFocus();
    QGLWidget::enterEvent(e);
}

void QTUI_GLView::leaveEvent( QEvent *e )
{
    QGLWidget::leaveEvent(e);
}

void QTUI_GLView::OnFrame()
{
	if (QApplication::mouseButtons() & Qt::MouseButton::LeftButton)
	{
		hge->Input_SetDIMouseKey(0);
	}
	if (QApplication::mouseButtons() & Qt::MouseButton::RightButton)
	{
		hge->Input_SetDIMouseKey(1);
	}
    if (keyState[Key_Space&0xff])
	{
		hge->Input_SetDIKey(DIK_SPACE);
    }
    if (keyState[Key_Escape&0xff])
    {
        hge->Input_SetDIKey(DIK_ESCAPE);
    }
    if (keyState[Key_Enter&0xff])
    {
        hge->Input_SetDIKey(DIK_RETURN);
    }
}

void QTUI_GLView::ClearKeyState()
{
	ZeroMemory(keyState, sizeof(keyState));
}
