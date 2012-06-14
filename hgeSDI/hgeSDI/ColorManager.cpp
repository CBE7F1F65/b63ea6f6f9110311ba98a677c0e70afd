#include "StdAfx.h"
#include "ColorManager.h"
#include "GObject.h"

ColorManager::ColorManager(void)
{
	hge = hgeCreate(HGE_VERSION);
}

ColorManager::~ColorManager(void)
{
	if (hge)
	{
		hge->Release();
	}
}

DWORD ColorManager::GetLayerLineColor( int layer )
{
	/*
//	4f80ff	Light Blue
	ff4f4f	Light Red
	4fff4f	Green
//	4f4fff	Medium Blue
	ff4fff	Magenta
	4fffff	Cyan
	ffff4f	Yellow
	bababa	Light Gray
//	000000	Black
	ff6600	Orange
	005400	Dark Green
	009999	Teal
	cc9966	Tan
	993300	Brown
	9933ff	Violet
	ff9900	Gold
//	000087	Dark Blue
	ff99cc	Pink
	9999ff	Lavender
	990000	Brick Red
	666600	Olive
	ff9999	Peach
	990033	Burgundy
	99cc00	Grass Green
	996600	Ochre
	660066	Purple
	808080	Gray

	ffffff	White
	*/
	switch (layer%24)
	{
	case 0: return 0xffafafaf;	// White
	case 1: return 0xffff4f4f;	// Light Red
	case 2: return 0xff4fff4f;	// Green
	case 3: return 0xffff4fff;	// Magenta
	case 4: return 0xff4fffff;	// Cyan
	case 5: return 0xffffff4f;	// Yellow
	case 6: return 0xffbababa;	// Light Gray
	case 7: return 0xffff6600;	// Orange
	case 8: return 0xff005400;	// Dark Green
	case 9: return 0xff009999;	// Teal
	case 10: return 0xffcc9966;	// Tan
	case 11: return 0xff993300;	// Brown
	case 12: return 0xff9933ff;	// Violet
	case 13: return 0xffff9900;	// Gold
	case 14: return 0xffff99cc;	// Pink
	case 15: return 0xff9999ff;	// Lavender
	case 16: return 0xff990000;	// Brick Red
	case 17: return 0xff666600;	// Olive
	case 18: return 0xffff9999;	// Peach
	case 19: return 0xff990033;	// Burgundy
	case 20: return 0xff99cc00;	// Grass Green
	case 21: return 0xff996600;	// Ochre
	case 22: return 0xff660066;	// Purple
	case 23: return 0xff808080;	// Gray
	}
	return 0xffffffff;
}

DWORD ColorManager::GetTextColor( int type, int state )
{
	if (type == COLORMT_LIST)
	{
		switch (state)
		{
		case COLORMS_DISABLED:
			return 0xff717171;
		}
	}
	return 0xffffffff;
}

DWORD ColorManager::GetTextBkColor( int type, int state )
{
	if (type == COLORMT_LIST)
	{
		switch (state)
		{
		case COLORMS_ACTIVE:
			return 0xff596678;
		case COLORMS_NONACTIVE:
			return 0xff494949;
		case COLORMS_BACKGROUND:
			return 0xff464646;
		case COLORMS_FRAME:
			return 0xff333333;
		case COLORMS_NONEEDITABLE:
			return 0xff494949;
		case COLORMS_SUBSTANCE:
			return 0xff595959;
		case COLORMS_DISABLED:
			return 0xff535353;
		}
	}
	return 0xffffffff;
}

DWORD ColorManager::ARGBToABGR( DWORD col )
{
	return ARGB(GETA(col), GETB(col), GETG(col), GETR(col));
}

void ColorManager::Init()
{
}

DWORD ColorManager::Highlight( DWORD col, int iHighlightLevel )
{
	float a, h, s, l;
	ARGBToAHSL(col, &a, &h, &s, &l);

	switch (iHighlightLevel)
	{
	case HIGHLIGHTLEVEL_NONE:
		break;
	case HIGHLIGHTLEVEL_SELECTED:
		a = 1;
		h += 0.2f;
//		l += 0.2f;
		s += 0.2f;
		if (h > 1)
		{
			h -= 1;
		}
		if (l > 1)
		{
			l = 1;
		}
		if (s > 1)
		{
			s = 1;
		}
		break;
	case HIGHLIGHTLEVEL_PICKED:
		a = 1;
		l += 0.2f;
		s += 0.2f;
		if (l > 1)
		{
			l = 1;
		}
		if (s > 1)
		{
			s = 1;
		}
		break;
	}

	return AHSLToARGB(a, h, s, l);
}

void ColorManager::ARGBToAHSL( DWORD col, float *a, float *h, float *s, float *l )
{
	float r = GETR(col)/255.0f;
	float g = GETG(col)/255.0f;
	float b = GETB(col)/255.0f;
	if (a)
	{
		*a = GETA(col)/255.0f;
	}

	float fmax = max(max(r, g), b);
	float fmin = min(min(r, g), b);
	float fmid = (fmax+fmin)/2.0f;

	float hf;
	float sf;
	float lf = fmid;
	float diff = fmax-fmin;

	if (fmax == fmin)
	{
		hf = sf = 0;
	}
	else
	{
		sf = (lf > 0.5f) ? (diff / (2-fmax-fmin)) : (diff / (fmax + fmin));

		if (fmax == r)
		{
			hf = (g-b)/diff + (g<b?6:0);
		}
		else if (fmax == g)
		{
			hf = (b-r)/diff + 2;
		}
		else
		{
			hf = (r-g)/diff + 4;
		}
		hf/=6.0f;
	}


	if (h)
	{
		*h = hf;
	}
	if (s)
	{
		*s = sf;
	}
	if (l)
	{
		*l = lf;
	}
}

DWORD ColorManager::AHSLToARGB( float a, float h, float s, float l )
{
	float r, g, b;
	if (s == 0.0f)
	{
		r=g=b=l;
	}
	else
	{       
		float q = (l < 0.5) ? (l * (1 + s)) : (l + s - l * s);
		float p = 2 * l - q;
		r = HueToRGB(p, q, h + 1/3.0f);
		g = HueToRGB(p, q, h);
		b = HueToRGB(p, q, h - 1/3.0f);
	}

	return ARGB(a*255, r*255, g*255, b*255);
}

float ColorManager::HueToRGB( float p, float q, float t )
{       
	if(t < 0) t += 1;
	if(t > 1) t -= 1;
	if(t < 1/6.0f) return p + (q - p) * 6 * t;
	if(t < 1/2.0f) return q;
	if(t < 2/3.0f) return p + (q - p) * (2/3.0f - t) * 6;
	return p;

}