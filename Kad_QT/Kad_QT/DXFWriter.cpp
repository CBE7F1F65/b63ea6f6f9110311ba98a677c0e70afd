#include "StdAfx.h"
#include "DXFWriter.h"
#include "StringManager.h"

#define _DXFTEXTYOFFSET	0.25f

DXFWriter::DXFWriter(void)
{
	fWidth = -1;
	fHeight = -1;
	fmul = 1.0f;
}


DXFWriter::~DXFWriter(void)
{
	SetBaseName();
}

bool DXFWriter::SetBaseName( const char * basename/*=NULL*/, float mul/*=1.0f*/ )
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

	QString strfilename = strbasename+".dxf";
	pdxf = new QFile(strfilename);
	
	strpiecename = QFileInfo(strfilename).completeBaseName();

	if (!pdxf->open(QIODevice::WriteOnly|QIODevice::Text))
	{
		delete pdxf;
		pdxf = NULL;
		return false;
	}
	fmul = mul;

	return true;
}

bool DXFWriter::WriteHeader()
{
	if (!pdxf)
	{
		return false;
	}
//	QString qsheader("0\nSECTION\n2\nHEADER\n9\n$ACADVER\n1\nAC1006\n9\n$INSBASE\n10\n0.0\n20\n0.0\n30\n0.0\n9\n$EXTMIN\n10\n737.117064473832\n20\n699.2814634313361\n30\n0.0\n9\n$EXTMAX\n10\n4093.223966378263\n20\n2251.315564440493\n30\n0.0\n9\n$LINMIN\n10\n0.0\n20\n0.0\n9\n$LINMAX\n10\n1000.0\n20\n1000.0\n0\nENDSEC\n");
	QString qsheader = QString("0\nSECTION\n  2\nHEADER\n  0\nENDSEC\n");
	QTextStream qts(pdxf);
	qts<<qsheader;
	return true;
}

bool DXFWriter::WriteTables()
{
	/*
	if (!pdxf)
	{
		return false;
	}
	QString qstables("0\nSECTION\n2\nTABLES\n0\nTABLE\n2\nVPORT\n0\nVPORT\n2\n*Active\n70\n0\n10\n0.0\n20\n0.0\n11\n1.0\n21\n1.0\n12\n2626.972541774842\n22\n1414.779425730669\n13\n0.0\n23\n0.0\n14\n10.0\n24\n10.0\n15\n10.0\n25\n10.0\n16\n0.0\n26\n0.0\n36\n1.0\n17\n0.0\n27\n0.0\n37\n0.0\n40\n2138.77279082633\n41\n2.185697808535178\n42\n50.0\n43\n0.0\n44\n0.0\n50\n0.0\n51\n0.0\n71\n0\n72\n1000\n73\n1\n74\n3\n75\n0\n76\n1\n77\n0\n78\n0\n0\nENDTAB\n0\nENDSEC\n");
	QTextStream qts(pdxf);
	qts<<qstables;
	*/
	return true;

}

bool DXFWriter::WriteBlocksBegin()
{
	if (!pdxf)
	{
		return false;
	}
	float fhc = fHeight/2*fmul;
	QString qsbb = QString("  0\nSECTION\n  2\nBLOCKS\n  0\nBLOCK\n  8\n0\n  2\n%1\n 70\n64\n 10\n0.0000\n 20\n0.0000\n  0\nTEXT\n  8\n1\n 10\n%2\n 20\n%3\n 40\n.25\n 50\n0.0000\n  1\nPiece Name: %1\n  7\nSTANDARD\n  0\nTEXT\n  8\n1\n 10\n%2\n 20\n%4\n 40\n.25\n 50\n0.0000\n  1\nQuantity: 0,1\n  7\nSTANDARD\n  0\nTEXT\n  8\n1\n 10\n%2\n 20\n%5\n 40\n.25\n 50\n0.0000\n  1\nANNOTATION: CUT\n  7\nSTANDARD\n  0\nTEXT\n  8\n1\n 10\n%2\n 20\n%6\n 40\n.25\n 50\n0.0000\n  1\nCATEGORY: SIZE\n  7\nSTANDARD\n  0\nTEXT\n  8\n1\n 10\n%2\n 20\n%7\n 40\n.25\n 50\n0.0000\n  1\nSIZE: NONE\n  7\nSTANDARD\n")
		.arg(strpiecename).arg(fWidth/2*fmul).arg(fhc).arg(fhc+_DXFTEXTYOFFSET).arg(fhc+_DXFTEXTYOFFSET*2).arg(fhc+_DXFTEXTYOFFSET*3).arg(fhc+_DXFTEXTYOFFSET*4);
	QTextStream qts(pdxf);
	qts<<qsbb;
	return true;
}

