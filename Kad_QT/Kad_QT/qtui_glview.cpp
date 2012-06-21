#include "stdafx.h"
#include "qtui_glview.h"

HGE * hge = hgeCreate(HGE_VERSION);
HTARGET tar=NULL;
HTEXTURE tex=NULL;

bool FrameFunc()
{
	if (tar == NULL)
	{
		int nScreenWidth = hge->System_GetState(HGE_SCREENWIDTH);
		int nScreenHeight = hge->System_GetState(HGE_SCREENHEIGHT);
		int w = nScreenWidth;
		int h = nScreenHeight;
		tar = hge->Target_Create(w, h, false);

		hge->Gfx_BeginScene(tar);
		hge->Gfx_Clear(0);
		float ww = hge->Texture_GetWidth(tex, true);
		float hh = hge->Texture_GetHeight(tex, true);
		
	
		for (int i=0; i<6000; i++)
		{
			float x0 = hge->Random_Float(0, w);
			float y0 = hge->Random_Float(0, w);
			float x1 = hge->Random_Float(0, h);
			float y1 = hge->Random_Float(0, h);
			hge->Gfx_RenderLine(x0, y0, x1, y1, 0xff00ff00);
		}
	
		hge->Gfx_RenderLine(0, 0, 1000, 1000);

		hge->Gfx_EndScene();
	}
	
	return false;
}

bool RenderFunc()
{
	if (tex.tex == NULL)
	{
//		tex = hge->Texture_Load("e:\\hge[th09x]\\Resource\\Graphic\\font_00.png");
	}
	hge->Gfx_BeginScene();
	hge->Gfx_Clear(0xff808080);

	int w = hge->System_GetState(HGE_SCREENWIDTH);
	int h = hge->System_GetState(HGE_SCREENHEIGHT);
	/*
	for (int i=0; i<6000; i++)
	{
		float x0 = hge->Random_Float(0, w);
		float y0 = hge->Random_Float(0, w);
		float x1 = hge->Random_Float(0, h);
		float y1 = hge->Random_Float(0, h);
		hge->Gfx_RenderLine(x0, y0, x1, y1);
	}
	for (int i=0; i<100; i++)
	{
		hge->Gfx_RenderPoint(i, 200);
	}
	*/
	hge->Gfx_RenderLine(0, 0, 800, 100, 0xff00ff00);
	hge->Gfx_RenderLine(0, 0, 100, 100, 0xffff0000);
	if (tex.tex)
	{
		float ww = hge->Texture_GetWidth(tex, true);
		float hh = hge->Texture_GetHeight(tex, true);
		hgeQuad quad;
		quad.blend = BLEND_DEFAULT;
		quad.tex = tex;
		quad.v[0].x = 0; quad.v[0].y = 0; quad.v[0].tx = 0; quad.v[0].ty = 0; quad.v[0].z = 0; quad.v[0].col=0xffffffff;
		quad.v[1].x = ww; quad.v[1].y = 0; quad.v[1].tx = 1; quad.v[1].ty = 0; quad.v[1].z = 0; quad.v[1].col=0xffffffff;
		quad.v[2].x = ww; quad.v[2].y = hh; quad.v[2].tx = 1; quad.v[2].ty = 1; quad.v[2].z = 0; quad.v[2].col=0xffffffff;
		quad.v[3].x = 0; quad.v[3].y = hh; quad.v[3].tx = 0; quad.v[3].ty = 1; quad.v[3].z = 0; quad.v[3].col=0xffffffff;
//		hge->Gfx_RenderQuad(&quad);
	}
	
	if (tar)
	{
		hgeQuad quad;
		quad.blend = BLEND_DEFAULT;
		quad.tex = hge->Target_GetTexture(tar);
		float ww = hge->Texture_GetWidth(quad.tex, true);
		float hh = hge->Texture_GetHeight(quad.tex, true);
		quad.v[0].x = 0; quad.v[0].y = 0; quad.v[0].tx = 0; quad.v[0].ty = 0; quad.v[0].z = 0; quad.v[0].col=0xffffffff;
		quad.v[1].x = ww; quad.v[1].y = 0; quad.v[1].tx = 1; quad.v[1].ty = 0; quad.v[1].z = 0; quad.v[1].col=0xffffffff;
		quad.v[2].x = ww; quad.v[2].y = hh; quad.v[2].tx = 1; quad.v[2].ty = 1; quad.v[2].z = 0; quad.v[2].col=0xffffffff;
		quad.v[3].x = 0; quad.v[3].y = hh; quad.v[3].tx = 0; quad.v[3].ty = 1; quad.v[3].z = 0; quad.v[3].col=0xffffffff;
		hge->Gfx_RenderQuad(&quad);
	}
	

	hge->Gfx_EndScene();

	return false;
}

