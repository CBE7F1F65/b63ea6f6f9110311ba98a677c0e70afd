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
	QString qsbb = QString("  0\nSECTION\n  2\nBLOCKS\n  0\nBLOCK\n  8\n0\n  2\n%1\n 70\n64\n 10\n0.0000\n 20\n0.0000\n  0\nTEXT\n  8\n1\n 10\n%2\n 20\n%3\n 40\n.25\n 50\n0.0000\n  1\nPiece Name: %1\n  7\nSTANDARD\n  0\nTEXT\n  8\n1\n 10\n%2\n 20\n%4\n 40\n.25\n 50\n0.0000\n  1\nQuantity: 0,1\n  7\nSTANDARD\n  0\nTEXT\n  8\n1\n 10\n%2\n 20\n%5\n 40\n.25\n 50\n0.0000\n  1\nANNOTATION: CUT 1\n  7\nSTANDARD\n  0\nTEXT\n  8\n1\n 10\n%2\n 20\n%6\n 40\n.25\n 50\n0.0000\n  1\nCATEGORY: SIZE 6\n  7\nSTANDARD\n  0\nTEXT\n  8\n1\n 10\n%2\n 20\n%7\n 40\n.25\n 50\n0.0000\n  1\nSIZE: 6\n  7\nSTANDARD\n")
		.arg(strpiecename).arg(fWidth/2*fmul).arg(fhc).arg(fhc+_DXFTEXTYOFFSET).arg(fhc+_DXFTEXTYOFFSET*2).arg(fhc+_DXFTEXTYOFFSET*3).arg(fhc+_DXFTEXTYOFFSET*4);
	QTextStream qts(pdxf);
	qts<<qsbb;
	return true;
}

bool DXFWriter::WriteDirectLine( float x0, float y0, float x1, float y1, int layerID/*=DXFLAYER_INTERNAL*/, int qualityID/*=DXFLAYER_INTERNALQUALITYC*/ )
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

bool DXFWriter::WriteGLine( GLine * pLine, float xoffset, float yoffset, int layerID/*=DXFLAYER_INTERNAL*/, int qualityID/*=DXFLAYER_INTERNALQUALITYC*/ )
{
	if (!pdxf)
	{
		return false;
	}

	list<DXFVertex> lstvertexes;

	GBezierLine * pBezier = NULL;
	if (!pLine->isStraightLine())
	{
		pBezier = (GBezierLine *)pLine;
	}

	float beginx = 0;
	float beginy = 0;
	float endx = 0;
	float endy = 0;
	if (pBezier)
	{
		BezierSublinesInfo * pbsi = pBezier->getBSInfo();
		int nsub = pbsi->GetSubPointsCount();
		if (nsub < 2)
		{
			return false;
		}
		for (int i=0; i<nsub; i++)
		{
			float fProp = pBezier->CalculateProportion(pbsi->GetX(i), pbsi->GetY(i), i);
			float tempx = (pbsi->GetX(i)+xoffset)*fmul;
			float tempy = (pbsi->GetY(i)+yoffset)*fmul;
			swap(tempx, tempy);
			_LimitSmallNumbers(tempx);
			_LimitSmallNumbers(tempy);
			lstvertexes.push_back(DXFVertex(tempx, tempy, fProp));
		}
	}
	else
	{
		float tx0 = (pLine->GetBeginPoint()->getX()+xoffset)*fmul;
		float ty0 = (pLine->GetBeginPoint()->getY()+yoffset)*fmul;
		float tx1 = (pLine->GetEndPoint()->getX()+xoffset)*fmul;
		float ty1 = (pLine->GetEndPoint()->getY()+yoffset)*fmul;
		swap(tx0, ty0);
		swap(tx1, ty1);
		_LimitSmallNumbers(tx0);
		_LimitSmallNumbers(ty0);
		_LimitSmallNumbers(tx1);
		_LimitSmallNumbers(ty1);
		lstvertexes.push_back(DXFVertex(tx0, ty0, 0));
		lstvertexes.push_back(DXFVertex(tx1, ty1, 1));
	}
	beginx = lstvertexes.front().x;
	beginy = lstvertexes.front().y;
	endx = lstvertexes.back().x;
	endy = lstvertexes.back().x;

	list<GPoint *> * plstclingby = pLine->getClingBy();
	for (list<GPoint *>::iterator it=plstclingby->begin(); it!=plstclingby->end(); ++it)
	{
		if ((*it)->isNotch())
		{
			GNotch * pNotch = (GNotch *)(*it);
			GClingInfo * pcli = pNotch->getClingInfo();
			float fProp;
			pcli->CalculateClingProportion(&fProp);

			float tempx = (pNotch->getX()+xoffset)*fmul;
			float tempy = (pNotch->getY()+yoffset)*fmul;
			swap(tempx, tempy);
			_LimitSmallNumbers(tempx);
			_LimitSmallNumbers(tempy);

			int nAngle = pNotch->GetAngle();

			lstvertexes.push_back(DXFVertex(tempx, tempy, fProp, nAngle));
		}
	}

	lstvertexes.sort();

	QString qslinedef = QString("  0\nPOLYLINE\n  8\n%1\n 66\n1\n 70\n0\n").arg(layerID);
	QString qslinevertexs;

	for (list<DXFVertex>::iterator it=lstvertexes.begin(); it!=lstvertexes.end(); ++it)
	{
		qslinevertexs += QString("  0\nVERTEX\n  8\n%1\n 10\n%2\n 20\n%3\n").arg(layerID).arg(it->x).arg(it->y);
	}
	qslinevertexs += QString("  0\nSEQEND\n");

	QString qslinequality = QString("  0\nPOLYLINE\n  8\n%1\n 66\n1\n 70\n0\n").arg(qualityID);


	for (list<DXFVertex>::iterator it=lstvertexes.begin(); it!=lstvertexes.end(); ++it)
	{
		qslinequality += QString("  0\nVERTEX\n  8\n%1\n 10\n%2\n 20\n%3\n").arg(qualityID).arg(it->x).arg(it->y);
	}
	qslinequality += QString("  0\nSEQEND\n");

	QString qspointbegin = QString("  0\nPOINT\n  8\n%1\n 10\n%2\n 20\n%3\n").arg(DXFLAYER_TURNPOINT).arg(beginx).arg(beginy);
	QString qslinecurvepoint;
	for (list<DXFVertex>::iterator it=++lstvertexes.begin(); it!=--lstvertexes.end(); ++it)
	{
		int tlayerid = DXFLAYER_CURVEPOINT;
		if (it->fProp == 0 || it->fProp == 1)
		{
			tlayerid = DXFLAYER_TURNPOINT;
		}
		qslinecurvepoint += QString("  0\nPOINT\n  8\n%1\n 10\n%2\n 20\n%3\n").arg(tlayerid).arg(it->x).arg(it->y);
		if (it->nAngle != INT_MAX)
		{
			qslinecurvepoint += QString("  0\nPOINT\n  8\n%1\n 10\n%2\n 20\n%3\n").arg(DXFLAYER_NOTCH).arg(it->x).arg(it->y);
		}
	}

	QString qspointend = QString("  0\nPOINT\n  8\n%1\n 10\n%2\n 20\n%3\n").arg(DXFLAYER_TURNPOINT).arg(endx).arg(endy);



	QTextStream qts(pdxf);
	qts<<qslinedef;
	qts<<qslinevertexs;
	qts<<qslinequality;
	qts<<qspointbegin;
	qts<<qslinecurvepoint;
	qts<<qspointend;
	return true;

}