bool DXFWriter::WriteLine( float x0, float y0, float x1, float y1, int layerID/*=DXFLAYER_INTERNAL*/, int qualityID/*=DXFLAYER_INTERNALQUALITYC*/ )
{
	if (!pdxf)
	{
		return false;
	}
	/*
	if (fHeight >= 0)
	{
		y0 = fHeight - y0;
		y1 = fHeight - y1;
	}
	*/
	x0 *= fmul;
	y0 *= fmul;
	x1 *= fmul;
	y1 *= fmul;
	swap(x0, y0);
	swap(x1, y1);
	_LimitSmallNumbers(x0);
	_LimitSmallNumbers(y0);
	_LimitSmallNumbers(x1);
	_LimitSmallNumbers(y1);
	/*
	QString qsline("0\nLINE\n");
	qsline+="8\n"+QString::number(layerID)+"\n10\n"+QString::number(x0)+"\n20\n"+QString::number(y0)+"\n30\n0\n11\n"+QString::number(x1)+"\n21\n"+QString::number(y1)+"\n31\n0\n";
	*/
	QString qsline = QString("  0\nPOLYLINE\n  8\n%1\n 66\n1\n 70\n0\n  0\nVERTEX\n  8\n%1\n 10\n%4\n 20\n%5\n  0\nVERTEX\n  8\n%1\n 10\n%6\n 20\n%7\n  0\nSEQEND\n  8\n%1\n  0\nPOLYLINE\n  8\n%2\n 66\n1\n 70\n0\n  0\nVERTEX\n  8\n%2\n 10\n%4\n 20\n%5\n  0\nVERTEX\n  8\n%2\n 10\n%6\n 20\n%7\n  0\nSEQEND\n  8\n%2\n  0\nPOINT\n  8\n%3\n 10\n%4\n 20\n%5\n  0\nPOINT\n  8\n%3\n 10\n%6\n 20\n%7\n")
		.arg(layerID).arg(qualityID).arg(DXFLAYER_TURNPOINT).arg(x0).arg(y0).arg(x1).arg(y1);
	QTextStream qts(pdxf);
	qts<<qsline;
	return true;

}

bool DXFWriter::WriteBezier( BezierSublinesInfo &bsinfo, float xoffset, float yoffset, int layerID/*=DXFLAYER_INTERNAL*/, int qualityID/*=DXFLAYER_INTERNALQUALITYC*/ )
{
	if (!pdxf)
	{
		return false;
	}
	int nsub = bsinfo.GetSubPointsCount();
	if (nsub < 2)
	{
		return false;
	}

	float * xvals = (float *)malloc(sizeof(float)*nsub);
	float * yvals = (float *)malloc(sizeof(float)*nsub);

	for (int i=0; i<nsub; i++)
	{
		float tempx = (bsinfo.GetX(i)+xoffset)*fmul;
		float tempy = (bsinfo.GetY(i)+yoffset)*fmul;
		swap(tempx, tempy);
		_LimitSmallNumbers(tempx);
		_LimitSmallNumbers(tempy);
		xvals[i] = tempx;
		yvals[i] = tempy;
	}

	QString qslinedef = QString("  0\nPOLYLINE\n  8\n%1\n 66\n1\n 70\n0\n").arg(layerID);
	QString qslinevertexs;
	for (int i=0; i<nsub; i++)
	{
		qslinevertexs += QString("  0\nVERTEX\n  8\n%1\n 10\n%2\n 20\n%3\n").arg(layerID).arg(xvals[i]).arg(yvals[i]);
	}
	qslinevertexs += QString("  0\nSEQEND\n");

	QString qslinequality = QString("  0\nPOLYLINE\n  8\n%1\n 66\n1\n 70\n0\n").arg(qualityID);

	for (int i=0; i<nsub; i++)
	{
		qslinequality += QString("  0\nVERTEX\n  8\n%1\n 10\n%2\n 20\n%3\n").arg(qualityID).arg(xvals[i]).arg(yvals[i]);
	}
	qslinequality += QString("  0\nSEQEND\n");
	
	QString qspointbegin = QString("  0\nPOINT\n  8\n%1\n 10\n%2\n 20\n%3\n").arg(DXFLAYER_TURNPOINT).arg(xvals[0]).arg(yvals[0]);
	QString qslinecurvepoint;
	for (int i=1; i<nsub-1; i++)
	{
		qslinecurvepoint += QString("  0\nPOINT\n  8\n%1\n 10\n%2\n 20\n%3\n").arg(DXFLAYER_CURVEPOINT).arg(xvals[i]).arg(yvals[i]);
	}

	QString qspointend = QString("  0\nPOINT\n  8\n%1\n 10\n%2\n 20\n%3\n").arg(DXFLAYER_TURNPOINT).arg(xvals[nsub-1]).arg(yvals[nsub-1]);

	QTextStream qts(pdxf);
	qts<<qslinedef;
	qts<<qslinevertexs;
	qts<<qslinequality;
	qts<<qspointbegin;
	qts<<qslinecurvepoint;
	qts<<qspointend;
	return true;
}