bool FocusLostFunc()
{
	return true;
}

bool FocusGainFunc()
{
	return true;
}

bool ExitFunc()
{
	if (tar)
	{
		hge->Target_Free(tar);
	}
	if (tex.tex)
	{
		hge->Texture_Free(tex);
	}
	return false;
}

bool GfxRestoreFunc()
{
	return true;
}

void InitHGE()
{
	hge->System_SetState(HGE_FRAMEFUNC, FrameFunc);
	hge->System_SetState(HGE_RENDERFUNC, RenderFunc);
	hge->System_SetState(HGE_FOCUSLOSTFUNC, FocusLostFunc);
	hge->System_SetState(HGE_FOCUSLOSTFUNC, FocusGainFunc);
	hge->System_SetState(HGE_EXITFUNC, ExitFunc);
	hge->System_SetState(HGE_GFXRESTOREFUNC, GfxRestoreFunc);

	hge->System_SetState(HGE_WINDOWED, true);
	hge->System_SetState(HGE_LOGFILE, "log.log");
	hge->System_SetState(HGE_INIFILE, "ini.ini");
	hge->System_SetState(HGE_USESOUND, false);
	hge->System_SetState(HGE_NOWMPAINT, true);
	hge->System_SetState(HGE_FPS, 60);

//	hge->System_SetState(HGE_SCREENWIDTH, w);
//	hge->System_SetState(HGE_SCREENHEIGHT, h);

	hge->System_SetState(HGE_MANAGELOOP, false);
	hge->System_SetState(HGE_OWNWINDOW, false);
	hge->System_SetState(HGE_CALLRENDER, false);

	hge->System_Initiate();

}

QTUI_GLView::QTUI_GLView(QWidget *parent)
	: QGLWidget(parent)
{
	updatetimer = NULL;
}

QTUI_GLView::~QTUI_GLView()
{
	if (updatetimer)
	{
		delete updatetimer;
	}
	if (hge)
	{
		hgeCallback ef = hge->System_GetState(HGE_EXITFUNC);
		if (ef)
		{
			ef();
		}
		hge->Release();
	}
}

void QTUI_GLView::initializeGL()
{
	InitHGE();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	if (!updatetimer)
	{
		updatetimer = new QTimer(this);
		connect( updatetimer, SIGNAL(timeout()), SLOT(SIG_OnUpdate()) );
		updatetimer->start( 16 );
	}
}

void QTUI_GLView::resizeGL( int w, int h )
{
	hge->System_SetState(HGE_SCREENWIDTH, w);
	hge->System_SetState(HGE_SCREENHEIGHT, h);
	hge->Gfx_Resize(w, h);
}

void QTUI_GLView::paintGL()
{
	if (hge)
	{
		hgeCallback rf = hge->System_GetState(HGE_RENDERFUNC);
		if (rf)
		{
			rf();
		}
	}
}

void QTUI_GLView::SIG_OnUpdate()
{
	float fps = hge->Timer_GetFPS(30);
	QString str = QString("%1").arg(fps);
	this->window()->setWindowTitle(str);
	updateGL();
	hge->System_Start();
}