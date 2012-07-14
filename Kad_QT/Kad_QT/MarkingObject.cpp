#include "StdAfx.h"
#include "MarkingObject.h"
#include "GUICoordinate.h"
#include "MarkingManager.h"
#include "RenderHelper.h"
#include "qmaininterface.h"

MarkingUI::MarkingUI( MarkingObject * p )
{
	pWidget = NULL;
	cb = NULL;
	ASSERT(p);
	pMarking = p;
	bEditable = false;
}

MarkingUI::~MarkingUI()
{
	DeleteUI();
}

void MarkingUI::UseUI()
{
	DeleteUI();
    pWidget = new QTUI_MarkingFloating_Widget(this, QMainInterface::getInstance().GetPGLView());
}

void MarkingUI::DeleteUI()
{
	if (pWidget)
	{
		delete pWidget;
		pWidget = NULL;
	}
}

void MarkingUI::DoMove()
{
	if (pWidget)
	{
		GUICoordinate * pguic = &GUICoordinate::getInstance();
		float x = pguic->CtoSx(ptPos.x);
		float y = pguic->CtoSy(ptPos.y);
		pWidget->MoveTo(x, y);
	}
}

void MarkingUI::SetString( QString * pStr )
{
	ASSERT(pStr);
	str = *pStr;
	if (pWidget)
	{
		pWidget->SetEditString(str);
	}
}

bool MarkingUI::DoCallback( bool bAccept )
{
	if (cb)
	{
		return cb(this, bAccept);
	}
	return false;
}

void MarkingUI::SetEditable( bool bSet )
{
	bEditable = bSet;
	pWidget->OnSetEditable(bEditable);
}

float MarkingUI::getFloat( bool * bOk/*=NULL*/ )
{
	float fval = 0;
	fval = str.toFloat(bOk);
	return fval;
}

int MarkingUI::getInt( bool *bOk/*=NULL*/ )
{
	int ival = 0;
	ival = str.toInt(bOk);
	return ival;
}

#define _MARKLENGTH_OFFSETLENGTH	48

MarkingObject::MarkingObject()
{
	pTargetObj = NULL;
	nMarkFlag = -1;
	nModifyVersion = -1;

	pmuiPositionX = new MarkingUI(this);
	pmuiPositionY = new MarkingUI(this);
	pmuiLength = new MarkingUI(this);
	pmuiAngle = new MarkingUI(this);
}

MarkingObject::~MarkingObject()
{
	if (pmuiPositionX)
	{
		delete pmuiPositionX;
	}
	if (pmuiPositionY)
	{
		delete pmuiPositionY;
	}
	if (pmuiLength)
	{
		delete pmuiLength;
	}
	if (pmuiAngle)
	{
		delete pmuiAngle;
	}
}

void MarkingObject::SetValue( GObject * pObj, int nFlag )
{
	ASSERT(pObj);
	pTargetObj = pObj;
	nMarkFlag = nFlag;

	if (nFlag & MARKFLAG_POSITIONX)
	{
		pmuiPositionX->UseUI();
	}
	if (nFlag & MARKFLAG_POSITIONY)
	{
		pmuiPositionY->UseUI();
	}
	if (nFlag & MARKFLAG_LENGTH)
	{
		pmuiLength->UseUI();
	}
	if (nFlag & MARKFLAG_ANGLE)
	{
		pmuiAngle->UseUI();
	}
}

void MarkingObject::Update()
{

}

void MarkingObject::Render()
{

}

void MarkingObject::SetRedraw()
{
	MarkingManager::getInstance().SetRedraw();
}

void MarkingObject::SetEditable( int nFlag, bool bSet )
{
	if (nFlag & MARKFLAG_POSITIONX)
	{
		pmuiPositionX->SetEditable(bSet);
	}
	if (nFlag & MARKFLAG_POSITIONY)
	{
		pmuiPositionY->SetEditable(bSet);
	}
	if (nFlag & MARKFLAG_LENGTH)
	{
		pmuiLength->SetEditable(bSet);
	}
	if (nFlag & MARKFLAG_ANGLE)
	{
		pmuiAngle->SetEditable(bSet);
	}
}

void MarkingObject::SetCallback( int nFlag, MarkingInputDoneCallback cb )
{
	if (nFlag & MARKFLAG_POSITIONX)
	{
		pmuiPositionX->SetCallback(cb);
	}
	if (nFlag & MARKFLAG_POSITIONY)
	{
		pmuiPositionY->SetCallback(cb);
	}
	if (nFlag & MARKFLAG_LENGTH)
	{
		pmuiLength->SetCallback(cb);
	}
	if (nFlag & MARKFLAG_ANGLE)
	{
		pmuiAngle->SetCallback(cb);
	}
}
/************************************************************************/
/* MARKINGLINE                                                          */
/************************************************************************/

