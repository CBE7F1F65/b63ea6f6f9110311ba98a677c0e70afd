#include "StdAfx.h"
#include "MarkingObject.h"
#include "GUICoordinate.h"
#include "MarkingManager.h"
#include "RenderHelper.h"
#include "qmaininterface.h"

void MarkingUI::UseUI()
{
	DeleteUI();
    pWidget = new QTUI_MarkingFloating_Widget(QMainInterface::getInstance().GetPGLView());
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
#define _MARKLENGTH_OFFSETLENGTH	48

MarkingObject::MarkingObject()
{
	pTargetObj = NULL;
	nMarkFlag = -1;
	nModifyVersion = -1;
}

MarkingObject::~MarkingObject()
{
	muiPositionX.DeleteUI();
	muiPositionY.DeleteUI();
	muiLength.DeleteUI();
	muiAngle.DeleteUI();
}

void MarkingObject::SetValue( GObject * pObj, int nFlag )
{
	ASSERT(pObj);
	pTargetObj = pObj;
	nMarkFlag = nFlag;

	if (nFlag & MARKFLAG_POSITION)
	{
		muiPositionX.UseUI();
		muiPositionY.UseUI();
	}
	if (nFlag & MARKFLAG_LENGTH)
	{
		muiLength.UseUI();
	}
	if (nFlag & MARKFLAG_ANGLE)
	{
		muiAngle.UseUI();
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

		muiPositionX.SetPosition(PointF2D(pLine->getX(), pLine->getY()));
		muiPositionY.SetPosition(PointF2D(pLine->getX(), pLine->getY()));

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
		float xdiff = l*cosf(ARC(angle));
		float ydiff = l*sinf(ARC(angle));
		ptLengthDiff.x = xdiff;
		ptLengthDiff.y = ydiff;

		QString str;
		if (nMarkFlag & MARKFLAG_POSITION)
		{
			str.sprintf("%f", pLine->GetMidPoint()->getX());
			muiPositionX.SetString(&str);
			str.sprintf("%f", pLine->GetMidPoint()->getY());
			muiPositionY.SetString(&str);
			muiPositionX.DoMove();
			muiPositionY.DoMove();
		}

		if (nMarkFlag & MARKFLAG_LENGTH)
		{
			muiLength.SetPosition(pLine->GetMidPoint()->GetPointF2D() + ptLengthDiff);
			str.sprintf("%f", pLine->getLength());
			muiLength.SetString(&str);
			muiLength.DoMove();
		}

		if (nMarkFlag & MARKFLAG_ANGLE)
		{
			fStraightLineLength = pmh->LineSegmentLength(pt1, pt2);
			ptArcPoint = PointF2D(pt1.x+fStraightLineLength, pt1.y);
			muiAngle.SetPosition(PointF2D(pt1.x+fStraightLineLength*cosf(ARC(nLineAngle/2)), pt1.y+fStraightLineLength*sinf(ARC(nLineAngle/2))));

			str.sprintf("%f", (float)(nLineAngle)/(ANGLEBASE_90/90));
			muiAngle.SetString(&str);
			muiAngle.DoMove();
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
