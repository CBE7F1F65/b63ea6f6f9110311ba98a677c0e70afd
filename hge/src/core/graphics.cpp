/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Core functions implementation: graphics
*/

/************************************************************************/
/* Part of this file is modified by h5nc (h5nc@yahoo.com.cn)            */
/* Changes of adjusting from d3d8 to d3d9 will not be marked in detail  */
/************************************************************************/

#include "hge_impl.h"

#if IF_RENDERSYS(HRENDERSYS_DX)

#include <d3d9.h>
#include <d3dx9.h>

#elif IF_RENDERSYS(HRENDERSYS_PSP)

#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspgu.h>
#include <pspgum.h>
#include <psprtc.h>

#include "../src/psp/PSP_graphics.h"

static unsigned int __attribute__((aligned(16))) gulist[262144];

#define SLICE_SIZE_F			64.0f

#elif IF_RENDERSYS(HRENDERSYS_GLES)

#include "../PSP/PSP_graphics.h"

#endif

DWORD _ARGBtoABGR(DWORD color)
{
	return (GETR(color) | (GETG(color)<<8) | (GETB(color)<<16) | (GETA(color)<<24));
}
DWORD _ARGBtoRGBA(DWORD color)
{
	return (color<<8)|(color>>24);
}

void CALL HGE_Impl::Gfx_Clear(DWORD color)
{
	/************************************************************************/
	/* TODO                                                                 */
	/************************************************************************/
	if(pCurTarget)
	{
#if IF_RENDERSYS(HRENDERSYS_DX)
		if(pCurTarget->pDepth)
			pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 1.0f, 0 );
		else
			pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, color, 1.0f, 0 );
#endif
	}
	else
	{
#if IF_RENDERSYS(HRENDERSYS_DX)
		if(bZBuffer)
			pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color, 1.0f, 0 );
		else
			pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET, color, 1.0f, 0 );
#endif
	}

#if IF_RENDERSYS(HRENDERSYS_PSP)
	sceGuDisable(GU_SCISSOR_TEST);
	sceGuClearColor(color);
	sceGuClear(GU_COLOR_BUFFER_BIT);
	sceGuEnable(GU_SCISSOR_TEST);

#elif IF_RENDERSYS(HRENDERSYS_GLS)

	glDisable(GL_SCISSOR_TEST);
	glClearColor( (float)GETR(color)/255.0f, (float)GETG(color)/255.0f, (float)GETB(color)/255.0f, (float)GETA(color)/255.0f );
	glClear( GL_COLOR_BUFFER_BIT );
	glEnable(GL_SCISSOR_TEST);

#endif
}

void CALL HGE_Impl::Gfx_SetClipping(int x, int y, int w, int h)
{
	int scr_width = 0;
	int	scr_height = 0;

	if(!pCurTarget) {
		scr_width=pHGE->System_GetStateInt(HGE_SCREENWIDTH);
		scr_height=pHGE->System_GetStateInt(HGE_SCREENHEIGHT);
	}
	else {
		scr_width=Texture_GetWidth((HTEXTURE)((DWORD)pCurTarget->pTex));
		scr_height=Texture_GetHeight((HTEXTURE)((DWORD)pCurTarget->pTex));
	}
#if IF_RENDERSYS(HRENDERSYS_DX)
	D3DVIEWPORT9 vp;

	if(!w) {
		vp.X=0;
		vp.Y=0;
		vp.Width=scr_width;
		vp.Height=scr_height;
	}
	else
	{
		if(x<0) { w+=x; x=0; }
		if(y<0) { h+=y; y=0; }

		if(x+w > scr_width) w=scr_width-x;
		if(y+h > scr_height) h=scr_height-y;

		vp.X=x;
		vp.Y=y;
		vp.Width=w;
		vp.Height=h;
	}

	vp.MinZ=0.0f;
	vp.MaxZ=1.0f;

	_render_batch();
	pD3DDevice->SetViewport(&vp);

	D3DXMATRIX tmp;
	Math_MatrixScaling(&matProj, 1.0f, -1.0f, 1.0f);
	Math_MatrixTranslation(&tmp, -0.5f, +0.5f, 0.0f);
	Math_MatrixMultiply(&matProj, &matProj, &tmp);
	Math_MatrixOrthoOffCenterLH(&tmp, (float)vp.X, (float)(vp.X+vp.Width), -((float)(vp.Y+vp.Height)), -((float)vp.Y), vp.MinZ, vp.MaxZ);
	Math_MatrixMultiply(&matProj, &matProj, &tmp);
	pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);
#elif IF_RENDERSYS(HRENDERSYS_PSP)
	if (!w)
	{
		w = scr_width;
		h = scr_height;
	}
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuScissor(x,y,x+w,y+h);
#elif IF_RENDERSYS(HRENDERSYS_GLS)
	if (!w)
	{
		w = scr_width;
		h = scr_height;
	}
	glEnable(GL_SCISSOR_TEST);
	glScissor(x, y, w, h);
#endif
}

/************************************************************************/
/* These functions are added by h5nc (h5nc@yahoo.com.cn)                */
/************************************************************************/
void CALL HGE_Impl::Gfx_SetTransform(float x, float y, float dx, float dy, float rot, float hscale, float vscale)
{
	D3DXMATRIX tmp;

	if(vscale==0.0f) Math_MatrixIdentity(&matView);
	else
	{
		Math_MatrixTranslation(&matView, -x, -y, 0.0f);
		Math_MatrixScaling(&tmp, hscale, vscale, 1.0f);
		Math_MatrixMultiply(&matView, &matView, &tmp);
		Math_MatrixRotationZ(&tmp, -rot);
		Math_MatrixMultiply(&matView, &matView, &tmp);
		Math_MatrixTranslation(&tmp, x+dx, y+dy, 0.0f);
		Math_MatrixMultiply(&matView, &matView, &tmp);
	}

	_render_batch();
	Gfx_SetTransform(D3DTS_VIEW, &matView);
}

void CALL HGE_Impl::Gfx_SetTransform(D3DTRANSFORMSTATETYPE State, const D3DMATRIX * pMatrix)
{
	_render_batch();
#if IF_RENDERSYS(HRENDERSYS_DX)
	pD3DDevice->SetTransform(State, pMatrix);
#elif IF_RENDERSYS(HRENDERSYS_PSP)
	sceGumMatrixMode((int)State);
	sceGumLoadIdentity();
	sceGumMultMatrix((ScePspFMatrix4*)pMatrix);
#elif IF_RENDERSYS(HRENDERSYS_GLS)
	glMatrixMode((int)State);
	glLoadIdentity();
	glMultMatrixf((GLfloat*)pMatrix);
#endif
}

D3DMATRIX CALL HGE_Impl::Gfx_GetTransform(D3DTRANSFORMSTATETYPE State)
{
	D3DMATRIX mat;
#if IF_RENDERSYS(HRENDERSYS_DX)
	pD3DDevice->GetTransform(State, &mat);
#elif IF_RENDERSYS(HRENDERSYS_PSP)
	sceGumMatrixMode((int)State);
	sceGumStoreMatrix((ScePspFMatrix4*)&mat);
#elif IF_RENDERSYS(HRENDERSYS_GLS)
	glMatrixMode((GLenum)State);
	glLoadMatrixf((GLfloat*)&mat);
#endif
	return mat;
}

void CALL HGE_Impl::Gfx_SetTextureInfo(int _nTexInfo, hgeTextureInfo * _texInfo)
{
	nTexInfo = _nTexInfo;
	texInfo = _texInfo;
}

bool CALL HGE_Impl::Gfx_BeginScene(HTARGET targ)
{
	glBindTexture(GL_TEXTURE_2D, 0);
	CurTexture = 0;

	CRenderTargetList *target=(CRenderTargetList *)targ;
#if IF_RENDERSYS(HRENDERSYS_DX)
	LPDIRECT3DSURFACE9 pSurf=0, pDepth=0;
	D3DDISPLAYMODE Mode;

	HRESULT hr = pD3DDevice->TestCooperativeLevel();
	if (hr == D3DERR_DEVICELOST) return false;
	else if (hr == D3DERR_DEVICENOTRESET)
	{
		if(bWindowed)
		{
			if(FAILED(pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &Mode)) || Mode.Format==D3DFMT_UNKNOWN)
			{
				_PostError("Can't determine desktop video mode");
				return false;
			}

			d3dppW.BackBufferFormat = Mode.Format;
			if(_format_id(Mode.Format) < 4) nScreenBPP=16;
			else nScreenBPP=32;
		}
	    if(!_GfxRestore()) return false;
	}
#endif
	if(VertArray)
	{
		_PostError("Gfx_BeginScene: Scene is already being rendered");
		return false;
	}
#if IF_RENDERSYS(HRENDERSYS_DX)
	if(target != pCurTarget)
	{
		if(target)
		{
			target->pTex->GetSurfaceLevel(0, &pSurf);
			pDepth=target->pDepth;
		}
		else
		{
			pSurf=pScreenSurf;
			pDepth=pScreenDepth;
		}
		//+1st -3rd
		if(FAILED(pD3DDevice->SetRenderTarget(0, pSurf)))
		{
			if(target) pSurf->Release();
			_PostError("Gfx_BeginScene: Can't set render target");
			return false;
		}
		if(target)
		{
			pSurf->Release();
			if(target->pDepth)
				pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
			else
				pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );

			/************************************************************************/
			/* This line is deleted by h5nc (h5nc@yahoo.com.cn)                     */
			/************************************************************************/
//			_SetProjectionMatrix(target->width, target->height);
		}
		else
		{
			if(bZBuffer) pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_TRUE );
			else pD3DDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE );
			_SetProjectionMatrix(nScreenWidth, nScreenHeight);
		}

		/************************************************************************/
		/* These 3 lines are deleted by h5nc (h5nc@yahoo.com.cn)                 */
		/************************************************************************/
