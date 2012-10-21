#include "StdAfx.h"
#include "DXFWriter.h"


DXFWriter::DXFWriter(void)
{
	strbasename = "";
	topy = -1;
}


DXFWriter::~DXFWriter(void)
{
	SetBaseName();
}

bool DXFWriter::SetBaseName( const char * basename/*=NULL*/ )
{
	if (pdxf)
	{
		pdxf->close();
		delete pdxf;
		pdxf = NULL;
	}

	if (!basename)
	{
		strbasename = "";
		return true;
	}

	strbasename = basename;

	pdxf = new QFile(strbasename+".dxf");
	if (!pdxf->open(QIODevice::WriteOnly|QIODevice::Text))
	{
		delete pdxf;
		pdxf = NULL;
		return false;
	}

	return true;
}

bool DXFWriter::WriteHeader()
{
	if (!pdxf)
	{
		return false;
	}
	QString qsheader("0\nSECTION\n2\nHEADER\n9\n$ACADVER\n1\nAC1006\n9\n$INSBASE\n10\n0.0\n20\n0.0\n30\n0.0\n9\n$EXTMIN\n10\n737.117064473832\n20\n699.2814634313361\n30\n0.0\n9\n$EXTMAX\n10\n4093.223966378263\n20\n2251.315564440493\n30\n0.0\n9\n$LINMIN\n10\n0.0\n20\n0.0\n9\n$LINMAX\n10\n1000.0\n20\n1000.0\n0\nENDSEC\n");
	QTextStream qts(pdxf);
	qts<<qsheader;
	return true;
}

bool DXFWriter::WriteTables()
{
	if (!pdxf)
	{
		return false;
	}
	QString qstables("0\nSECTION\n2\nTABLES\n0\nTABLE\n2\nVPORT\n0\nVPORT\n2\n*Active\n70\n0\n10\n0.0\n20\n0.0\n11\n1.0\n21\n1.0\n12\n2626.972541774842\n22\n1414.779425730669\n13\n0.0\n23\n0.0\n14\n10.0\n24\n10.0\n15\n10.0\n25\n10.0\n16\n0.0\n26\n0.0\n36\n1.0\n17\n0.0\n27\n0.0\n37\n0.0\n40\n2138.77279082633\n41\n2.185697808535178\n42\n50.0\n43\n0.0\n44\n0.0\n50\n0.0\n51\n0.0\n71\n0\n72\n1000\n73\n1\n74\n3\n75\n0\n76\n1\n77\n0\n78\n0\n0\nENDTAB\n0\nENDSEC\n");
	QTextStream qts(pdxf);
	qts<<qstables;
	return true;

}

bool DXFWriter::WriteEntitiesBegin()
{
	if (!pdxf)
	{
		return false;
	}
	QString qseb("0\nSECTION\n2\nENTITIES\n");
	QTextStream qts(pdxf);
	qts<<qseb;
	return true;
}

bool DXFWriter::WriteLine( float x0, float y0, float x1, float y1, int layerID/*=0*/ )
{
	if (!pdxf)
	{
		return false;
	}
	if (topy >= 0)
	{
		y0 = topy - y0;
		y1 = topy - y1;
	}
	QString qsline("0\nLINE\n");
	qsline+="8\n"+QString::number(layerID+1)+"\n10\n"+QString::number(x0)+"\n20\n"+QString::number(y0)+"\n30\n0\n11\n"+QString::number(x1)+"\n21\n"+QString::number(y1)+"\n31\n0\n";
	QTextStream qts(pdxf);
	qts<<qsline;
	return true;

}

bool DXFWriter::WriteEntitiesEnd()
{
	if (!pdxf)
	{
		return false;
	}
	QString qsee("0\nENDSEC\n");
	QTextStream qts(pdxf);
	qts<<qsee;
	return true;
}

bool DXFWriter::WriteEOF()
{
	if (!pdxf)
	{
		return false;
	}
	QString qseof("0\nEOF");
	QTextStream qts(pdxf);
	qts<<qseof;
	return true;

}
