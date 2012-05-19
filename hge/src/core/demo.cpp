/*
** Haaf's Game Engine 1.8
** Copyright (C) 2003-2007, Relish Games
** hge.relishgames.com
**
** Core functions implementation: HGE splash screen
*/


#include "hge_impl.h"

#ifdef DEMO

hgeQuad dquad;
float dtime;

unsigned short hgelogo[]={
	0x5089,0x474E,0x0A0D,0x0A1A,0x0000,0x0D00,0x4849,0x5244,
	0x0000,0x8000,0x0000,0x8000,0x0308,0x0000,0xF400,0x91E0,
	0x00F9,0x0000,0x6704,0x4D41,0x0041,0xAF00,0x37C8,0x8A05,
	0x00E9,0x0000,0x7419,0x5845,0x5374,0x666F,0x7774,0x7261,
	0x0065,0x6441,0x626F,0x2065,0x6D49,0x6761,0x5265,0x6165,
	0x7964,0xC971,0x3C65,0x0000,0x3000,0x4C50,0x4554,0x8F8F,
	0x298F,0x2929,0xFFFF,0xEEFF,0xEEEE,0xCECE,0xBACE,0xBABA,
	0x7575,0xAA75,0xAAAA,0x4444,0x5544,0x5555,0xF8F8,0xE0F8,
	0xE0E0,0x6565,0x1065,0x1010,0x0000,0xD600,0xD6D6,0x531D,
	0x9976,0x0000,0x3008,0x4449,0x5441,0xDA78,0xE062,0x601B,
	0x1000,0x0C40,0xED03,0x8000,0x1A00,0x0770,0x0400,0x80D0,
	0x003B,0x8020,0xDC06,0x0001,0x3401,0x0EE0,0x0800,0x01A0,
	0x0077,0x0040,0xB80D,0x0003,0x6802,0x1DC0,0x1000,0x0340,
	0x00EE,0x0080,0x701A,0x0007,0xD004,0x3B80,0x2000,0x9080,
	0xC01D,0xC3C8,0xCEC0,0xCACA,0xC2C2,0x334F,0xC2C0,0xCAC2,
	0xCECA,0xC3C0,0x6488,0x4029,0x2100,0xC01C,0xCEC1,0x3744,
	0xCEC0,0xB701,0x2016,0xE080,0x600E,0x6663,0x6262,0x6061,
	0xE4E3,0x60E0,0x21A4,0xE0E0,0x64E0,0x6063,0x6261,0x6662,
	0xD983,0x100B,0x3040,0x7007,0x3330,0x72B1,0x2BD2,0x39DE,
	0x9959,0x6198,0x0061,0x4010,0x0730,0x33B0,0xF2B1,0x2FD2,
	0xF1E5,0x32B2,0x43B1,0x0099,0x0401,0x0075,0x1323,0x2713,
	0xD33D,0x273E,0x1313,0x2534,0x0402,0xD410,0x3C01,0x2C4C,
	0xCDF4,0x2C7D,0x3C4C,0x0610,0x0040,0x1D41,0xC0C0,0xE744,
	0x0102,0x216E,0x0040,0xC031,0x0092,0x7D1B,0xC01D,0x4B06,
	0x0004,0x0401,0x0075,0x7D2B,0x0093,0x1128,0x42B0,0x0018,
	0x0401,0x0077,0x7D07,0xC01D,0x7301,0x4000,0x3100,0xD2C0,
	0x5904,0xE00E,0x1605,0xAC2B,0xC040,0xC6C0,0xB241,0xA003,
	0x1EA9,0x8020,0x0EE0,0x9440,0xACCF,0xC050,0x5483,0xC04B,
	0x90C4,0x714B,0x1E3E,0xA456,0x95D2,0x0199,0x8E45,0x0715,
	0x9B80,0x7308,0x4000,0x4100,0xC01D,0xE48F,0x9800,0x6C91,
	0xEEC8,0x0285,0x8F24,0xB232,0x97A0,0xC8F0,0x18E9,0x2D57,
	0x7080,0x3F00,0x0184,0x4010,0x3B64,0x0780,0xE98B,0xBC2C,
	0x3BA4,0x2000,0xC880,0x0075,0x560F,0x59E3,0x7748,0x4000,
	0x9100,0x00E9,0x6646,0xE6EC,0x9033,0x00EC,0x0080,0xD322,
	0xAC01,0x2CB8,0x2460,0x01D5,0x0100,0x9E44,0x1803,0x06E1,
	0x8372,0x379A,0x0C8C,0x4118,0xB440,0x0003,0x8802,0x073C,
	0xA030,0x3B27,0x9244,0x45E0,0x8435,0x0D95,0x6015,0x0038,
	0x8020,0x73C8,0x3300,0x887A,0x9C23,0x83C4,0x10D3,0x0014,
	0x0077,0x0040,0xE591,0x7800,0xB00C,0x1923,0x1A88,0x4407,
	0x003B,0x8020,0x72C8,0x0F00,0x779A,0x8041,0x161B,0x24E6,
	0x003A,0x8020,0x72C8,0x1B00,0x8296,0x9611,0x982F,0x7449,
	0x4000,0x9100,0x00E5,0xF476,0x8D34,0x089C,0x7448,0x4000,
	0x9100,0x00E5,0x2C56,0x600E,0xD323,0x0001,0x4401,0x0735,
	0xA0F0,0x77D6,0x3B44,0x2000,0xA880,0x00E6,0x4034,0x03B4,
	0x0200,0xC068,0x001D,0x4010,0x1C78,0xC0C0,0x0701,0x646C,
	0x003B,0x10C9,0x6020,0x74C4,0x4000,0xE100,0x0071,0x4036,
	0x03B2,0x0150,0xA62B,0x0003,0x6802,0x1DC0,0x1000,0x0340,
	0x00EE,0x0080,0x86A2,0xB003,0xB855,0x3B44,0x2000,0x0680,
	0x01DC,0x0100,0xE034,0x000E,0x2008,0x0E3C,0x6A40,0x304B,
	0xED90,0xD400,0x0906,0xA60F,0x0003,0x8802,0x051A,0x3E11,
	0x1007,0x882C,0x0200,0xC068,0x001D,0x4010,0x70D4,0xB800,
	0xE20E,0xD326,0x0001,0x3401,0xD5E0,0x4031,0x0D00,0x03B8,
	0x0200,0xC068,0x001D,0x4010,0x3994,0x9780,0x0772,0x0400,
	0x6510,0x426D,0xCA0E,0x001D,0x4010,0xB594,0x398A,0x5B31,
	0xA4C5,0x003A,0x8020,0xEB28,0x3017,0x0F20,0x91FC,0x00E7,
	0x0080,0xACA2,0xC467,0xD98D,0x2233,0x01D5,0x0100,0x6144,
	0x90DF,0x2303,0x9009,0x00EA,0x0080,0xB0A2,0xCC77,0x88CC,
	0x001E,0x3AA4,0x2000,0xC880,0x0073,0x6207,0x8C70,0x750D,
	0x007C,0x01DD,0x38DC,0xC947,0x0EA0,0x0800,0x6A20,0x908F,
	0x6FE0,0x600F,0xA019,0x0800,0x7220,0x88C7,0x8D30,0x6666,
	0xCB21,0x0001,0x4401,0x28EE,0x1B19,0xD186,0xAC6C,0x3964,
	0x2000,0xC880,0x271E,0x4064,0x879F,0x23E2,0x01CF,0x0100,
	0xFE44,0x2948,0xFA0F,0x2518,0x0E79,0x0800,0xF220,0x801D,
	0xF69C,0x93C1,0xE460,0x0039,0x8020,0x38B0,0x8480,0x72C1,
	0x361E,0xD650,0xE362,0x7D24,0x0BBE,0x00EE,0x0080,0xC8A2,
	0x1401,0xB800,0x0003,0x6802,0x1DC0,0x1000,0x0340,0x00EE,
	0x0080,0x701A,0x0007,0xD004,0x3B80,0x2000,0x0680,0x01DC,
	0x0100,0xE034,0x000E,0xA008,0x7701,0x4000,0x0D00,0x03B8,
	0x0200,0xEA08,0x2E00,0xBA26,0x2E03,0xCD88,0x0100,0xE034,
	0x000E,0xA008,0x7701,0x4000,0x0D00,0x03B8,0x0200,0x9088,
	0x4003,0x9D23,0xCC3C,0xCC4C,0xBC3C,0x1C7C,0x2C2C,0x0C1C,
	0x0140,0x2616,0x4E36,0xF026,0x29F8,0x900B,0x64E0,0x0261,
	0x1C51,0x2C0C,0x9510,0xB6A0,0x5801,0x9998,0x8D83,0x9B89,
	0x8183,0x0390,0x0200,0x6210,0x38E5,0xC000,0x0820,0x6D63,
	0x526D,0xFFFD,0x8B73,0x01F1,0x1C0E,0x2323,0x0630,0xEC48,
	0x25B8,0xAD87,0x3D80,0x09C8,0x73A1,0x9600,0x120F,0x025A,
	0x0E2B,0x2BEF,0x4E8A,0xDBB8,0xB914,0x00A6,0x005F,0xA031,
	0x34F5,0x1D30,0x6A00,0x0099,0x05AD,0x4635,0x8098,0x3016,
	0x1D02,0x6C00,0x8211,0x649B,0xC0AC,0x0846,0xB310,0xDB82,
	0x2022,0x0192,0x106C,0x0584,0x18A4,0x3918,0x4770,0x7401,
	0x056A,0x8020,0x7188,0x0F00,0xCC2F,0x1C01,0x4C8C,0x8C7C,
	0xDC1C,0x3720,0xB0F0,0x72B2,0x6C43,0x0162,0xC4DA,0xCACA,
	0x750C,0xD40D,0x9C01,0xAB40,0x1839,0x35F1,0xA1DF,0x000E,
	0x2008,0xD406,0x2729,0x0E2C,0x60E0,0x0704,0xD83C,0x6C01,
	0x2C7C,0x6C7C,0x208C,0xC00B,0x597A,0xAE81,0xB601,0x590D,
	0x0321,0x4845,0x800E,0xE174,0xB019,0xDEB5,0xD099,0x95FA,
	0x0100,0x80C4,0xB7D6,0x92C5,0x1806,0x9958,0x0E60,0xF200,
	0x80F8,0x05FE,0x17BA,0x051A,0x34A0,0x8DC0,0x37F0,0xC58C,
	0xCAC0,0xB208,0x8F9B,0x8F01,0xA003,0xDD13,0x0100,0x5F04,
	0x82C4,0x0A27,0xE098,0x600E,0x8605,0x3707,0x271F,0x131F,
	0x6931,0x8080,0xA003,0x8E73,0x0100,0x00C4,0x535B,0xDF82,
	0xD001,0xC05C,0xC7CD,0xC9C8,0x6E04,0x8324,0x00D2,0xDC0B,
	0x0C01,0x04E0,0xF501,0x9137,0x0021,0xDC1D,0x0800,0x0620,
	0xFC3E,0x0089,0x00EA,0x1E66,0xB5A0,0xA02C,0x3556,0x1330,
	0x9C82,0x6E0B,0x83AC,0xC01C,0x4A0D,0x900A,0x0104,0x555B,
	0x6201,0x32F1,0x7010,0x6C00,0x0168,0x8020,0x7018,0xB576,
	0x0C68,0xBD60,0x800E,0x8200,0xB20F,0xD3B0,0x7ED7,0xACF8,
	0x4033,0x2100,0x7946,0xB8D8,0x67E9,0x373D,0xD7A2,0x1005,
	0xC840,0xC84B,0xD938,0xB859,0x9999,0x68B9,0x980A,0xB999,
	0xD859,0x5791,0x016D,0xD004,0x2F80,0x056A,0xA008,0x7701,
	0x4000,0x0D00,0x03B8,0x0200,0xC068,0x001D,0x4010,0xEE03,
	0x8000,0x1A00,0x0770,0x0400,0x80D0,0x003B,0x8020,0xDC06,
	0x0001,0x3401,0x0EE0,0x0800,0xB820,0x1803,0x1791,0x6230,
	0x0955,0x7E6C,0x17C0,0x3234,0x52B0,0x01CB,0x0100,0x80C4,
	0x0818,0xB023,0x96AE,0x0995,0x01C9,0x4CAC,0x72D4,0x4000,
	0x2600,0x2DAD,0x6005,0x8418,0x4291,0x8583,0xF7A5,0xEE3F,
	0x3B12,0x7F58,0xBE0C,0x9F82,0xB9A2,0x6807,0x17A5,0xD289,
	0x8F16,0x144A,0xF585,0x3339,0xA8FB,0x9836,0xF4F3,0x081C,
	0x172C,0x2A1B,0x9531,0x7A53,0x78FA,0xDFCD,0xBA86,0xB65F,
	0x1668,0xB9F3,0x92E7,0xBEA0,0x3002,0x0669,0x002B,0x2080,
	0x250C,0x12A7,0xFF44,0xBBFF,0x1739,0x71D4,0xC130,0x6F9B,
	0x6FB2,0xF001,0xA748,0x6C00,0x9ADD,0x5A54,0x8EF1,0x9044,
	0xE7C6,0xFF46,0xA3E8,0x50DC,0xC80A,0x29C6,0xB80A,0x8481,
	0x58D2,0xAE34,0x1F53,0x50CD,0x9103,0x5730,0x8EA3,0xC617,
	0x008F,0x7242,0x0300,0xB82F,0x0735,0xCB6C,0x4070,0x6039,
	0x3007,0x65C2,0x0E40,0x7200,0x8138,0x4276,0xB194,0x7131,
	0x3822,0x38A0,0x2984,0x0903,0x5543,0x4282,0x978B,0x2C05,
	0x3208,0x6E05,0xCC10,0x805E,0x4200,0x0349,0xD648,0xB370,
	0x1D42,0xC1C0,0xE507,0x4340,0x9B80,0x1399,0x0692,0x40D8,
	0xE00E,0x2480,0x601D,0x0C97,0x01C5,0xC02C,0x1BEE,0x2554,
	0xEE34,0x26C0,0x7361,0x4000,0xE100,0x0072,0x642B,0x1DAE,
	0x00E2,0x0730,0x0FAA,0x59E8,0x5E56,0x0754,0x0230,0x78AB,
	0x0754,0x8030,0x1BE2,0x12A2,0x6096,0x3971,0x2000,0x7080,
	0x8039,0x0F8D,0xE6BC,0xE206,0x3000,0x1A07,0x1C02,0x3C8C,
	0x96C0,0x8A2A,0xB803,0x7899,0x50D9,0x001D,0x0652,0x0955,
	0x0076,0xD730,0x72E2,0x4000,0x6100,0x803A,0x1D93,0x03D4,
	0xE763,0x6264,0x1607,0x200D,0x1095,0x540E,0x981F,0x0AC3,
	0x0077,0x1B1B,0x1B48,0x0F58,0x03B2,0xC240,0x9510,0x9CC0,
	0x340B,0x1B92,0x082C,0x0532,0x01C3,0x0100,0xEE84,0x6000,
	0x068A,0xF32D,0x0766,0x60A5,0xCE50,0xA904,0x8764,0x7C26,
	0x3EB0,0x7006,0x8662,0x86BA,0xA819,0x0D98,0xC424,0xEC8A,
	0x5E00,0xF06E,0x3DF0,0x2558,0x6828,0xD9C0,0x978B,0x6C05,
	0x860A,0x0003,0x0802,0x28BD,0xE466,0x6361,0xC907,0xB282,
	0x423C,0x0710,0x9787,0x1C13,0x7C02,0xE88C,0x5C23,0x1C1C,
	0x2AC8,0x2179,0xA82B,0x3838,0x0DB0,0x03AA,0x1004,0x03BA,
	0x5138,0x2796,0x1A13,0x62EA,0x4C46,0x91DC,0x000B,0x0802,
	0x32A3,0xE5E2,0x6520,0x3CA1,0x5F76,0x0291,0x0200,0xC068,
	0x436B,0x0080,0x701A,0x0007,0xD004,0x3B80,0x2000,0x0680,
	0x01DC,0x0100,0xE034,0x000E,0xA008,0x7701,0x4000,0x0D00,
	0x03B8,0x0200,0xC068,0x001D,0x4010,0xEE03,0x8000,0x1A00,
	0x0770,0x0400,0x0018,0x0BED,0xD38C,0x676F,0xBE6E,0x0000,
	0x0000,0x4549,0x444E,0x42AE,0x8260
};