bool DXFWriter::WriteBlocksEnd()
{
	if (!pdxf)
	{
		return false;
	}
	QString qsbe("  0\nENDBLK\n  0\nENDSEC\n");
	QTextStream qts(pdxf);
	qts<<qsbe;
	return true;
}

bool DXFWriter::WriteEntities()
{
	if (!pdxf)
	{
		return false;
	}
	StringManager * psm = &StringManager::getInstance();
	QString qse = QString("  0\nSECTION\n  2\nENTITIES\n  0\nTEXT\n  8\n1\n 10\n0.0000\n 20\n0.0000\n 40\n.25\n 50\n0.0000\n  1\nStyle Name: %1\n  7\nSTANDARD\n  0\nTEXT\n  8\n1\n 10\n0.0000\n 20\n0.1400\n 40\n.25\n 50\n0.0000\n  1\nCreation Date: %2\n  7\nSTANDARD\n  0\nTEXT\n  8\n1\n 10\n0.0000\n 20\n0.2800\n 40\n.25\n 50\n0.0000\n  1\nCreation Time: %3\n  7\nSTANDARD\n  0\nTEXT\n  8\n1\n 10\n0.0000\n 20\n0.4200\n 40\n.25\n 50\n0.0000\n  1\nAuthor: %4\n  7\nSTANDARD\n  0\nTEXT\n  8\n1\n 10\n0.0000\n 20\n0.5600\n 40\n.25\n 50\n0.0000\n  1\nSample Size: NONE\n  7\nSTANDARD\n  0\nTEXT\n  8\n1\n 10\n0.0000\n 20\n0.7000\n 40\n.25\n 50\n0.0000\n  1\nGrade Rule Table: RULIB\n  7\nSTANDARD\n  0\nTEXT\n  8\n1\n 10\n0.0000\n 20\n0.8400\n 40\n.25\n 50\n0.0000\n  1\nUnits: %5\n  7\nSTANDARD\n  0\nTEXT\n  8\n1\n 10\n0.0000\n 20\n0.9800\n 40\n.25\n 50\n0.0000\n  1\nCurve Tolerance: .006\n  7\nSTANDARD\n  0\nTEXT\n  8\n1\n 10\n0.0000\n 20\n1.1200\n 40\n.25\n 50\n0.0000\n  1\nASTM/D13Proposal 1 Version: D 6673-04\n  7\nSTANDARD\n  0\nINSERT\n  8\n1\n  2\n%1\n 10\n5.0000\n 20\n0.0000\n  0\nENDSEC\n")
		.arg(strpiecename).arg(QDate::currentDate().toString("MM_dd_yyyy")).arg(QTime::currentTime().toString()).arg(psm->GetAppIDName()).arg(psm->GetUnitName());
	QTextStream qts(pdxf);
	qts<<qse;
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

bool DXFWriter::WriteFrame(float fgrowth/*=0.0f*/)
{
	WriteLine(-fgrowth, -fgrowth, fWidth+fgrowth, -fgrowth, DXFLAYER_BOUNDARY, DXFLAYER_QUALITYVALIDATIONC);
	WriteLine(fWidth+fgrowth, -fgrowth, fWidth+fgrowth, fHeight+fgrowth, DXFLAYER_BOUNDARY, DXFLAYER_QUALITYVALIDATIONC);
	WriteLine(fWidth+fgrowth, fHeight+fgrowth, -fgrowth, fHeight+fgrowth, DXFLAYER_BOUNDARY, DXFLAYER_QUALITYVALIDATIONC);
	WriteLine(-fgrowth, fHeight+fgrowth, -fgrowth, -fgrowth, DXFLAYER_BOUNDARY, DXFLAYER_QUALITYVALIDATIONC);
	return true;
}

bool DXFWriter::WriteGrain()
{
	WriteLine(fWidth, 0, fWidth, fHeight, DXFLAYER_GRAIN, DXFLAYER_GRADEREF);
	return true;
}

void DXFWriter::SetPieceInfo( float w, float h, const char * piecename )
{
	fWidth = w;
	fHeight = h;
	if (piecename)
	{
		strpiecename = piecename;
	}
}