//		pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);
//		D3DXMatrixIdentity(&matView);
//		pD3DDevice->SetTransform(D3DTS_VIEW, &matView);

		pCurTarget=target;
	}
	pD3DDevice->BeginScene();
	pVB->Lock( 0, 0, (void**)&VertArray, 0 );
#elif IF_RENDERSYS(HRENDERSYS_PSP)
	sceGuStart(GU_DIRECT, gulist);
	matProj.m[0][0] = SCREEN_HEIGHT/SCREEN_WIDTH;
	matProj.m[0][1] = 0.0f;
	matProj.m[0][2] = 0.0f;
	matProj.m[0][3] = 0.0f;
	matProj.m[1][0] = 0.0f;
	matProj.m[1][1] = -1.0f;
	matProj.m[1][2] = 0.0f;
	matProj.m[1][3] = 0.0f;
	matProj.m[2][0] = 0.0f;
	matProj.m[2][1] = 0.0f;
	matProj.m[2][2] = -1.0f;
	matProj.m[2][3] = -1.0f;
	matProj.m[3][0] = -SCREEN_HEIGHT/2;
	matProj.m[3][1] = SCREEN_HEIGHT/2;
	matProj.m[3][2] = SCREEN_HEIGHT/2;
	matProj.m[3][3] = SCREEN_HEIGHT/2;

	sceGumMatrixMode(GU_PROJECTION);
	sceGumLoadIdentity();
	sceGumMultMatrix((ScePspFMatrix4*)&matProj);

	sceGumMatrixMode(GU_VIEW);
	sceGumLoadIdentity();
#elif IF_RENDERSYS(HRENDERSYS_GLS)

	VertArray = VecVertArray;//new hgeVertex[VERTEX_BUFFER_SIZE];
 #if IF_FRAMWORK(HFRAMEWORK_QT)
	if (target != pCurTarget)
	{
		if (target)
		{
			QGLFunctions funcs(QGLContext::currentContext());
			bool bframebuffers = funcs.hasOpenGLFeature(QGLFunctions::Framebuffers);
			if (bframebuffers)
			{
				/*GLuint fbo;
				funcs.glGenFramebuffers(1, &fbo);
				funcs.glBindFramebuffer(GL_FRAMEBUFFER, fbo);
				funcs.glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target->pTex, 0);*/
				funcs.glBindFramebuffer(GL_FRAMEBUFFER, target->pFrameBuffer);


				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glOrtho(0, target->width, target->height, 0, target->height/2, 0);

				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
			}
		}
		else
		{
			QGLFunctions funcs(QGLContext::currentContext());
			bool bframebuffers = funcs.hasOpenGLFeature(QGLFunctions::Framebuffers);
			if (bframebuffers)
			{
				funcs.glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
		}
		pCurTarget = target;


	}
 #endif /*HFRAMEWORK_QT*/

 #if IF_PLATFORM(HPLATFORM_IOS)
	matProj.m[0][0] = SCREEN_HEIGHT/SCREEN_WIDTH;
	matProj.m[0][1] = 0.0f;
	matProj.m[0][2] = 0.0f;
	matProj.m[0][3] = 0.0f;
	matProj.m[1][0] = 0.0f;
	matProj.m[1][1] = -1.0f;
	matProj.m[1][2] = 0.0f;
	matProj.m[1][3] = 0.0f;
	matProj.m[2][0] = 0.0f;
	matProj.m[2][1] = 0.0f;
	matProj.m[2][2] = -1.0f;
	matProj.m[2][3] = -1.0f;
	matProj.m[3][0] = -SCREEN_HEIGHT/2;
	matProj.m[3][1] = SCREEN_HEIGHT/2;
	matProj.m[3][2] = SCREEN_HEIGHT/2;
	matProj.m[3][3] = SCREEN_HEIGHT/2;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
//	glOrthof(0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, SCREEN_HEIGHT/2, 0);
	glMultMatrixf((GLfloat*)&matProj);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
 #endif /*HPLATFORM_IOS*/

#endif //DX

	return true;
}

#if IF_PLATFORM(HPLATFORM_PSP)
#include <malloc.h>
int __freemem()
{
	void *ptrs[480];
	int mem, x, i;
	void *ptr;

	for (x = 0; x < 480; x++)
	{
		ptr = malloc(51200);
		if (!ptr) break;

		ptrs[x] = ptr;
	}
	mem = x * 51200;
	for (i = 0; i < x; i++)
		free(ptrs[i]);

	return mem;
}
#endif // PSP

void CALL HGE_Impl::Gfx_EndScene()
{
	_render_batch(true);
#if IF_RENDERSYS(HRENDERSYS_DX)
	pD3DDevice->EndScene();
	if(!pCurTarget) pD3DDevice->Present( NULL, NULL, NULL, NULL );
#elif IF_RENDERSYS(HRENDERSYS_PSP)
	sceGuFinish();
	sceGuSync(0,0);

	pspDebugScreenSetXY(0, 0);
	int freemem = __freemem();
	pspDebugScreenPrintf("%f  %d(%.2fM)", Timer_GetFPS(), freemem, freemem*1.0f/(1024*1024));

	sceGuSwapBuffers();
#endif
}


void CALL HGE_Impl::Gfx_RenderPoint(float x, float y, float z, DWORD color)
{
#if IF_RENDERSYS(HRENDERSYS_DX|HRENDERSYS_GLS)
	if(VertArray)
	{
		if(CurPrimType!=HGEPRIM_POINTS || nPrim>=VERTEX_BUFFER_SIZE/HGEPRIM_POINTS || CurTexture || CurBlendMode!=BLEND_DEFAULT)
		{
			_render_batch();

			CurPrimType=HGEPRIM_POINTS;
			if(CurBlendMode != BLEND_DEFAULT) _SetBlendMode(BLEND_DEFAULT);
 #if IF_RENDERSYS(HRENDERSYS_DX)
			if(CurTexture) { pD3DDevice->SetTexture(0, 0); CurTexture=0; }
 #else
			if (CurTexture)	{glDisable(GL_TEXTURE_2D); CurTexture=0;};
 #endif
		}

		int i=nPrim*HGEPRIM_POINTS;
		VertArray[i].x = x;
		VertArray[i].y = y;
		VertArray[i].z = z;
 #if IF_RENDERSYS(HRENDERSYS_GLS)
		VertArray[i].col = _ARGBtoABGR(color);
 #else
		VertArray[i].col = color;
 #endif
		VertArray[i].tx = VertArray[i].ty = 0.0f;

		nPrim++;
	}
#elif IF_RENDERSYS(HRENDERSYS_PSP)
	struct pspVertex* vertices = (struct pspVertex*)sceGuGetMemory(1 * sizeof(struct pspVertex));
	if (!vertices)
	{
		return;
	}
	vertices[0].color = color;
	vertices[0].x = x;
	vertices[0].y = y;

	sceGuDisable(GU_TEXTURE_2D);
	sceGuShadeModel(GU_FLAT);
	sceGuShadeModel(GU_SMOOTH);
	sceGuAmbientColor(0xffffffff);
	sceGumDrawArray(GU_POINTS, GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_3D, 1*1, 0, vertices);
	sceGuEnable(GU_TEXTURE_2D);
	/*

	For Render Without Buffer

#elif defined __IPHONE || __BOOL_USEOPENGL_QT
	if (CurBlendMode != BLEND_DEFAULT)
	{
		_SetBlendMode(BLEND_DEFAULT);
	}
	glDisable(GL_TEXTURE_2D);

	hgeVertex vertices[HGEPRIM_POINTS];

	vertices[0].x = x;
	vertices[0].y = y;
	vertices[0].z = z;
	vertices[0].col = _ARGBtoABGR(color);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(hgeVertex), &(vertices[0].col));
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(hgeVertex), &(vertices[0].x));

	glDrawArrays(GL_POINTS, 0, HGEPRIM_POINTS);
	glEnable(GL_TEXTURE_2D);
	*/
#endif // WIN32
}

void CALL HGE_Impl::Gfx_RenderLine2(float x1, float y1, float x2, float y2, DWORD color1/* =0xFFFFFFFF */, DWORD color2/* =0xFFFFFFFF */, float z1/* =0 */, float z2/* =0 */)
{
#if IF_RENDERSYS(HRENDERSYS_DX|HRENDERSYS_GLS)
	if(VertArray)
	{
		if(CurPrimType!=HGEPRIM_LINES || nPrim>=VERTEX_BUFFER_SIZE/HGEPRIM_LINES || CurTexture || CurBlendMode!=BLEND_DEFAULT)
		{
			_render_batch();

			CurPrimType=HGEPRIM_LINES;
			if(CurBlendMode != BLEND_DEFAULT) _SetBlendMode(BLEND_DEFAULT);
 #if IF_RENDERSYS(HRENDERSYS_DX)
			if(CurTexture) { pD3DDevice->SetTexture(0, 0); CurTexture=0; }
 #else
			if (CurTexture)	{glDisable(GL_TEXTURE_2D); CurTexture=0;};
 #endif
		}

		int i=nPrim*HGEPRIM_LINES;
		VertArray[i].x = x1; VertArray[i+1].x = x2;
		VertArray[i].y = y1; VertArray[i+1].y = y2;
		VertArray[i].z = z1; VertArray[i+1].z = z2;
 #if IF_RENDERSYS(HRENDERSYS_GLS)
		VertArray[i].col = _ARGBtoABGR(color1); VertArray[i+1].col = _ARGBtoABGR(color2);
 #else
		VertArray[i].col = color1; VertArray[i+1].col = color2;
 #endif
		VertArray[i].tx    = VertArray[i+1].tx =
			VertArray[i].ty    = VertArray[i+1].ty = 0.0f;

		nPrim++;
	}
#elif IF_RENDERSYS(HRENDERSYS_PSP)
	struct pspVertex* vertices = (struct pspVertex*)sceGuGetMemory(HGEPRIM_LINES * sizeof(struct pspVertex));
	if (!vertices)
	{
		return;
	}
	vertices[0].color = color1;
	vertices[0].x = x1;
	vertices[0].y = y1;
	vertices[0].z = z1;

	vertices[1].color = color2;
	vertices[1].x = x2;
	vertices[1].y = y2;
	vertices[1].z = z2;

	sceGuDisable(GU_TEXTURE_2D);
	sceGuShadeModel(GU_FLAT);
	sceGuShadeModel(GU_SMOOTH);
	sceGuAmbientColor(0xffffffff);
	sceGumDrawArray(GU_LINES, GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_3D, HGEPRIM_LINES, 0, vertices);
	sceGuEnable(GU_TEXTURE_2D);
	/*
#elif defined __IPHONE || __BOOL_USEOPENGL_QT

	if (CurBlendMode != BLEND_DEFAULT)
	{
		_SetBlendMode(BLEND_DEFAULT);
	}
	glDisable(GL_TEXTURE_2D);

	hgeVertex vertices[HGEPRIM_LINES];

	vertices[0].x = x1;
	vertices[0].y = y1;
	vertices[0].z = z1;
	vertices[0].col = _ARGBtoABGR(color1);

	vertices[1].x = x2;
	vertices[1].y = y2;
	vertices[1].z = z2;
	vertices[1].col = _ARGBtoABGR(color2);

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(hgeVertex), &(vertices[0].col));
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(hgeVertex), &(vertices[0].x));

	glDrawArrays(GL_LINES, 0, HGEPRIM_LINES);
	glEnable(GL_TEXTURE_2D);
	*/

#endif // WIN32
}