void DInit() {
	int x=pHGE->System_GetState(HGE_SCREENWIDTH)/2;
	int y=pHGE->System_GetState(HGE_SCREENHEIGHT)/2;
	
	dquad.tex=pHGE->Texture_Load((char *)hgelogo, sizeof(hgelogo));
	dquad.blend=BLEND_DEFAULT;

	for(int i=0; i<4; i++) {
		dquad.v[i].z=0.5f;
		dquad.v[i].col=0xFFFFFFFF;
	}

	dquad.v[0].tx=0.0f; dquad.v[0].ty=0.0f;
	dquad.v[1].tx=1.0f; dquad.v[1].ty=0.0f;
	dquad.v[2].tx=1.0f; dquad.v[2].ty=1.0f;
	dquad.v[3].tx=0.0f; dquad.v[3].ty=1.0f;

	dquad.v[0].x=x-64.0f; dquad.v[0].y=y-64.0f;
	dquad.v[1].x=x+64.0f; dquad.v[1].y=y-64.0f;
	dquad.v[2].x=x+64.0f; dquad.v[2].y=y+64.0f;
	dquad.v[3].x=x-64.0f; dquad.v[3].y=y+64.0f;

	dtime=0.0f;
}

void DDone() {
	pHGE->Texture_Free(dquad.tex);
}

bool DFrame() {
	BYTE alpha;
	DWORD col;

	dtime+=pHGE->Timer_GetDelta();

	if(dtime<0.25) alpha=(BYTE)((dtime*4)*0xFF);
	else if(dtime<1.0) alpha=0xFF;
	else if(dtime<1.25) alpha=(BYTE)((1.0f-(dtime-1.0f)*4)*0xFF);
	else return true;

	col=0xFFFFFF | (alpha<<24);
	dquad.v[0].col=col;
	dquad.v[1].col=col;
	dquad.v[2].col=col;
	dquad.v[3].col=col;

	pHGE->Gfx_BeginScene();
	pHGE->Gfx_Clear(0);
	pHGE->Gfx_RenderQuad(&dquad);
	pHGE->Gfx_EndScene();

	return false;
}

#endif