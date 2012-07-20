#include "stdafx.h"
#include "qtui_glview.h"
#include "MainInterface.h"
#include "qmaininterface.h"
#include "qkeystatemanager.h"

using namespace Qt;

HGE * hge=MainInterface::getInstance().hge;

QTUI_GLView::QTUI_GLView(QWidget *parent)
	: QGLWidget(parent)
{
    QMainInterface::getInstance().SetPGLView(this);

    updatetimer = NULL;

    this->installEventFilter(this);
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

}

void QTUI_GLView::OnMainFrameSetupUIDone()
{
    MainInterface::getInstance().OnPreInit();
    MainInterface::getInstance().OnInit(this, this->width(), this->height());
    if (!updatetimer)
    {
        updatetimer = new QTimer(this);
        connect( updatetimer, SIGNAL(timeout()), SLOT(SLT_OnUpdate()) );
        updatetimer->start( 12 );
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

    QMainInterface::getInstance().GetPStatusBar()->UpdateStatusBar();
    //	ClearKeyState();
}

void QTUI_GLView::SLT_HScrollValueChanged(int val)
{
    static int curpos = 5000;
    static int lastpos = curpos;

    lastpos = curpos;
    curpos = val;

    MainInterface::getInstance().OnDoScroll(true, curpos-lastpos, 1000);
}

void QTUI_GLView::SLT_VScrollValueChanged(int val)
{
    static int curpos = 5000;
    static int lastpos = curpos;

    lastpos = curpos;
    curpos = val;

    MainInterface::getInstance().OnDoScroll(false, curpos-lastpos, 1000);
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
	int numDegrees = e->delta() / 8;
	int numSteps = numDegrees / 15;

	if (e->orientation() == Qt::Horizontal)
	{
		hge->Input_SetMouseWheel(numSteps, true);
	}
	else
	{
		hge->Input_SetMouseWheel(numSteps, false);
	}
	setFocus();
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

bool QTUI_GLView::eventFilter(QObject *target, QEvent *e)
{
    int etype = e->type();
    if (etype == QEvent::KeyPress || etype == QEvent::KeyRelease || etype == QEvent::MouseButtonPress || etype == QEvent::MouseButtonRelease || etype == QEvent::MouseButtonDblClick || etype == QEvent::Wheel)
    {
        QMainInterface * pqm = &QMainInterface::getInstance();
        if (!pqm->GetPNodeInfoFloatingWidget()->isHidden())
        {
            return true;
        }
    }

    if (etype == QEvent::KeyPress)
	{

        QKeyEvent * pke = static_cast<QKeyEvent *>(e);

		int ekey = pke->key();

        QKeyStateManager * qksm = &QKeyStateManager::getInstance();
        qksm->SetKey(ekey);

		QMainInterface * pqm = &QMainInterface::getInstance();

		if (ekey == Qt::Key_Tab)
		{
			QTUI_MarkingFloating_Widget * pMarkingWidget = pqm->GetNextPMarkingWidget();
			if (pMarkingWidget)
			{
				pMarkingWidget->OnTabFocus();
			}
			else
			{
				pqm->GetPCommandFloatingWidget()->OnReceiveTextFromGL("");
			}
			return true;
		}

        Qt::KeyboardModifiers km = pke->modifiers();
        if (km)
        {

            if (km & Qt::ALT)
            {
                qksm->SetKey(Qt::ALT);
            }
            if (km & Qt::CTRL)
            {
                qksm->SetKey(Qt::CTRL);
            }
            if (km & Qt::META)
            {
                qksm->SetKey(Qt::META);
            }
            if (km & Qt::SHIFT)
            {
                qksm->SetKey(Qt::SHIFT);
            }

            return true;
		}
		if (ekey != Qt::Key_Backspace && ekey != Qt::Key_Delete && ekey != Qt::Key_Space && ekey != Qt::Key_Escape)
		{
			if (!pke->text().isEmpty())
			{
				pqm->GetPCommandFloatingWidget()->OnReceiveTextFromGL(pke->text());
			}
		}
    }
    else if(etype == QEvent::KeyRelease)
	{
		QKeyEvent * pke = static_cast<QKeyEvent *>(e);

		QKeyStateManager * qksm = &QKeyStateManager::getInstance();
		qksm->SetKey(pke->key(), false);

		Qt::KeyboardModifiers km = pke->modifiers();
		if (km)
		{

			if (km & Qt::ALT)
			{
				qksm->SetKey(Qt::ALT, false);
			}
			if (km & Qt::CTRL)
			{
				qksm->SetKey(Qt::CTRL, false);
			}
			if (km & Qt::META)
			{
				qksm->SetKey(Qt::META, false);
			}
			if (km & Qt::SHIFT)
			{
				qksm->SetKey(Qt::SHIFT, false);
			}

			return true;
		}
	}

    return QGLWidget::eventFilter(target, e);
}

void QTUI_GLView::focusInEvent( QFocusEvent * e )
{
	QGLWidget::focusInEvent(e);
	setCursor(Qt::BlankCursor);
}

void QTUI_GLView::focusOutEvent( QFocusEvent * e )
{
	QGLWidget::focusOutEvent(e);
	setCursor(Qt::ArrowCursor);
}

void QTUI_GLView::OnFrame()
{
	if (QApplication::mouseButtons() & Qt::LeftButton)
	{
		hge->Input_SetDIMouseKey(0);
	}
	if (QApplication::mouseButtons() & Qt::RightButton)
	{
		hge->Input_SetDIMouseKey(1);
	}

    QKeyStateManager * qksm = &QKeyStateManager::getInstance();

    if (QApplication::keyboardModifiers() & Qt::ALT)
    {
        qksm->SetKey(Qt::ALT);
    }
    else
    {
        qksm->SetKey(Qt::ALT, false);
    }
    if (QApplication::keyboardModifiers() & Qt::CTRL)
    {
        qksm->SetKey(Qt::CTRL);
    }
    else
    {
        qksm->SetKey(Qt::CTRL, false);
    }
    if (QApplication::keyboardModifiers() & Qt::META)
    {
        qksm->SetKey(Qt::META);
    }
    else
    {
        qksm->SetKey(Qt::META, false);
    }
    if (QApplication::keyboardModifiers() & Qt::SHIFT)
    {
        qksm->SetKey(Qt::SHIFT);
    }
    else
    {
        qksm->SetKey(Qt::SHIFT, false);
    }

    qksm->Flush();
}

void QTUI_GLView::OnFrameEnd()
{
	QMainInterface::getInstance().ResolveMarkingOverlapping();
}