void CALL HGE_Impl::Gfx_RenderLine(float x1, float y1, float x2, float y2, DWORD color, float z)
{
	Gfx_RenderLine2(x1, y1, x2, y2, color, color, z, z);
}

/************************************************************************/
/* These 2 functions are added by h5nc (h5nc@yahoo.com.cn) for later use */
/************************************************************************/
/*
void CALL HGE_Impl::Gfx_3DRenderStart()
{
	if(VertArray)
	{
		_render_batch();
	}

	pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	if (!(CurBlendMode & BLEND_ZWRITE))
	{
		pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}
}

void CALL HGE_Impl::Gfx_3DRenderEnd()
{
	pD3DDevice->SetStreamSource( 0, pVB, 0, sizeof(hgeVertex));
	pD3DDevice->SetIndices( pIB );

	if (!(CurBlendMode & BLEND_ZWRITE))
	{
		pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	}
	pD3DDevice->SetFVF(D3DFVF_HGEVERTEX);
	pD3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
}
*/

void CALL HGE_Impl::Gfx_RenderTriple(const hgeTriple *triple)
{
#if IF_RENDERSYS(HRENDERSYS_DX)
	if(VertArray)
	{
		HTEXTURE tex = triple->tex;
		Texture_LoadTextureWhenNeeded(&tex);
		DWORD ttex = Texture_GetTexture(tex);
		if(CurPrimType!=HGEPRIM_TRIPLES || nPrim>=VERTEX_BUFFER_SIZE/HGEPRIM_TRIPLES || CurTexture!=ttex || CurBlendMode!=triple->blend)
		{
			_render_batch();

			CurPrimType=HGEPRIM_TRIPLES;
			if(CurBlendMode != triple->blend) _SetBlendMode(triple->blend);

			if(ttex != CurTexture) {
				pD3DDevice->SetTexture( 0, (LPDIRECT3DTEXTURE9)ttex );
				CurTexture = ttex;
			}
		}

		memcpy(&VertArray[nPrim*HGEPRIM_TRIPLES], triple->v, sizeof(hgeVertex)*HGEPRIM_TRIPLES);
		nPrim++;
	}
#endif
}

void CALL HGE_Impl::Gfx_RenderQuad(const hgeQuad *quad)
{
	if (quad->v[0].col == 0 && quad->v[1].col == 0 && quad->v[2].col == 0 && quad->v[3].col == 0 && !((quad->blend)&BLEND_COLORMUL)) {
		return;
	}
#if IF_RENDERSYS(HRENDERSYS_DX|HRENDERSYS_GLS)
	if(VertArray)
	{
		HTEXTURE tex = quad->tex;
		Texture_LoadTextureWhenNeeded(&tex);
		DWORD ttex = Texture_GetTexture(tex);
		if(CurPrimType!=HGEPRIM_QUADS || nPrim>=VERTEX_BUFFER_SIZE/HGEPRIM_QUADS || CurTexture!=ttex || CurBlendMode!=quad->blend)
		{
			_render_batch();

			CurPrimType=HGEPRIM_QUADS;
			if(CurBlendMode != quad->blend) _SetBlendMode(quad->blend);
/*
			if (!ttex)
			{
				return;
			}*/

			if(ttex != CurTexture)
			{
 #if IF_RENDERSYS(HRENDERSYS_DX)
				pD3DDevice->SetTexture( 0, (LPDIRECT3DTEXTURE9)ttex );
 #elif IF_RENDERSYS(HRENDERSYS_GLES)
				Image * pTex = (Image *)Texture_GetTexture(tex);
				if(pTex == 0 || pTex->textureWidth == 0 || pTex->textureHeight ==0 || !pTex->data)
				{
					return;
				}

				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, pTex->texid);
 #elif IF_RENDERSYS(HRENDERSYS_GL)

				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, ttex);
 #endif
				CurTexture = ttex;
			}
		}
 #if IF_RENDERSYS(HRENDERSYS_GLS)
		int beginindex = nPrim*HGEPRIM_QUADS;
		for (int i=0; i<HGEPRIM_QUADS; i++) {
			int j=i;
			if (i > 1)
			{
				j = HGEPRIM_QUADS+1-i;
			}
			VertArray[i+beginindex].tx = quad->v[j+beginindex].tx;
			VertArray[i+beginindex].ty = quad->v[j+beginindex].ty;
			VertArray[i+beginindex].x = quad->v[j+beginindex].x;
			VertArray[i+beginindex].y = quad->v[j+beginindex].y;
			VertArray[i+beginindex].z = quad->v[j+beginindex].z;
			VertArray[i+beginindex].col = _ARGBtoABGR(quad->v[j+beginindex].col);
		}
		for (int i=0; i<HGEPRIM_QUADS/2; i++)
		{
			float ty = VertArray[i+beginindex].y;
			VertArray[i+beginindex].y = VertArray[i+beginindex+HGEPRIM_QUADS/2].y;
			VertArray[i+beginindex+HGEPRIM_QUADS/2].y = ty;
		}
 #else
		memcpy(&VertArray[nPrim*HGEPRIM_QUADS], quad->v, sizeof(hgeVertex)*HGEPRIM_QUADS);
 #endif
		nPrim++;
	}
#elif IF_RENDERSYS(HRENDERSYS_PSP)
	struct pspVertexUV *vertices;
	int i;

	HTEXTURE tex = quad->tex;
	Texture_LoadTextureWhenNeeded(&tex);
	Image * pTex = (Image *)Texture_GetTexture(tex);
	if(pTex == 0 || pTex->textureWidth == 0 || pTex->textureHeight ==0 || !pTex->data)
	{
		return;
	}

	sceGuTexMode(GU_PSM_8888, 0, 0, GU_TRUE);
	if((DWORD)pTex != CurTexture)
	{
		CurTexture = (DWORD)pTex;
		sceGuTexImage(0, pTex->textureWidth, pTex->textureHeight, pTex->textureWidth, pTex->data);
		sceKernelDcacheWritebackAll();
	}
	if (CurBlendMode != quad->blend)
	{
		_SetBlendMode(quad->blend);
	}

	vertices = (struct pspVertexUV*)sceGuGetMemory(HGEPRIM_QUADS * sizeof(struct pspVertexUV));
	if (!vertices)
	{
		return;
	}
	for (i=0; i<HGEPRIM_QUADS; i++)
	{
		int j = i;
		if (i > 1)
		{
			j = 5-i;
		}
		vertices[i].u = quad->v[j].tx;
		vertices[i].v = quad->v[j].ty;
		vertices[i].x = quad->v[j].x;
		vertices[i].y = quad->v[j].y;
		vertices[i].z = quad->v[j].z;
		vertices[i].color = _ARGBtoABGR(quad->v[j].col);
	}
	sceGumDrawArray(GU_TRIANGLE_STRIP,GU_TEXTURE_32BITF|GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_3D, HGEPRIM_QUADS, 0, vertices);
	/*
#elif defined __IPHONE || __BOOL_USEOPENGL_QT

	HTEXTURE tex = quad->tex;
	Texture_LoadTextureWhenNeeded(&tex);

	Image * pTex = (Image *)Texture_GetTexture(tex);
	if(pTex == 0 || pTex->textureWidth == 0 || pTex->textureHeight ==0 || !pTex->data)
	{
		return;
	}

	glEnable(GL_TEXTURE_2D);
	if((DWORD)pTex != CurTexture)
	{
		CurTexture = (DWORD)pTex;
		glBindTexture(GL_TEXTURE_2D, pTex->texid);
	}
	if (CurBlendMode != quad->blend)
	{
		_SetBlendMode(quad->blend);
	}

	hgeVertex vertices[HGEPRIM_QUADS];

	for (int i=0; i<HGEPRIM_QUADS; i++) {
		int j=i;
		if (i > 1)
		{
			j = 5-i;
		}
		vertices[i].tx = quad->v[j].tx;
		vertices[i].ty = quad->v[j].ty;
		vertices[i].x = quad->v[j].x;
		vertices[i].y = quad->v[j].y;
		vertices[i].z = quad->v[j].z;
		vertices[i].col = _ARGBtoABGR(quad->v[j].col);
	}

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(HGEPRIM_QUADS, GL_UNSIGNED_BYTE, sizeof(hgeVertex), &(vertices[0].col));
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, sizeof(hgeVertex), &(vertices[0].tx));
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(hgeVertex), &(vertices[0].x));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, HGEPRIM_QUADS);
	glDisable(GL_TEXTURE_2D);
	*/
