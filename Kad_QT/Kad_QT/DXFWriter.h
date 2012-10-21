#pragma once

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

	bool SetBaseName(const char * basename=NULL);
	void SetTopY(float y=-1){topy=y;};
	bool WriteHeader();
	bool WriteTables();
	bool WriteEntitiesBegin();
	bool WriteLine(float x0, float y0, float x1, float y1, int layerID=0);
	bool WriteEntitiesEnd();
	bool WriteEOF();

	QFile * GetDXF(){return pdxf;};

private:
	QString strbasename;
	QFile * pdxf;
	float topy;
};

