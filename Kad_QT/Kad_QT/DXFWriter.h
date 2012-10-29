#pragma once

#define DXFLAYER_BOUNDARY			1
#define DXFLAYER_TURNPOINT			2
#define DXFLAYER_CURVEPOINT			3
#define DXFLAYER_NOTCH				4
#define DXFLAYER_GRADEREF			5
#define DXFLAYER_MIRROR				6
#define DXFLAYER_GRAIN				7
#define DXFLAYER_INTERNAL			8
#define DXFLAYER_STRIPEREF			9
#define DXFLAYER_PLAIDREF			10
#define DXFLAYER_INTERNALCUTOUT		11
#define DXFLAYER_NUMERICALCONTROL	12
#define DXFLAYER_DRILLHOLE			13
#define DXFLAYER_SEWLINE			14
#define DXFLAYER_ANNOTATIONTEXT		15
#define DXFLAYER_TNOTCH				80
#define DXFLAYER_CASTLENOTCH		81
#define DXFLAYER_CHECKNOTCH			82
#define DXFLAYER_UNOTCH				83
#define DXFLAYER_QUALITYVALIDATIONC	84
#define DXFLAYER_INTERNALQUALITYC	85
#define DXFLAYER_CUTOUTQUALITYC		86
#define DXFLAYER_SEWLINEQUALITYC	87

class DXFWriter
{
public:
	static DXFWriter& getInstance() { static DXFWriter instance; return instance; }

private:
	DXFWriter();
	~DXFWriter();
	DXFWriter(DXFWriter const&);
	void operator=(DXFWriter const&);

public:

	bool SetBaseName(const char * basename=NULL,  float mul=1.0f);
	void SetTopY(float y=-1){topy=y;};
	bool WriteHeader();
	bool WriteTables();
	bool WriteEntitiesBegin();
	bool WriteLine(float x0, float y0, float x1, float y1, int layerID=DXFLAYER_SEWLINE);
	bool WriteFrameAndGrain(float xl, float yt, float w, float h);
	bool WriteEntitiesEnd();
	bool WriteEOF();

	QFile * GetDXF(){return pdxf;};

private:
	QString strbasename;
	QFile * pdxf;
	float topy;
	float fmul;
};