#endif
}

hgeVertex* CALL HGE_Impl::Gfx_StartBatch(int prim_type, HTEXTURE tex, int blend, int *max_prim)
{
#if IF_RENDERSYS(HRENDERSYS_DX|HRENDERSYS_GLS)
	if(VertArray)
	{
		_render_batch();

		CurPrimType=prim_type;
		if(CurBlendMode != blend) _SetBlendMode(blend);
		Texture_LoadTextureWhenNeeded(&tex);
		DWORD ttex = Texture_GetTexture(tex);
		if(ttex != CurTexture)
		{

 #if IF_RENDERSYS(HRENDERSYS_DX)
			pD3DDevice->SetTexture( 0, (LPDIRECT3DTEXTURE9)ttex );
 #elif IF_RENDERSYS(HRENDERSYS_GLES)
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, ((Image *)ttex)->texid);
 #elif IF_RENDERSYS(HRENDERSYS_GL)
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, ttex);
 #endif
			CurTexture = ttex;
		}

		*max_prim=VERTEX_BUFFER_SIZE / prim_type;
		return VertArray;
	}
	else return NULL;
#else
	return NULL;
#endif
}

void CALL HGE_Impl::Gfx_FinishBatch(int nprim)
{
	nPrim=nprim;
}

HTARGET CALL HGE_Impl::Target_Create(int width, int height, bool zbuffer)
{
	CRenderTargetList *pTarget;
	D3DSURFACE_DESC TDesc;

	pTarget = new CRenderTargetList;
	pTarget->pTex=0;
	pTarget->pDepth=0;

	/************************************************************************/
	/* This parameter in comment is changed by h5nc (h5nc@yahoo.com.cn)     */
	/************************************************************************/
#if IF_RENDERSYS(HRENDERSYS_DX)
	if(FAILED(D3DXCreateTexture(pD3DDevice, width, height, 1, D3DUSAGE_RENDERTARGET,
						/*d3dpp->BackBufferFormat*/D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pTarget->pTex)))
	{
		_PostError("Can't create render target texture");
		delete pTarget;
		return 0;
	}

	pTarget->pTex->GetLevelDesc(0, &TDesc);
	pTarget->width=TDesc.Width;
	pTarget->height=TDesc.Height;

	if(zbuffer)
	{
		//+5th +6th
		if(FAILED(pD3DDevice->CreateDepthStencilSurface(pTarget->width, pTarget->height,
						D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &pTarget->pDepth, NULL)))
		{
			pTarget->pTex->Release();
			_PostError("Can't create render target depth buffer");
			delete pTarget;
			return 0;
		}
	}

#elif IF_RENDERSYS(HRENDERSYS_GLS)
 #if IF_FRAMWORK(HFRAMEWORK_QT)
	/*
	QGLFramebufferObject * render_fbo = new QGLFramebufferObject(width, height);
	pTarget->width = render_fbo->size().width();
	pTarget->height = render_fbo->size().height();
	pTarget->pTex = render_fbo->texture();
	pTarget->pDepth = render_fbo;
	*/
	pTarget->pFrameBuffer = 0;
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &(pTarget->pTex));
	glBindTexture(GL_TEXTURE_2D, pTarget->pTex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, CurTexture);
	glDisable(GL_TEXTURE_2D);
	pTarget->width = width;
	pTarget->height = height;

	QGLFunctions funcs(QGLContext::currentContext());
	if (funcs.hasOpenGLFeature(QGLFunctions::Framebuffers))
	{
		funcs.glGenFramebuffers(1, (unsigned int*)(&pTarget->pFrameBuffer));
		funcs.glBindFramebuffer(GL_FRAMEBUFFER, pTarget->pFrameBuffer);
		if (zbuffer)
		{
			funcs.glGenRenderbuffers(1, (unsigned int*)&(pTarget->pDepth));
			funcs.glBindRenderbuffer(GL_RENDERBUFFER, pTarget->pDepth);
			funcs.glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
			funcs.glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}

		// attach texture
		funcs.glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pTarget->pTex, 0);
		// Attach to the FBO for depth
		funcs.glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, pTarget->pDepth); 

		funcs.glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
 #endif

#endif


	Texture_AddToList((HTEXTURE)pTarget->pTex, pTarget->width, pTarget->height);

	pTarget->next=pTargets;
	pTargets=pTarget;


	return (HTARGET)pTarget;
}

void CALL HGE_Impl::Target_Free(HTARGET target)
{
	CRenderTargetList *pTarget=pTargets, *pPrevTarget=NULL;

	while(pTarget)
	{
		if((CRenderTargetList *)target == pTarget)
		{
			Texture_Free((HTEXTURE)pTarget->pTex);

			if(pPrevTarget)
				pPrevTarget->next = pTarget->next;
			else
				pTargets = pTarget->next;

#if IF_RENDERSYS(HRENDERSYS_DX)
			if(pTarget->pTex) pTarget->pTex->Release();
			if(pTarget->pDepth) pTarget->pDepth->Release();
#elif IF_RENDERSYS(HRENDERSYS_GLS)
 #if IF_FRAMWORK(HFRAMEWORK_QT)
			QGLFunctions funcs(QGLContext::currentContext());
			if (funcs.hasOpenGLFeature(QGLFunctions::Framebuffers))
			{
				if (pTarget->pFrameBuffer)
				{
					funcs.glDeleteFramebuffers(1, &pTarget->pFrameBuffer);
				}
				else
				{
					// Error
				}
				if (pTarget->pDepth)
				{
					funcs.glDeleteRenderbuffers(1, &pTarget->pDepth);
				}
			}
 #endif
#endif
			delete pTarget;
			return;
		}

		pPrevTarget=pTarget;
		pTarget=pTarget->next;
	}
}

HTEXTURE CALL HGE_Impl::Target_GetTexture(HTARGET target)
{
	CRenderTargetList *targ=(CRenderTargetList *)target;
	if(target) return (HTEXTURE)((DWORD)targ->pTex);
	else return NULL;
}

HTEXTURE CALL HGE_Impl::Texture_Create(int width, int height)
{
#if IF_RENDERSYS(HRENDERSYS_DX)
	LPDIRECT3DTEXTURE9 pTex;

	if( FAILED( D3DXCreateTexture( pD3DDevice, width, height,
										1,					// Mip levels
										0,					// Usage
										D3DFMT_A8R8G8B8,	// Format
										D3DPOOL_MANAGED,	// Memory pool
										&pTex ) ) )
	{
		_PostError("Can't create texture");
		return NULL;
	}

	return (HTEXTURE)((DWORD)pTex);
#else

	return NULL;
#endif
}

HTEXTURE CALL HGE_Impl::Texture_Load(const char *filename, DWORD size, bool bMipmap)
{
	void *data;
	DWORD _size;

#if IF_RENDERSYS(HRENDERSYS_DX)
	D3DFORMAT fmt1, fmt2;
	LPDIRECT3DTEXTURE9 pTex;
	D3DXIMAGE_INFO info;
#endif


	if(size) { data=(void *)filename; _size=size; }
	else
	{
		data=pHGE->Resource_Load(filename, &_size);
		if(!data) return NULL;
	}

#if IF_RENDERSYS(HRENDERSYS_DX)
	if(*(DWORD*)data == 0x20534444) // Compressed DDS format magic number
	{
		fmt1=D3DFMT_UNKNOWN;
		fmt2=D3DFMT_A8R8G8B8;
	}
	else
	{
		fmt1=D3DFMT_A8R8G8B8;
		fmt2=D3DFMT_UNKNOWN;
	}

//	if( FAILED( D3DXCreateTextureFromFileInMemory( pD3DDevice, data, _size, &pTex ) ) ) pTex=NULL;
	if( FAILED( D3DXCreateTextureFromFileInMemoryEx( pD3DDevice, data, _size,
										D3DX_DEFAULT, D3DX_DEFAULT,
										bMipmap ? 0:1,		// Mip levels
										0,					// Usage
										fmt1,				// Format
										D3DPOOL_MANAGED,	// Memory pool
										D3DX_FILTER_NONE,	// Filter
										D3DX_DEFAULT,		// Mip filter
										0,			// Color key
										&info, NULL,
										&pTex ) ) )

	if( FAILED( D3DXCreateTextureFromFileInMemoryEx( pD3DDevice, data, _size,
										D3DX_DEFAULT, D3DX_DEFAULT,
										bMipmap ? 0:1,		// Mip levels
										0,					// Usage
										fmt2,				// Format
										D3DPOOL_MANAGED,	// Memory pool
										D3DX_FILTER_NONE,	// Filter
										D3DX_DEFAULT,		// Mip filter
										0,			// Color key
										&info, NULL,
										&pTex ) ) )

	{
		_PostError("Can't create texture");
		if(!size) Resource_Free(data);
		return NULL;
	}
#elif IF_RENDERSYS(HRENDERSYS_PSP|HRENDERSYS_GLES)
	Image * pTex = loadImageFromMemory((BYTE *)data, _size);
#elif IF_RENDERSYS(HRENDERSYS_GL)

 #if IF_FRAMWORK(HFRAMEWORK_QT)
	QImage qimg;
	qimg.loadFromData((BYTE*)data, _size);
	QImage qimg_gl = QGLWidget::convertToGLFormat(qimg);
	if (qimg_gl.isNull())
	{
		delete data;
		return NULL;
	}
	GLuint pTex;
 #endif // IF_FRAMWORK(HFRAMEWORK_QT)

#endif

	int twidth, theight;
#if IF_RENDERSYS(HRENDERSYS_DX)
	twidth = info.Width;
	theight = info.Height;
#elif IF_RENDERSYS(HRENDERSYS_GLES|HRENDERSYS_PSP)
	twidth = pTex->textureWidth;
	theight = pTex->textureHeight;
#elif IF_RENDERSYS(HRENDERSYS_GL)
 #if IF_FRAMWORK(HFRAMEWORK_QT)
	twidth = qimg_gl.width();
	theight = qimg_gl.height();
 #endif
#endif


#if IF_RENDERSYS(HRENDERSYS_GLS)

 #if IF_RENDERSYS(HRENDERSYS_GLES)
	glGenTextures(1, &pTex->texid);
	if (pTex->texid)
	{
		glBindTexture(GL_TEXTURE_2D, pTex->texid);
 #elif IF_RENDERSYS(HRENDERSYS_GL)
	glGenTextures(1, &pTex);
	if (pTex) {
		glBindTexture(GL_TEXTURE_2D, pTex);
 #endif
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
 #if IF_RENDERSYS(HRENDERSYS_GLES)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, twidth, theight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pTex->data);
 #elif IF_RENDERSYS(HRENDERSYS_GL)
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, twidth, theight, 0, GL_RGBA, GL_UNSIGNED_BYTE, qimg_gl.bits());
 #endif
		CurTexture = pTex;
	}
#endif //IF_RENDERSYS(HRENDERSYS_GLS)

	if(!size) Resource_Free(data);
	if (!pTex)
	{
		return NULL;
	}


	Texture_AddToList((HTEXTURE)((DWORD)pTex), twidth, theight);

	return (HTEXTURE)((DWORD)pTex);

}