MarkingLine::MarkingLine(GLine * pLine, int nFlag)
{
	ASSERT(pLine);
	SetValue(pLine, nFlag);
}

void MarkingLine::Update()
{
	if (nModifyVersion != pTargetObj->getModifyVersion())
	{
		nModifyVersion = pTargetObj->getModifyVersion();

		GLine * pLine = (GLine *)pTargetObj;

		pmuiPositionX->SetPosition(PointF2D(pLine->getX(), pLine->getY()));
		pmuiPositionY->SetPosition(PointF2D(pLine->getX(), pLine->getY()));

		MathHelper * pmh = &MathHelper::getInstance();

		PointF2D pt1 = pLine->GetBeginPoint()->GetPointF2D();
		PointF2D pt2 = pLine->GetEndPoint()->GetPointF2D();

		int angle = pmh->GetLineAngle(pt1, pt2);
		nLineAngle = angle;

		angle += ANGLEBASE_90;
		pmh->RestrictAngle(&angle);
		if (angle >= 0)
		{
			angle += ANGLEBASE_180;
		}

		float l = GUICoordinate::getInstance().StoCs(_MARKLENGTH_OFFSETLENGTH);
		float xdiff = l*cost(angle);
		float ydiff = l*sint(angle);
		ptLengthDiff.x = xdiff;
		ptLengthDiff.y = ydiff;

		QString str;
		if (nMarkFlag & MARKFLAG_POSITIONX)
		{
			str.sprintf("%f", pLine->GetMidPoint()->getX());
			pmuiPositionX->SetString(&str);
			pmuiPositionX->DoMove();
		}
		if (nMarkFlag & MARKFLAG_POSITIONY)
		{
			str.sprintf("%f", pLine->GetMidPoint()->getY());
			pmuiPositionY->SetString(&str);
			pmuiPositionY->DoMove();
		}

		if (nMarkFlag & MARKFLAG_LENGTH)
		{
			pmuiLength->SetPosition(pLine->GetMidPoint()->GetPointF2D() + ptLengthDiff);
			str.sprintf("%f", pLine->getLength());
			pmuiLength->SetString(&str);
			pmuiLength->DoMove();
		}

		if (nMarkFlag & MARKFLAG_ANGLE)
		{
			fStraightLineLength = pmh->LineSegmentLength(pt1, pt2);
			ptArcPoint = PointF2D(pt1.x+fStraightLineLength, pt1.y);
			pmuiAngle->SetPosition(PointF2D(pt1.x+fStraightLineLength*cost(nLineAngle/2), pt1.y+fStraightLineLength*sint(nLineAngle/2)));

			str.sprintf("%f", (float)(nLineAngle)/(ANGLEBASE_90/90));
			pmuiAngle->SetString(&str);
			pmuiAngle->DoMove();
		}

		SetRedraw();
	}
}

void MarkingLine::Render()
{
	if (nMarkFlag <= 0)
	{
		return;
	}
	RenderHelper * prh = &RenderHelper::getInstance();
	DWORD col = pTargetObj->getLineColor();
	col = SETA(col, ColorManager::getInstance().GetMarkingAlpha());

	int savedstyle = prh->getLineStyle();
	prh->SetLineStyle(RHLINESTYLE_DOTTEDLINE);
	GLine * pLine = (GLine *)pTargetObj;
	PointF2D ptBegin = pLine->GetBeginPoint()->GetPointF2D();
	PointF2D ptEnd = pLine->GetEndPoint()->GetPointF2D();
	if (nMarkFlag & MARKFLAG_LENGTH)
	{
		PointF2D ptBeginPlusOff = ptBegin + ptLengthDiff;
		PointF2D ptEndPlusOff = ptEnd + ptLengthDiff;

		prh->RenderLine(ptBegin.x, ptBegin.y, ptBeginPlusOff.x, ptBeginPlusOff.y, col);
		prh->RenderLine(ptEnd.x, ptEnd.y, ptEndPlusOff.x, ptEndPlusOff.y, col);
		if (pLine->isStraightLine())
		{
			prh->RenderLine(ptBeginPlusOff.x, ptBeginPlusOff.y, ptEndPlusOff.x, ptEndPlusOff.y, col);
		}
		else
		{
			GBezierLine * pBezier = (GBezierLine *)pLine;
			prh->RenderBezierByInfo(pBezier->getBSInfo(), col, &ptLengthDiff);
		}
	}
	if (nMarkFlag & MARKFLAG_ANGLE)
	{
		prh->RenderLine(ptBegin.x, ptBegin.y, ptArcPoint.x, ptArcPoint.y, col);
		if (!pLine->isStraightLine())
		{
			prh->RenderLine(ptBegin.x, ptBegin.y, ptEnd.x, ptEnd.y, col);
		}
		prh->RenderArc(ptBegin.x, ptBegin.y, fStraightLineLength, 0, nLineAngle, col);
	}
	prh->SetLineStyle(savedstyle);
}
