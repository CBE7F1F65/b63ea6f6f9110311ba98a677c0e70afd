#include "StdAfx.h"
#include "MarkingObject.h"
#include "GUICoordinate.h"
#include "MarkingManager.h"
#include "RenderHelper.h"
#include "qmaininterface.h"


MarkingUI::MarkingUI( MarkingObject * p, int typeflag )
{
	pWidget = NULL;
	cb = NULL;
	ASSERT(p);
	pMarking = p;
	bEditable = false;

	nTypeFlag = typeflag;
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

void MarkingUI::SetLock( bool bSet )
{
	if (bSet)
	{
		pWidget->LockValue();
	}
	else
	{
		pWidget->UnlockValue();
	}
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

void MarkingUI::OnDoneEdit( QString strEdit )
{
	str = strEdit;
}

bool MarkingUI::IsValueLocked()
{
	if (pWidget)
	{
		return pWidget->IsValueLocked();
	}
	return false;
}

#define _MARKLENGTH_OFFSETLENGTH	48

/************************************************************************/
/*                                                                      */
/************************************************************************/

#define MARKBUNCH_CALLFUNC(FLAG, FUNC)	\
	if ((FLAG)&MARKFLAG_POSITIONX)	\
	{	\
		pmuiPositionX->FUNC;	\
	}	\
	if ((FLAG)&MARKFLAG_POSITIONY)	\
	{	\
		pmuiPositionY->FUNC;	\
	}	\
	if ((FLAG)&MARKFLAG_LENGTH)	\
	{	\
		pmuiLength->FUNC;	\
	}	\
	if ((FLAG)&MARKFLAG_ANGLE)	\
	{	\
		pmuiAngle->FUNC;	\
	}	\
	if ((FLAG)&MARKFLAG_SPLITLENGTH_B)	\
	{	\
		pmuiSplitB->FUNC;	\
	}	\
	if ((FLAG)&MARKFLAG_SPLITLENGTH_E)	\
	{	\
		pmuiSplitE->FUNC;	\
	}


/************************************************************************/
/*                                                                      */
/************************************************************************/
MarkingObject::MarkingObject()
{
	pTargetObj = NULL;
	nMarkFlag = -1;
	nModifyVersion = -1;

	pmuiPositionX = new MarkingUI(this, MARKFLAG_POSITIONX);
	pmuiPositionY = new MarkingUI(this, MARKFLAG_POSITIONY);
	pmuiLength = new MarkingUI(this, MARKFLAG_LENGTH);
	pmuiAngle = new MarkingUI(this, MARKFLAG_ANGLE);
	pmuiSplitB = new MarkingUI(this, MARKFLAG_SPLITLENGTH_B);
	pmuiSplitE = new MarkingUI(this, MARKFLAG_SPLITLENGTH_E);
}

MarkingObject::~MarkingObject()
{
	if (pmuiPositionX) { delete pmuiPositionX; }
	if (pmuiPositionY) { delete pmuiPositionY; }
	if (pmuiLength) { delete pmuiLength; }
	if (pmuiAngle) { delete pmuiAngle; }
	if (pmuiSplitB) { delete pmuiSplitB; }
	if (pmuiSplitE) { delete pmuiSplitE; }
}

void MarkingObject::SetValue( GObject * pObj, int nFlag )
{
	ASSERT(pObj);
	pTargetObj = pObj;
	nMarkFlag = nFlag;

	MARKBUNCH_CALLFUNC(nFlag, UseUI());
}

#undef MARKBUNCH_CALLFUNC

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

MarkingUI * MarkingObject::getMarkingUI( int nFlag )
{
	switch (nFlag)
	{
	case MARKFLAG_POSITIONX:
		return pmuiPositionX;
	case MARKFLAG_POSITIONY:
		return pmuiPositionY;
	case MARKFLAG_LENGTH:
		return pmuiLength;
	case MARKFLAG_ANGLE:
		return pmuiAngle;
	case MARKFLAG_SPLITLENGTH_B:
		return pmuiSplitB;
	case MARKFLAG_SPLITLENGTH_E:
		return pmuiSplitE;
	}
	ASSERT(true);
	return NULL;
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

		PointF2D ptBegin = pLine->GetBeginPoint()->GetPointF2D();
		PointF2D ptEnd = pLine->GetEndPoint()->GetPointF2D();

		int angle = pmh->GetLineAngle(ptBegin, ptEnd);
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
			fStraightLineLength = pmh->LineSegmentLength(ptBegin, ptEnd);
			ptArcPoint = PointF2D(ptBegin.x+fStraightLineLength, ptBegin.y);
			pmuiAngle->SetPosition(PointF2D(ptBegin.x+fStraightLineLength*cost(nLineAngle/2), ptBegin.y+fStraightLineLength*sint(nLineAngle/2)));

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

MarkingSplitLine::MarkingSplitLine( GLine * pLine, int nFlag )
{
	ASSERT(pLine);
	SetValue(pLine, nFlag);
}

void MarkingSplitLine::Update()
{
	bool bRecalcPosition = false;
	GLine * pLine = (GLine *)pTargetObj;
	if (nModifyVersion != pTargetObj->getModifyVersion())
	{
		nModifyVersion = pTargetObj->getModifyVersion();
		bRecalcPosition = true;


		MathHelper * pmh = &MathHelper::getInstance();

		PointF2D ptBegin = pLine->GetBeginPoint()->GetPointF2D();
		PointF2D ptEnd = pLine->GetEndPoint()->GetPointF2D();

		fLineLength = pLine->getLength();
		int angle = pmh->GetLineAngle(ptBegin, ptEnd);

		angle += ANGLEBASE_90;
		pmh->RestrictAngle(&angle);
		if (angle >= 0)
		{
			angle += ANGLEBASE_180;
		}

		float l = GUICoordinate::getInstance().StoCs(_MARKLENGTH_OFFSETLENGTH);
		float xdiff = l*cost(angle);
		float ydiff = l*sint(angle);
		ptPosDiff.x = xdiff;
		ptPosDiff.y = ydiff;
	}

	if (bRecalcPosition || bSplitPointChanged)
	{
		float fProp = pLine->CalculateProportion(ptSplitPoint.x, ptSplitPoint.y, iSplitSec);

		float bx, by, ex, ey;
		pLine->GetPositionAtProportion(fProp/2, &bx, &by);
		pLine->GetPositionAtProportion((1+fProp)/2, &ex, &ey);

		QString str;

		if (nMarkFlag & MARKFLAG_SPLITLENGTH_B)
		{
			pmuiSplitB->SetPosition(PointF2D(bx, by)+ptPosDiff);
			str.sprintf("%f(%f%%)", fProp*fLineLength, fProp*100);
			pmuiSplitB->SetString(&str);
			pmuiSplitB->DoMove();
		}
		if (nMarkFlag & MARKFLAG_SPLITLENGTH_E)
		{
			pmuiSplitE->SetPosition(PointF2D(ex, ey)+ptPosDiff);
			str.sprintf("%f(%f%%)", (1-fProp)*fLineLength, (1-fProp)*100);
			pmuiSplitE->SetString(&str);
			pmuiSplitE->DoMove();
		}

		SetRedraw();
	}
	bSplitPointChanged = false;
}

void MarkingSplitLine::Render()
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

	if (nMarkFlag & MARKFLAG_SPLITLENGTH)
	{
		PointF2D ptBeginPlusOff = ptBegin + ptPosDiff;
		PointF2D ptEndPlusOff = ptEnd + ptPosDiff;
		PointF2D ptSplitPlusOff = ptSplitPoint + ptPosDiff;

		prh->RenderLine(ptBegin.x, ptBegin.y, ptBeginPlusOff.x, ptBeginPlusOff.y, col);
		prh->RenderLine(ptEnd.x, ptEnd.y, ptEndPlusOff.x, ptEndPlusOff.y, col);
		prh->RenderLine(ptSplitPoint.x, ptSplitPoint.y, ptSplitPlusOff.x, ptSplitPlusOff.y, col);
		if (pLine->isStraightLine())
		{
			prh->RenderLine(ptBeginPlusOff.x, ptBeginPlusOff.y, ptEndPlusOff.x, ptEndPlusOff.y, col);
		}
		else
		{
			GBezierLine * pBezier = (GBezierLine *)pLine;
			prh->RenderBezierByInfo(pBezier->getBSInfo(), col, &ptPosDiff);
		}
	}
	prh->SetLineStyle(savedstyle);

}

void MarkingSplitLine::SetSplitPoint( float x, float y, int isec/*=0*/ )
{
	if (x != ptSplitPoint.x || y != ptSplitPoint.y || isec != iSplitSec)
	{
		ptSplitPoint.x = x;
		ptSplitPoint.y = y;
		iSplitSec = isec;
		bSplitPointChanged = true;
	}
}