bool CALL HGE_Impl::Texture_AddToList(HTEXTURE tex, int width, int height)
{
	CTextureList *texItem;
	texItem=new CTextureList;
	texItem->tex=tex;
	texItem->width=width;
	texItem->height=height;
	texItem->next=textures;
	textures=texItem;
	return true;
}

void CALL HGE_Impl::Texture_Free(HTEXTURE tex)
{
	DWORD ttex = Texture_GetTexture(tex);
#if IF_RENDERSYS(HRENDERSYS_DX)
	LPDIRECT3DTEXTURE9 pTex=(LPDIRECT3DTEXTURE9)ttex;
#elif IF_RENDERSYS(HRENDERSYS_GLES|HRENDERSYS_PSP)
	Image * pTex = (Image *)ttex;
#endif
	CTextureList *texItem=textures, *texPrev=0;

	while(texItem)
	{
		if(texItem->tex.tex==tex.tex)
		{
			if(texPrev) texPrev->next=texItem->next;
			else textures=texItem->next;
			delete texItem;
			break;
		}
		texPrev=texItem;
		texItem=texItem->next;
	}
#if IF_RENDERSYS(HRENDERSYS_DX)
	if(pTex != NULL) pTex->Release();
#elif IF_RENDERSYS(HRENDERSYS_GLES|HRENDERSYS_PSP)
	if (pTex)
	{
		freeImage(pTex);
	}
#elif IF_RENDERSYS(HRENDERSYS_GL)
	glDeleteTextures( 1, (const GLuint *)&ttex );
#endif
}

DWORD CALL HGE_Impl::Texture_GetTexture(HTEXTURE tex)
{
	return tex.GetTexture(nTexInfo, texInfo);
}

int CALL HGE_Impl::Texture_GetWidth(HTEXTURE tex, bool bOriginal)
{
	DWORD ttex = Texture_GetTexture(tex);
	if (!ttex)
	{
		return tex.GetTextureWidthByInfo(nTexInfo, texInfo);
	}
#if IF_RENDERSYS(HRENDERSYS_DX)
	D3DSURFACE_DESC TDesc;
	LPDIRECT3DTEXTURE9 pTex=(LPDIRECT3DTEXTURE9)ttex;
#endif
	CTextureList *texItem=textures;

#if IF_RENDERSYS(HRENDERSYS_GLS)
	bOriginal = true;
#endif
	if(bOriginal)
	{
		while(texItem)
		{
			if(texItem->tex.tex==tex.tex) return texItem->width;
			texItem=texItem->next;
		}
		return 0;
	}
#if IF_RENDERSYS(HRENDERSYS_DX)
	else
	{
		if(FAILED(pTex->GetLevelDesc(0, &TDesc))) return 0;
		else return TDesc.Width;
	}
#elif IF_RENDERSYS(HRENDERSYS_GLES|HRENDERSYS_PSP)
	Image * pTex = (Image *)ttex;
	return pTex->textureWidth;
#elif IF_RENDERSYS(HRENDERSYS_GL)
	return 0;
#endif
}

int CALL HGE_Impl::Texture_GetHeight(HTEXTURE tex, bool bOriginal)
{
	DWORD ttex = Texture_GetTexture(tex);
	if (!ttex)
	{
		return tex.GetTextureHeightByInfo(nTexInfo, texInfo);
	}
#if IF_RENDERSYS(HRENDERSYS_DX)
	D3DSURFACE_DESC TDesc;
	LPDIRECT3DTEXTURE9 pTex=(LPDIRECT3DTEXTURE9)ttex;
#endif
	CTextureList *texItem=textures;

#if IF_RENDERSYS(HRENDERSYS_GLS)
	bOriginal = true;
#endif
	if(bOriginal)
	{
		while(texItem)
		{
			if(texItem->tex.tex==tex.tex) return texItem->height;
			texItem=texItem->next;
		}
		return 0;
	}
#if IF_RENDERSYS(HRENDERSYS_DX)
	else
	{
		if(FAILED(pTex->GetLevelDesc(0, &TDesc))) return 0;
		else return TDesc.Height;
	}
#elif IF_RENDERSYS(HRENDERSYS_GLES|HRENDERSYS_PSP)
	Image * pTex = (Image *)ttex;
	return pTex->textureHeight;
#elif IF_RENDERSYS(HRENDERSYS_GL)
	return 0;
#endif
}

DWORD * CALL HGE_Impl::Texture_Lock(HTEXTURE tex, bool bReadOnly, int left, int top, int width, int height)
{
	Texture_LoadTextureWhenNeeded(&tex);
	DWORD ttex = Texture_GetTexture(tex);
#if IF_RENDERSYS(HRENDERSYS_DX)
	LPDIRECT3DTEXTURE9 pTex=(LPDIRECT3DTEXTURE9)ttex;
	D3DSURFACE_DESC TDesc;
	D3DLOCKED_RECT TRect;
	RECT region, *prec;
	int flags;

	pTex->GetLevelDesc(0, &TDesc);
	if(TDesc.Format!=D3DFMT_A8R8G8B8 && TDesc.Format!=D3DFMT_X8R8G8B8) return 0;

	if(width && height)
	{
		region.left=left;
		region.top=top;
		region.right=left+width;
		region.bottom=top+height;
		prec=&region;
	}
	else prec=0;

	if(bReadOnly) flags=D3DLOCK_READONLY;
	else flags=0;

	if(FAILED(pTex->LockRect(0, &TRect, prec, flags)))
	{
		_PostError("Can't lock texture");
		return NULL;
	}

	return (DWORD *)TRect.pBits;
#else
	return NULL;
#endif
}

void CALL HGE_Impl::Texture_Unlock(HTEXTURE tex)
{
	DWORD ttex = Texture_GetTexture(tex);
#if IF_RENDERSYS(HRENDERSYS_DX)
	LPDIRECT3DTEXTURE9 pTex=(LPDIRECT3DTEXTURE9)ttex;
	pTex->UnlockRect(0);
#endif
}

bool CALL HGE_Impl::Texture_Save(HTEXTURE tex, const char * filename, DWORD filetype)
{
	DWORD ttex = Texture_GetTexture(tex);
#if IF_RENDERSYS(HRENDERSYS_DX)
	LPDIRECT3DTEXTURE9 pTex=(LPDIRECT3DTEXTURE9)ttex;
	if (!pTex)
	{
		return false;
	}
	HRESULT hr = D3DXSaveTextureToFile(filename, (D3DXIMAGE_FILEFORMAT)filetype, pTex, NULL);
	if (FAILED(hr))
	{
		return false;
	}
#endif
	return true;
}

bool CALL HGE_Impl::Texture_LoadTextureWhenNeeded(HTEXTURE * tex)
{
	if (procLoadTextureFunc)
	{
		Texture_SetTextureToLoad(tex);
		bool bret = procLoadTextureFunc();
		Texture_SetTextureToLoad(NULL);
		return bret;
	}
	return false;
}

void CALL HGE_Impl::Texture_SetTextureToLoad(HTEXTURE * tex)
{
	textoload = tex;
}

HTEXTURE * CALL HGE_Impl::Texture_GetTextureToLoad()
{
	return textoload;
}

//////// Implementation ////////