/*
bool DXFWriter::WriteBezier( GBezierLine * pBezier, float xoffset, float yoffset, int layerID/ *=DXFLAYER_INTERNAL* /, int qualityID/ *=DXFLAYER_INTERNALQUALITYC* / )
{
	if (!pdxf)
	{
		return false;
	}
	BezierSublinesInfo * pbsi = pBezier->getBSInfo();
	int nsub = pbsi->GetSubPointsCount();
	if (nsub < 2)
	{
		return false;
	}

	list<DXFVertex> lstvertexes;

	list<GPoint *> * plstclingby = pBezier->getClingBy();
	for (list<GPoint *>::iterator it=plstclingby->begin(); it!=plstclingby->end(); ++it)
	{
		if ((*it)->isNotch())
		{
			GNotch * pNotch = (GNotch *)(*it);
			GClingInfo * pcli = pNotch->getClingInfo();
			float fProp;
			pcli->CalculateClingProportion(&fProp);

			float tempx = (pNotch->getX()+xoffset)*fmul;
			float tempy = (pNotch->getY()+yoffset)*fmul;
			swap(tempx, tempy);
			_LimitSmallNumbers(tempx);
			_LimitSmallNumbers(tempy);

			int nAngle = pNotch->GetAngle();

			lstvertexes.push_back(DXFVertex(tempx, tempy, fProp, nAngle));
		}
	}
	for (int i=0; i<nsub; i++)
	{
		float fProp = pBezier->CalculateProportion(pbsi->GetX(i), pbsi->GetY(i), i);
		float tempx = (pbsi->GetX(i)+xoffset)*fmul;
		float tempy = (pbsi->GetY(i)+yoffset)*fmul;
		swap(tempx, tempy);
		_LimitSmallNumbers(tempx);
		_LimitSmallNumbers(tempy);
		lstvertexes.push_back(DXFVertex(tempx, tempy, fProp));
	}

	lstvertexes.sort();
	/ *
	float * xvals = (float *)malloc(sizeof(float)*nsub);
	float * yvals = (float *)malloc(sizeof(float)*nsub);

	for (int i=0; i<nsub; i++)
	{
		float tempx = (pbsi->GetX(i)+xoffset)*fmul;
		float tempy = (pbsi->GetY(i)+yoffset)*fmul;
		swap(tempx, tempy);
		_LimitSmallNumbers(tempx);
		_LimitSmallNumbers(tempy);
		xvals[i] = tempx;
		yvals[i] = tempy;
	}
	* /

	QString qslinedef = QString("  0\nPOLYLINE\n  8\n%1\n 66\n1\n 70\n0\n").arg(layerID);
	QString qslinevertexs;
	/ *
	for (int i=0; i<nsub; i++)
	{
		qslinevertexs += QString("  0\nVERTEX\n  8\n%1\n 10\n%2\n 20\n%3\n").arg(layerID).arg(xvals[i]).arg(yvals[i]);
	}
	* /
	for (list<DXFVertex>::iterator it=lstvertexes.begin(); it!=lstvertexes.end(); ++it)
	{
		qslinevertexs += QString("  0\nVERTEX\n  8\n%1\n 10\n%2\n 20\n%3\n").arg(layerID).arg(it->x).arg(it->y);
	}
	qslinevertexs += QString("  0\nSEQEND\n");

	QString qslinequality = QString("  0\nPOLYLINE\n  8\n%1\n 66\n1\n 70\n0\n").arg(qualityID);

	/ *for (int i=0; i<nsub; i++)
	{
		qslinequality += QString("  0\nVERTEX\n  8\n%1\n 10\n%2\n 20\n%3\n").arg(qualityID).arg(xvals[i]).arg(yvals[i]);
	}* /
	for (list<DXFVertex>::iterator it=lstvertexes.begin(); it!=lstvertexes.end(); ++it)
	{
		qslinequality += QString("  0\nVERTEX\n  8\n%1\n 10\n%2\n 20\n%3\n").arg(qualityID).arg(it->x).arg(it->y);
	}
	qslinequality += QString("  0\nSEQEND\n");
	
//	QString qspointbegin = QString("  0\nPOINT\n  8\n%1\n 10\n%2\n 20\n%3\n").arg(DXFLAYER_TURNPOINT).arg(xvals[0]).arg(yvals[0]);
	QString qslinecurvepoint;
	/ *
	for (int i=0; i<nsub; i++)
	{
		qslinecurvepoint += QString("  0\nPOINT\n  8\n%1\n 10\n%2\n 20\n%3\n").arg(DXFLAYER_CURVEPOINT).arg(xvals[i]).arg(yvals[i]);
	}
	* /
	for (list<DXFVertex>::iterator it=lstvertexes.begin(); it!=lstvertexes.end(); ++it)
	{
		qslinecurvepoint += QString("  0\nPOINT\n  8\n%1\n 10\n%2\n 20\n%3\n").arg(DXFLAYER_CURVEPOINT).arg(it->x).arg(it->y);
		if (it->nAngle != INT_MAX)
		{
			qslinecurvepoint += QString("  0\nPOINT\n  8\n%1\n 10\n%2\n 20\n%3\n").arg(DXFLAYER_NOTCH).arg(it->x).arg(it->y);
		}
	}

//	QString qspointend = QString("  0\nPOINT\n  8\n%1\n 10\n%2\n 20\n%3\n").arg(DXFLAYER_TURNPOINT).arg(xvals[nsub-1]).arg(yvals[nsub-1]);

	QTextStream qts(pdxf);
	qts<<qslinedef;
	qts<<qslinevertexs;
	qts<<qslinequality;
//	qts<<qspointbegin;
	qts<<qslinecurvepoint;
//	qts<<qspointend;
	return true;
}*/

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
	WriteDirectLine(-fgrowth, -fgrowth, fWidth+fgrowth, -fgrowth, DXFLAYER_BOUNDARY, DXFLAYER_QUALITYVALIDATIONC);
	WriteDirectLine(fWidth+fgrowth, -fgrowth, fWidth+fgrowth, fHeight+fgrowth, DXFLAYER_BOUNDARY, DXFLAYER_QUALITYVALIDATIONC);
	WriteDirectLine(fWidth+fgrowth, fHeight+fgrowth, -fgrowth, fHeight+fgrowth, DXFLAYER_BOUNDARY, DXFLAYER_QUALITYVALIDATIONC);
	WriteDirectLine(-fgrowth, fHeight+fgrowth, -fgrowth, -fgrowth, DXFLAYER_BOUNDARY, DXFLAYER_QUALITYVALIDATIONC);
	return true;
}

bool DXFWriter::WriteGrain()
{
	WriteDirectLine(fWidth/2, fHeight/4, fWidth/2, fHeight*3/4, DXFLAYER_GRAIN, DXFLAYER_GRADEREF);
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
