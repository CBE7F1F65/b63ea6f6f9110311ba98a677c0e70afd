#include "stdafx.h"
#include "qtui_glview.h"
#include "MainInterface.h"

using namespace Qt;

HGE * hge=MainInterface::getInstance().hge;

QTUI_GLView::QTUI_GLView(QWidget *parent)
	: QGLWidget(parent)
{
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
	MainInterface::getInstance().OnPreInit();
	MainInterface::getInstance().OnInit(this, this->width(), this->height());
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	if (!updatetimer)
	{
		updatetimer = new QTimer(this);
		connect( updatetimer, SIGNAL(timeout()), SLOT(SIG_OnUpdate()) );
		updatetimer->start( 4 );
	}
}

void QTUI_GLView::resizeGL( int w, int h )
{
	MainInterface::getInstance().OnResizeWindow(w, h);
}

void QTUI_GLView::paintGL()
{
	MainInterface::getInstance().Render();
}

void QTUI_GLView::SIG_OnUpdate()
{
	if (hge)
	{
		float fps = hge->Timer_GetFPS(30);
		QString str = QString("%1").arg(fps);
		this->window()->setWindowTitle(str);
	}

	updateGL();

	MainInterface::getInstance().OnUpdateTimer();
//	ClearKeyState();
}

void QTUI_GLView::keyPressEvent( QKeyEvent * event )
{
	keyState[event->key()] = 1;
}

void QTUI_GLView::keyReleaseEvent( QKeyEvent *event )
{
	keyState[event->key()] = 0;
}

void QTUI_GLView::mouseMoveEvent( QMouseEvent *event )
{
	if (hge)
	{
		hge->Input_SetMousePos(event->x(), event->y());
	}
}

void QTUI_GLView::mousePressEvent( QMouseEvent *event )
{
	setFocus();
	if (hge)
	{
		hge->Input_SetDIMouseKey(event->button());
	}
}

void QTUI_GLView::mouseReleaseEvent( QMouseEvent *event )
{
	if (hge)
	{
		hge->Input_SetDIMouseKey(event->button(), false);
	}

}

void QTUI_GLView::wheelEvent( QWheelEvent *event )
{

}

void QTUI_GLView::enterEvent( QEvent *event )
{
	setFocus();
	QApplication::setOverrideCursor( QCursor( BlankCursor ) );
	setMouseTracking(true);
}

void QTUI_GLView::leaveEvent( QEvent *event )
{
	QApplication::setOverrideCursor( QCursor( ArrowCursor ) );
	setMouseTracking(false);
}

void QTUI_GLView::OnFrame()
{
	if (QApplication::mouseButtons() & MouseButton::LeftButton)
	{
		hge->Input_SetDIMouseKey(0);
	}
	if (QApplication::mouseButtons() & MouseButton::RightButton)
	{
		hge->Input_SetDIMouseKey(1);
	}
	if (keyState[Key_Space])
	{
		hge->Input_SetDIKey(DIK_SPACE);
	}
}

void QTUI_GLView::ClearKeyState()
{
	ZeroMemory(keyState, sizeof(keyState));
}