void HGE_Impl::_render_batch(bool bEndScene)
{
#if IF_RENDERSYS(HRENDERSYS_DX|HRENDERSYS_GLS)
	if(VertArray)
	{
#if IF_RENDERSYS(HRENDERSYS_DX)
		pVB->Unlock();
#endif
		if(nPrim)
		{
			switch(CurPrimType)
			{
			case HGEPRIM_QUADS:
#if IF_RENDERSYS(HRENDERSYS_GLS)
//					glEnable(GL_TEXTURE_2D);
					glEnableClientState(GL_COLOR_ARRAY);
					glColorPointer(HGEPRIM_QUADS, GL_UNSIGNED_BYTE, sizeof(hgeVertex), &(VertArray[0].col));
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glTexCoordPointer(2, GL_FLOAT, sizeof(hgeVertex), &(VertArray[0].tx));
					glEnableClientState(GL_VERTEX_ARRAY);
					glVertexPointer(3, GL_FLOAT, sizeof(hgeVertex), &(VertArray[0].x));

					glDrawArrays(GL_TRIANGLE_STRIP, 0, HGEPRIM_QUADS*nPrim);
#else
					//+2nd
					pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, nPrim<<2, 0, nPrim<<1);
#endif
					break;

			case HGEPRIM_TRIPLES:
#if IF_RENDERSYS(HRENDERSYS_GLS)
#else
					pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, nPrim);
#endif
					break;

			case HGEPRIM_LINES:
#if IF_RENDERSYS(HRENDERSYS_GLS)
//					glDisable(GL_TEXTURE_2D);
					glEnableClientState(GL_COLOR_ARRAY);
					glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(hgeVertex), &(VertArray[0].col));
					glEnableClientState(GL_VERTEX_ARRAY);
					glVertexPointer(3, GL_FLOAT, sizeof(hgeVertex), &(VertArray[0].x));

					glDrawArrays(GL_LINES, 0, HGEPRIM_LINES*nPrim);
#else
					pD3DDevice->DrawPrimitive(D3DPT_LINELIST, 0, nPrim);
#endif
					break;

			case HGEPRIM_POINTS:
#if IF_RENDERSYS(HRENDERSYS_GLS)
//					glDisable(GL_TEXTURE_2D);
					glEnableClientState(GL_COLOR_ARRAY);
					glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(hgeVertex), &(VertArray[0].col));
					glEnableClientState(GL_VERTEX_ARRAY);
					glVertexPointer(3, GL_FLOAT, sizeof(hgeVertex), &(VertArray[0].x));

					glDrawArrays(GL_POINTS, 0, HGEPRIM_POINTS*nPrim);
#else
					pD3DDevice->DrawPrimitive(D3DPT_POINTLIST, 0, nPrim);
#endif
					break;
			}

			nPrim=0;
		}

#if IF_RENDERSYS(HRENDERSYS_GLS)
		if(bEndScene) {/*delete[] VertArray;*/ VertArray=0;}
#else
		if(bEndScene) VertArray = 0;
#endif

#if IF_RENDERSYS(HRENDERSYS_DX)
		else pVB->Lock( 0, 0, (void**)&VertArray, 0 );
#endif
	}
#endif
}

void HGE_Impl::_SetBlendMode(int blend)
{
#if IF_RENDERSYS(HRENDERSYS_DX)
	if((blend & BLEND_ALPHABLEND) != (CurBlendMode & BLEND_ALPHABLEND))
	{
		if(blend & BLEND_ALPHABLEND) pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		else pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	}

    if((blend & BLEND_ZWRITE) != (CurBlendMode & BLEND_ZWRITE))
	{
		if(blend & BLEND_ZWRITE) pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		else pD3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	}

	if((blend & BLEND_COLORADD) != (CurBlendMode & BLEND_COLORADD))
	{
		if(blend & BLEND_COLORADD) pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_ADD);
		else pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	}

#elif IF_RENDERSYS(HRENDERSYS_PSP)
	int toblend;
	switch (blend)
	{
	case BLEND_ALPHAADD:
		toblend = GU_TFX_ADD;
	default:
		toblend = GU_TFX_MODULATE;
		break;
	}
	sceGuTexFunc(toblend, GU_TCC_RGBA);

#elif IF_RENDERSYS(HRENDERSYS_GLS)

	glDisable(GL_ALPHA_TEST);
	glEnable(GL_BLEND); // Enable Blending

	if((blend & BLEND_ALPHABLEND) != (CurBlendMode & BLEND_ALPHABLEND))
	{
		if(blend & BLEND_ALPHABLEND)
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	//Alpha blending
		else
			glBlendFunc(GL_SRC_ALPHA, GL_ONE); //Addictive
	}

    if((blend & BLEND_ZWRITE) != (CurBlendMode & BLEND_ZWRITE))
	{
		if(blend & BLEND_ZWRITE)
		{
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
		}
	}

	if((blend & BLEND_COLORADD) != (CurBlendMode & BLEND_COLORADD))
	{
		if(blend & BLEND_COLORADD) glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
		else glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}

#endif

	CurBlendMode = blend;
}

void HGE_Impl::_SetProjectionMatrix(int width, int height)
{
	D3DXMATRIX tmp;
	Math_MatrixScaling(&matProj, 1.0f, -1.0f, 1.0f);
	Math_MatrixTranslation(&tmp, -0.5f, height+0.5f, 0.0f);
	Math_MatrixMultiply(&matProj, &matProj, &tmp);
	Math_MatrixOrthoOffCenterLH(&tmp, 0, (float)width, 0, (float)height, 0.0f, 1.0f);
	Math_MatrixMultiply(&matProj, &matProj, &tmp);
}

#if IF_RENDERSYS(HRENDERSYS_PSP)

static unsigned int staticOffset = 0;
static unsigned int getMemorySize(unsigned int width, unsigned int height, unsigned int psm)
{
	switch (psm)
	{
	case GU_PSM_T4:
		return (width * height) >> 1;

	case GU_PSM_T8:
		return width * height;

	case GU_PSM_5650:
	case GU_PSM_5551:
	case GU_PSM_4444:
	case GU_PSM_T16:
		return 2 * width * height;

	case GU_PSM_8888:
	case GU_PSM_T32:
		return 4 * width * height;

	default:
		return 0;
	}
}
void* getStaticVramBuffer(unsigned int width, unsigned int height, unsigned int psm)
{
	unsigned int memSize = getMemorySize(width,height,psm);
	void* result = (void*)staticOffset;
	staticOffset += memSize;

	return result;
}
void* getStaticVramTexture(unsigned int width, unsigned int height, unsigned int psm)
{
	void* result = getStaticVramBuffer(width,height,psm);
	return (void*)(((unsigned int)result) + ((unsigned int)sceGeEdramGetAddr()));
}

#endif // PSP

bool HGE_Impl::_GfxInit()
{
#if IF_RENDERSYS(HRENDERSYS_DX)
	static const char *szFormats[]={"UNKNOWN", "R5G6B5", "X1R5G5B5", "A1R5G5B5", "X8R8G8B8", "A8R8G8B8"};
	D3DADAPTER_IDENTIFIER9 AdID;
	D3DDISPLAYMODE Mode;
	D3DFORMAT Format=D3DFMT_UNKNOWN;
	UINT nModes, i;

// Init D3D

	/************************************************************************/
	/* This parameter is changed by h5nc (h5nc@yahoo.com.cn)                */
	/************************************************************************/
	pD3D=Direct3DCreate9(D3D_SDK_VERSION/* 120 */); // D3D_SDK_VERSION
	if(pD3D==NULL)
	{
		_PostError("Can't create D3D interface");
		return false;
	}

// Get adapter info

	//change 2nd
	pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, D3DADAPTER_DEFAULT, &AdID);
	System_Log("D3D Driver: %s",AdID.Driver);
	System_Log("Description: %s",AdID.Description);
	System_Log("Version: %d.%d.%d.%d",
			HIWORD(AdID.DriverVersion.HighPart),
			LOWORD(AdID.DriverVersion.HighPart),
			HIWORD(AdID.DriverVersion.LowPart),
			LOWORD(AdID.DriverVersion.LowPart));

// Set up Windowed presentation parameters

	if(FAILED(pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &Mode)) || Mode.Format==D3DFMT_UNKNOWN)
	{
		_PostError("Can't determine desktop video mode");
		if(bWindowed) return false;
	}

	ZeroMemory(&d3dppW, sizeof(d3dppW));

	d3dppW.BackBufferWidth  = nScreenWidth;
	d3dppW.BackBufferHeight = nScreenHeight;
	d3dppW.BackBufferFormat = Mode.Format;
	d3dppW.BackBufferCount  = 1;
	d3dppW.MultiSampleType  = D3DMULTISAMPLE_NONE;
	d3dppW.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dppW.hDeviceWindow    = hwnd;
	d3dppW.Windowed         = TRUE;

	/************************************************************************/
	/* This parameter is changed by h5nc (h5nc@yahoo.com.cn)                */
	/************************************************************************/
	d3dppW.SwapEffect		= D3DSWAPEFFECT_DISCARD;

	if(bZBuffer)
	{
		d3dppW.EnableAutoDepthStencil = TRUE;
		d3dppW.AutoDepthStencilFormat = D3DFMT_D16;
	}

	/************************************************************************/
	/* The following blocks are modified by h5nc (h5nc@yahoo.com.cn)        */
	/************************************************************************/
	if(nScreenBPP == 32)
	{
		nModes=pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8);
		for(i=0;i<nModes;i++)
		{
			pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT, D3DFMT_X8R8G8B8, i, &Mode);
			if(Mode.Width != (UINT)nScreenWidth || Mode.Height != (UINT)nScreenHeight) continue;
			if(_format_id(Mode.Format) > _format_id(Format)) Format=Mode.Format;
		}
	}
	else
	{
		D3DFORMAT tempFormat = D3DFMT_X1R5G5B5;
		nModes = pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_X1R5G5B5);
		if(!nModes)
		{
			nModes = pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, D3DFMT_R5G6B5);
			tempFormat = D3DFMT_R5G6B5;
		}
		for(i=0; i<nModes; i++)
		{
			pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT, tempFormat, i, &Mode);
			if(Mode.Width != (UINT)nScreenWidth || Mode.Height != (UINT)nScreenHeight) continue;
			if(_format_id(Mode.Format) > _format_id(Format)) Format=Mode.Format;
		}
	}

	ZeroMemory(&d3dppFS, sizeof(d3dppFS));

	d3dppFS.BackBufferWidth  = nScreenWidth;
	d3dppFS.BackBufferHeight = nScreenHeight;
	d3dppFS.BackBufferFormat = Format;
	d3dppFS.BackBufferCount  = 1;
	d3dppFS.MultiSampleType  = D3DMULTISAMPLE_NONE;
	d3dppFS.hDeviceWindow    = hwnd;
	d3dppFS.Windowed         = FALSE;

	/************************************************************************/
	/* This parameter is changed by h5nc (h5nc@yahoo.com.cn)                */
	/************************************************************************/
	d3dppFS.SwapEffect       = D3DSWAPEFFECT_DISCARD;
	d3dppFS.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;

	if(nHGEFPS==HGEFPS_VSYNC) d3dppFS.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	else					  d3dppFS.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if(bZBuffer)
	{
		d3dppFS.EnableAutoDepthStencil = TRUE;
		d3dppFS.AutoDepthStencilFormat = D3DFMT_D16;
	}

	d3dpp = bWindowed ? &d3dppW : &d3dppFS;

	if(_format_id(d3dpp->BackBufferFormat) < 4) nScreenBPP=16;
	else nScreenBPP=32;

	/************************************************************************/
	/* These blocks are added by h5nc (h5nc@yahoo.com.cn)                   */
	/************************************************************************/
// Get D3D caps
	D3DCAPS9 d3dcaps;
	if(FAILED(pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dcaps)))
	{
		_PostError("Can't get D3DCAPS");
		return false;
	}

	DWORD d3dvp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	if ((d3dcaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0 || d3dcaps.VertexShaderVersion < D3DVS_VERSION(1,1))
	{
		d3dvp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

// Create D3D Device

		/************************************************************************/
		/* This parameter is changed by h5nc (h5nc@yahoo.com.cn)                */
		/************************************************************************/
	if( FAILED( pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd,
								/*D3DCREATE_SOFTWARE_VERTEXPROCESSING,*/
                                  d3dvp,
                                  d3dpp, &pD3DDevice ) ) )
	{
		_PostError("Can't create D3D device");
		return false;
	}

	Gfx_SetTextureInfo(0);
#elif IF_RENDERSYS(HRENDERSYS_PSP)
	#define BUF_WIDTH (512)
	// Setup GU
 	//pspDebugScreenInit();
	// Setup GU
	void * m_drawbuf = getStaticVramBuffer(BUF_WIDTH,SCREEN_HEIGHT,GU_PSM_8888);
	void * m_displaybuf = getStaticVramBuffer(BUF_WIDTH,SCREEN_HEIGHT,GU_PSM_8888);
	void * m_zbuf = getStaticVramBuffer(BUF_WIDTH,SCREEN_HEIGHT,GU_PSM_4444);
	// setup GU
	sceGuInit();
	sceGuStart(GU_DIRECT, gulist);
	sceGuDrawBuffer(GU_PSM_8888,m_drawbuf,BUF_WIDTH);
	sceGuDispBuffer(SCREEN_WIDTH,SCREEN_HEIGHT,m_displaybuf,BUF_WIDTH);
	sceGuDepthBuffer(m_zbuf,BUF_WIDTH);
	sceGuOffset(2048 - (SCREEN_WIDTH/2), 2048 - (SCREEN_HEIGHT/2));
	sceGuViewport(2048, 2048, SCREEN_WIDTH, SCREEN_HEIGHT);
	// Scissoring
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	// Backface culling
	sceGuFrontFace(GU_CCW);
	sceGuDisable(GU_CULL_FACE);		// no culling in 2D
	// Depth test
	sceGuDisable(GU_DEPTH_TEST);
	sceGuDepthMask(GU_TRUE);		// disable z-writes
	// Color keying
	sceGuDisable(GU_COLOR_TEST);
	sceGuDisable(GU_ALPHA_TEST);
	sceGuDisable(GU_CLIP_PLANES);
	// Texturing
	sceGuEnable(GU_TEXTURE_2D);
	sceGuShadeModel(GU_SMOOTH);
	sceGuTexWrap(GU_REPEAT, GU_REPEAT);
	sceGuTexFilter(GU_LINEAR,GU_LINEAR);
	//sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
	sceGuTexEnvColor(0xFFFFFFFF);
	sceGuColor(0xFFFFFFFF);
	sceGuAmbientColor(0xFFFFFFFF);
	sceGuTexOffset(0.0f, 0.0f);
	sceGuTexScale(1.0f, 1.0f);
	// Blending
	sceGuEnable(GU_BLEND);
	sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
	sceGuTexFunc(GU_TFX_MODULATE,GU_TCC_RGBA);
	sceGuTexFilter(GU_LINEAR,GU_LINEAR);
	sceGuDisable(GU_DITHER);

	// Projection

	matProj.m[0][0] = SCREEN_HEIGHT/SCREEN_WIDTH;
	matProj.m[0][1] = 0.0f;
	matProj.m[0][2] = 0.0f;
	matProj.m[0][3] = 0.0f;
	matProj.m[1][0] = 0.0f;
	matProj.m[1][1] = -1.0f;
	matProj.m[1][2] = 0.0f;
	matProj.m[1][3] = 0.0f;
	matProj.m[2][0] = 0.0f;
	matProj.m[2][1] = 0.0f;
	matProj.m[2][2] = -1.0f;
	matProj.m[2][3] = -1.0f;
	matProj.m[3][0] = -SCREEN_HEIGHT/2;
	matProj.m[3][1] = SCREEN_HEIGHT/2;
	matProj.m[3][2] = SCREEN_HEIGHT/2;
	matProj.m[3][3] = SCREEN_HEIGHT/2;

	sceGumMatrixMode(GU_PROJECTION);
	sceGumLoadIdentity();
	sceGumMultMatrix((ScePspFMatrix4*)&matProj);

	sceGumMatrixMode(GU_VIEW);
	sceGumLoadIdentity();

	sceGumMatrixMode(GU_MODEL);
	sceGumLoadIdentity();

	sceGuClearColor( 0x0 );
	sceGuClear(GU_COLOR_BUFFER_BIT|GU_FAST_CLEAR_BIT);
	sceGuFinish();
	sceGuSync(0,0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(1);
#elif IF_RENDERSYS(HRENDERSYS_GLES)
 #if IF_PLATFORM(HPLATFORM_IOS)
	glViewport(2048, 2048, SCREEN_WIDTH, SCREEN_HEIGHT);
	// Scissoring
	glEnable(GL_SCISSOR_TEST);
	glScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	// Backface culling
	glFrontFace(GL_CCW);
	glDisable(GL_CULL_FACE);		// no culling in 2D
	// Depth test
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);		// disable z-writes
	// Color keying
	glDisable(GL_ALPHA_TEST);
	// Texturing
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_FILTER_CONTROL_EXT, GL_LINEAR);
	//sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
	// Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DITHER);

	// Projection

	matProj.m[0][0] = SCREEN_HEIGHT/SCREEN_WIDTH;
	matProj.m[0][1] = 0.0f;
	matProj.m[0][2] = 0.0f;
	matProj.m[0][3] = 0.0f;
	matProj.m[1][0] = 0.0f;
	matProj.m[1][1] = -1.0f;
	matProj.m[1][2] = 0.0f;
	matProj.m[1][3] = 0.0f;
	matProj.m[2][0] = 0.0f;
	matProj.m[2][1] = 0.0f;
	matProj.m[2][2] = -1.0f;
	matProj.m[2][3] = -1.0f;
	matProj.m[3][0] = -SCREEN_HEIGHT/2;
	matProj.m[3][1] = SCREEN_HEIGHT/2;
	matProj.m[3][2] = SCREEN_HEIGHT/2;
	matProj.m[3][3] = SCREEN_HEIGHT/2;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMultMatrixf((GLfloat*)&matProj);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	/*
	glClearColor( 0, 0, 0, 0 );
	glClear(GL_COLOR_BUFFER_BIT);
	glFinish();
	 */
 #endif
#elif IF_RENDERSYS(HRENDERSYS_GL)

	// Backface culling
	glFrontFace(GL_CCW);
	glDisable(GL_CULL_FACE);		// no culling in 2D
	// Depth test
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);		// disable z-writes
	// Color keying
	glDisable(GL_ALPHA_TEST);
	// Texturing
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
	glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
	// Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DITHER);

	_SetBlendMode(BLEND_NULL);
	
#endif

	_AdjustWindow();
#if IF_RENDERSYS(HRENDERSYS_DX)
	System_Log("Mode: %d x %d x %s\n",nScreenWidth,nScreenHeight,szFormats[_format_id(Format)]);
#else
	System_Log("Mode: %d x %d\n",nScreenWidth,nScreenHeight);
#endif
// Create vertex batch buffer

	VertArray=0;
	textures=0;

// Init all stuff that can be lost

	_SetProjectionMatrix(nScreenWidth, nScreenHeight);
	Math_MatrixIdentity(&matView);

	if(!_init_lost()) return false;

	Gfx_Clear(0);

	return true;
}
#if IF_RENDERSYS(HRENDERSYS_DX)
int HGE_Impl::_format_id(D3DFORMAT fmt)
{
	switch(fmt) {
		case D3DFMT_R5G6B5:		return 1;
		case D3DFMT_X1R5G5B5:	return 2;
		case D3DFMT_A1R5G5B5:	return 3;
		case D3DFMT_X8R8G8B8:	return 4;
		case D3DFMT_A8R8G8B8:	return 5;
		default:				return 0;
	}
}
#endif

void HGE_Impl::_AdjustWindow()
{
#if IF_PLATFORM(HPLATFORM_WIN)
	if (bOwnWindow)
	{
		RECT *rc;
		LONG style;

		if(bWindowed) {rc=&rectW; style=styleW; }
		else  {rc=&rectFS; style=styleFS; }
		SetWindowLong(hwnd, GWL_STYLE, style);

		style=GetWindowLong(hwnd, GWL_EXSTYLE);
		if(bWindowed)
		{
			SetWindowLong(hwnd, GWL_EXSTYLE, style & (~WS_EX_TOPMOST));
			SetWindowPos(hwnd, HWND_NOTOPMOST, rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top, SWP_FRAMECHANGED);
		}
		else
		{
			SetWindowLong(hwnd, GWL_EXSTYLE, style | WS_EX_TOPMOST);
			SetWindowPos(hwnd, HWND_TOPMOST, rc->left, rc->top, rc->right-rc->left, rc->bottom-rc->top, SWP_FRAMECHANGED);
		}
	}
#endif
}

//void HGE_Impl::_Resize(int width, int height)
void CALL HGE_Impl::Gfx_Resize(int width, int height)
{
	if(hwndParent)
	{
		//if(procFocusLostFunc) procFocusLostFunc();
#if IF_RENDERSYS(HRENDERSYS_DX)
		d3dppW.BackBufferWidth=width;
		d3dppW.BackBufferHeight=height;
#endif
		nScreenWidth=width;
		nScreenHeight=height;

		_SetProjectionMatrix(nScreenWidth, nScreenHeight);
		_GfxRestore();

		//if(procFocusGainFunc) procFocusGainFunc();
	}
	if (!bOwnWindow)
	{
#if IF_RENDERSYS(HRENDERSYS_GLS)
		nScreenWidth = width;
		nScreenHeight = height;

		_SetProjectionMatrix(nScreenHeight, nScreenWidth);
		_GfxRestore();

		glViewport(0, 0, nScreenWidth, nScreenHeight);
		glEnable(GL_SCISSOR_TEST);
		glScissor(0, 0, nScreenWidth, nScreenHeight);

		matProj.m[0][0] = (float)nScreenHeight/(float)nScreenWidth;
		matProj.m[0][1] = 0.0f;
		matProj.m[0][2] = 0.0f;
		matProj.m[0][3] = 0.0f;
		matProj.m[1][0] = 0.0f;
		matProj.m[1][1] = -1.0f;
		matProj.m[1][2] = 0.0f;
		matProj.m[1][3] = 0.0f;
		matProj.m[2][0] = 0.0f;
		matProj.m[2][1] = 0.0f;
		matProj.m[2][2] = -1.0f;
		matProj.m[2][3] = -1.0f;
		matProj.m[3][0] = -nScreenHeight/2.0f;
		matProj.m[3][1] = nScreenHeight/2.0f;
		matProj.m[3][2] = nScreenHeight/2.0f;
		matProj.m[3][3] = nScreenHeight/2.0f;

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMultMatrixf((GLfloat*)&matProj);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
#endif
	}
}

void HGE_Impl::_GfxDone()
{
	CRenderTargetList *target=pTargets, *next_target;

	while(textures)	Texture_Free(textures->tex);
#if IF_RENDERSYS(HRENDERSYS_DX)
	if(pScreenSurf) { pScreenSurf->Release(); pScreenSurf=0; }
	if(pScreenDepth) { pScreenDepth->Release(); pScreenDepth=0; }
#endif

	while(target)
	{
#if IF_RENDERSYS(HRENDERSYS_DX)
		if(target->pTex) target->pTex->Release();
		if(target->pDepth) target->pDepth->Release();
#else
 #if IF_FRAMWORK(HFRAMEWORK_QT)
		QGLFunctions funcs(QGLContext::currentContext());
		if (funcs.hasOpenGLFeature(QGLFunctions::Framebuffers))
		{
			if (target->pFrameBuffer)
			{
				funcs.glDeleteFramebuffers(1, &target->pFrameBuffer);
			}
			else
			{
				// Error
			}
			if (target->pDepth)
			{
				funcs.glDeleteRenderbuffers(1, &target->pDepth);
			}
		}
 #endif
#endif
		next_target=target->next;
		delete target;
		target=next_target;
	}
	pTargets=0;
#if IF_RENDERSYS(HRENDERSYS_DX)
	if(pIB)
	{
		//-2nd
		pD3DDevice->SetIndices(NULL);
		pIB->Release();
		pIB = NULL;
	}
	if(pVB)
	{
		if(VertArray) {	pVB->Unlock(); VertArray=0;	}
		//+2nd
		pD3DDevice->SetStreamSource( 0, pVB, 0, sizeof(hgeVertex) );
		pVB->Release();
		pVB = NULL;
	}
	if(pD3DDevice) { pD3DDevice->Release(); pD3DDevice=0; }
	if(pD3D) { pD3D->Release(); pD3D=0; }
#elif IF_RENDERSYS(HRENDERSYS_PSP)
	sceGuTerm();
#endif
}

bool HGE_Impl::_GfxRestore()
{
#if IF_RENDERSYS(HRENDERSYS_DX)
	CRenderTargetList *target=pTargets;

	//if(!pD3DDevice) return false;
	//if(pD3DDevice->TestCooperativeLevel() == D3DERR_DEVICELOST) return;

	if(pScreenSurf) pScreenSurf->Release();
	if(pScreenDepth) pScreenDepth->Release();

	while(target)
	{
		if(target->pTex) target->pTex->Release();
		if(target->pDepth) target->pDepth->Release();
		target=target->next;
	}

	/************************************************************************/
	/* Blocks marked with "Yuki" are originally her codes                   */
	/* h5nc (h5nc@yahoo.com.cn) copied these codes under her permission     */
	/************************************************************************/
	/* add by Yuki */
	// begin
	CFontList * listIterator = fontList;
	while (listIterator)
	{
		if (listIterator->font) ((ID3DXFont *)(listIterator->font))->OnLostDevice();
		listIterator = listIterator->next;
	}
    // end
	if(pIB)
	{
		pD3DDevice->SetIndices(NULL);
		pIB->Release();
		pIB = NULL;
	}
	if(pVB)
	{
		//+2nd
		pD3DDevice->SetStreamSource( 0, pVB, 0, sizeof(hgeVertex) );
		pVB->Release();
		pVB = NULL;
	}

	pD3DDevice->Reset(d3dpp);
	//if(!_init_lost()) return false; //Movedown

	/************************************************************************/
	/* Blocks marked with "Yuki" are originally her codes                   */
	/* h5nc (h5nc@yahoo.com.cn) copied these codes under her permission     */
	/************************************************************************/
	/* add by Yuki */
	// begin
	listIterator = fontList;
	while (listIterator)
	{
		if (listIterator->font) ((ID3DXFont *)(listIterator->font))->OnResetDevice();
		listIterator = listIterator->next;
	}
	// end

#endif
	if(!_init_lost()) return false;
	if(procGfxRestoreFunc) return procGfxRestoreFunc();

	return true;
}

bool HGE_Impl::_init_lost()
{
#if IF_RENDERSYS(HRENDERSYS_DX)
	CRenderTargetList *target=pTargets;

// Store render target

	pScreenSurf=0;
	pScreenDepth=0;

	pD3DDevice->GetRenderTarget(0, &pScreenSurf);
	pD3DDevice->GetDepthStencilSurface(&pScreenDepth);

	while(target)
	{
		if(target->pTex)
			D3DXCreateTexture(pD3DDevice, target->width, target->height, 1, D3DUSAGE_RENDERTARGET,
			/*d3dpp->BackBufferFormat*/D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &target->pTex);
		if(target->pDepth)
			pD3DDevice->CreateDepthStencilSurface(target->width, target->height,
						   D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &target->pDepth, NULL);
		target=target->next;
	}

// Create Vertex buffer

	if( FAILED (pD3DDevice->CreateVertexBuffer(VERTEX_BUFFER_SIZE*sizeof(hgeVertex),
                                              D3DUSAGE_WRITEONLY,
											  D3DFVF_HGEVERTEX,
                                              D3DPOOL_DEFAULT, &pVB, NULL)))
	{
		_PostError("Can't create D3D vertex buffer");
		return false;
	}

//	pD3DDevice->SetVertexShader( D3DFVF_HGEVERTEX );
	pD3DDevice->SetFVF( D3DFVF_HGEVERTEX );
	pD3DDevice->SetStreamSource( 0, pVB, 0, sizeof(hgeVertex) );

// Create and setup Index buffer

	if( FAILED( pD3DDevice->CreateIndexBuffer(VERTEX_BUFFER_SIZE*6/4*sizeof(WORD),
                                              D3DUSAGE_WRITEONLY,
											  D3DFMT_INDEX16,
                                              D3DPOOL_DEFAULT, &pIB, NULL) ) )
	{
		_PostError("Can't create D3D index buffer");
		return false;
	}

	WORD *pIndices, n=0;
	if( FAILED( pIB->Lock( 0, 0, (void**)&pIndices, 0 ) ) )
	{
		_PostError("Can't lock D3D index buffer");
		return false;
	}

	for(int i=0; i<VERTEX_BUFFER_SIZE/4; i++) {
		*pIndices++=n;
		*pIndices++=n+1;
		*pIndices++=n+2;
		*pIndices++=n+2;
		*pIndices++=n+3;
		*pIndices++=n;
		n+=4;
	}

	pIB->Unlock();
	pD3DDevice->SetIndices(pIB);

// Set common render states

	//pD3DDevice->SetRenderState( D3DRS_LASTPIXEL, FALSE );
	pD3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	pD3DDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
	pD3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
	pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	pD3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
	pD3DDevice->SetRenderState( D3DRS_ALPHAREF,        0x01 );
	pD3DDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

	pD3DDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	pD3DDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
	pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	pD3DDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

//	pD3DDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTEXF_POINT);
	pD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	if(bTextureFilter)
	{
//		pD3DDevice->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_LINEAR);
//		pD3DDevice->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_LINEAR);
		pD3DDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_LINEAR);
		pD3DDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_LINEAR);
	}
	else
	{
//		pD3DDevice->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTEXF_POINT);
//		pD3DDevice->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTEXF_POINT);
		pD3DDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_POINT);
		pD3DDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_POINT);
	}
	pD3DDevice->SetTransform(D3DTS_VIEW, &matView);
	pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);
#endif

	return true;
}