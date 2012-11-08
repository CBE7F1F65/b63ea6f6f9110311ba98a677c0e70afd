#include "StdAfx.h"
#include "GLine.h"

#include "RenderHelper.h"
#include "RenderTargetManager.h"
#include "ColorManager.h"
#include "MainInterface.h"
#include "Command.h"
#include "GBaseNode.h"
#include "GObjectManager.h"

/************************************************************************/
/* GLINE                                                                */
/************************************************************************/
GLine::GLine(void)
{
	plbegin = NULL;
	plend = NULL;
	pmid = NULL;
	nLineRenderStyle = RHLINESTYLE_LINE;
}

GLine::~GLine(void)
{
}

void GLine::SetBeginEnd( float xb, float yb, float xe, float ye, float fAllowance/*=-1*/ )
{
	if (plbegin)
	{
		if (fAllowance >= 0 && fabsf(xb-plbegin->getX()) <= fAllowance && fabsf(yb-plbegin->getY()) <= fAllowance)
		{
		}
		else
		{
			plbegin->CallMoveTo(this, xb, yb, false);
		}
	}
	if (plend)
	{
		if (fAllowance >= 0 && fabsf(xe-plend->getX()) <= fAllowance && fabsf(ye-plend->getY()) <= fAllowance)
		{
		}
		else
		{
			plend->CallMoveTo(this, xe, ye, false);
		}
	}
//	UpdateMidPoint();
}

const char * GLine::getDisplayName()
{
	if (strDisplayName.length())
	{
		return strDisplayName.c_str();
	}
	return StringManager::getInstance().GetNNLineName();
}

bool GLine::MoveTo( GObject * pCaller, float newx, float newy, bool bTry, int moveActionID/*=-1*/ )
{
	if (!canMove())
	{
		return false;
	}

	if (moveActionID >= 0)
	{
		if (nMoveActionID == moveActionID)
		{
			return true;
		}
	}
	nMoveActionID = moveActionID;

	float xoffset = newx - getX();
	float yoffset = newy - getY();
	/*
	if (fabsf(xoffset) < M_FLOATEPS && fabsf(yoffset) < M_FLOATEPS)
	{
		return false;
	}
	*/
//	ToggleTryMoveState(bTry);

	if (pCaller)
	{
		plbegin->CallMoveByOffset(this, xoffset, yoffset, bTry, moveActionID);
		plend->CallMoveByOffset(this, xoffset, yoffset, bTry, moveActionID);

	}

//	UpdateMidPoint();
	return true;
}

bool GLine::CallMoveTo( GObject * pCaller, float newx, float newy, bool bTry, int moveActionID/*=-1*/ )
{
	if (!canMove())
	{
		return false;
	}
	if (moveActionID < 0)
	{
		moveActionID = pgm->GetNextMoveActionID(GMMATYPE_MOVE);
	}
	/*
	if (!clingByList.empty())
	{
		float xoffset = newx - getX();
		float yoffset = newy - getY();
		for (list<GPoint *>::iterator it=clingByList.begin(); it!=clingByList.end(); ++it)
		{
			(*it)->CallMoveByOffset(pCaller, xoffset, yoffset, bTry, moveActionID);
		}
	}
	*/
	return MoveTo(pCaller, newx, newy, bTry, moveActionID);
}

bool GLine::CallRotate( GObject * pCaller, float orix, float oriy, int angle, bool bTry, int moveActionID/*=-1*/ )
{
	if (!canMove())
	{
		return false;
	}
	if (moveActionID < 0)
	{
		moveActionID = pgm->GetNextMoveActionID(GMMATYPE_ROTATE, angle);
	}
	plbegin->CallRotate(pCaller, orix, oriy, angle, bTry, moveActionID);
	plend->CallRotate(pCaller, orix, oriy, angle, bTry, moveActionID);
	return true;
}

bool GLine::CallFlip( GObject * pCaller, float orix, float oriy, int angle, bool bTry, int moveActionID/*=-1*/ )
{
	if (!canMove())
	{
		return false;
	}
	if (moveActionID < 0)
	{
		moveActionID = pgm->GetNextMoveActionID(GMMATYPE_FLIP, angle);
	}
	plbegin->CallFlip(pCaller, orix, oriy, angle, bTry, moveActionID);
	plend->CallFlip(pCaller, orix, oriy, angle, bTry, moveActionID);
	return true;
}
bool GLine::CallScale( GObject * pCaller, float orix, float oriy, float fScaleX, float fScaleY, bool bTry, int moveActionID/*=-1*/ )
{
	if (!canMove())
	{
		return false;
	}
	if (moveActionID < 0)
	{
		moveActionID = pgm->GetNextMoveActionID(GMMATYPE_SCALE, 0, fScaleX, fScaleY);
	}
	plbegin->CallScale(pCaller, orix, oriy, fScaleX, fScaleY, bTry, moveActionID);
	plend->CallScale(pCaller, orix, oriy, fScaleX, fScaleY, bTry, moveActionID);
	return true;

}
void GLine::OnModify()
{
    if (!lstChildren.empty())
    {
        UpdateMidPoint();
    }
	super::OnModify();
}

bool GLine::CheckIntersectWithLineObj( GLine * pLine, list<PointF2D> *pPoints )
{
	if (isStraightLine())
	{
		if (pLine->isStraightLine())
		{
			return ((GStraightLine *)this)->CheckIntersectStraightStraight((GStraightLine *)pLine, pPoints);
		}
		else
		{
			return ((GBezierLine *)pLine)->CheckIntersectBezierStraight((GStraightLine *)this, pPoints);
		}
	}
	else
	{
		return ((GBezierLine *)this)->CheckIntersectBezierBezier((GBezierLine *)pLine, pPoints);
	}
	return false;
}

void GLine::OnRemove()
{
	super::OnRemove();
	DeclingByOther();
}

bool GLine::AddClingBy( GPoint * pPoint )
{
	ASSERT(pPoint);
	if (!pPoint->canAttach())
	{
		return false;
	}
	for (list<GPoint *>::iterator it=clingByList.begin(); it!=clingByList.end(); ++it)
	{
		if (*it == pPoint)
		{
			DASSERT(false);
			return false;
		}
	}
	clingByList.push_back(pPoint);
	return true;
}

void GLine::DeclingByOther( GPoint * pPoint/*=NULL*/ )
{
	if (!pPoint)
	{
		while (!clingByList.empty())
		{
			DeclingByOther(clingByList.front());
		}
	}
	else
	{
		for (list<GPoint *>::iterator it=clingByList.begin(); it!=clingByList.end(); ++it)
		{
			if (*it == pPoint)
			{
				pPoint->ClearClingTo();
				clingByList.erase(it);
				break;
			}
		}		
	}
}

bool GLine::isClingBy( GPoint * pPoint )
{
	if (!pPoint)
	{
		return false;
	}
	return pPoint->isClingTo(this);
}

GPiece * GLine::getPiece()
{
	GObject * pObj = pParent;
	while (pObj)
	{
		if (pObj->isPiece())
		{
			return (GPiece *)pObj;
		}
		pObj = pObj->getParent();
	}
	return NULL;
};

bool GLine::toStraightLine()
{
	if (isStraightLine())
	{
		return false;
	}
	GBezierLine * pBezier = (GBezierLine *)this;
	pBezier->GetBeginPoint()->GetHandle()->BindWith();
	pBezier->GetEndPoint()->GetHandle()->BindWith();
	pBezier->SetBeginHandlePos(pBezier->GetBeginPoint()->getX(), pBezier->GetBeginPoint()->getY());
	pBezier->SetEndHandlePos(pBezier->GetEndPoint()->getX(), pBezier->GetEndPoint()->getY());
	return true;
}

bool GLine::CloneData( GObject * pClone, GObject * pNewParent, bool bNoRelationship/*=true*/ )
{
	if (super::CloneData(pClone, pNewParent, bNoRelationship))
	{
		GLine * pLine = (GLine *)pClone;
		pLine->nLineRenderStyle = nLineRenderStyle;
		pLine->saInfo = saInfo;
		//////////////////////////////////////////////////////////////////////////
		list<GObject *>::reverse_iterator it=pLine->lstChildren.rbegin();
		pLine->plbegin = (GAnchorPoint *)*it;
		++it;
		pLine->plend = (GAnchorPoint *)*it;
		++it;
		pLine->pmid = (GMidPoint *)*it;
		ASSERT((*it)->isMidPoint());

		/*
		for (list<GPoint *>::iterator it=clingByList.begin(); it!=clingByList.end(); ++it)
		{
			pLine->AddClingBy(*it);
		}
		*/
		return true;
	}
	return false;
}

GNodeRelationshipGroup * GLine::CreateRelationshipGroup( bool bClingBy/*=true*/, bool bOneWay/*=false*/ )
{
	if (bClingBy && !clingByList.empty())
	{
		GNodeRelationshipGroup * pnrg = new GNodeRelationshipGroup(this);
		for (list<GPoint *>::iterator it=clingByList.begin(); it!=clingByList.end(); ++it)
		{
			GPoint * pClingPoint = *it;
			GNodeRelClingBy * pnrclingby = new GNodeRelClingBy(pClingPoint, *pClingPoint->getClingInfo());
		}
		return pnrg;
	}
	return NULL;
}

void GLine::OnIndepend()
{
	super::OnIndepend();
	DeclingByOther();
}

void GLine::AddSA( float xinner, float yinner, float fsa )
{
	if (!fsa)
	{
		saInfo.ClearSA();
		CallRedrawModify();
		return;
	}
	bool bLeft = MathHelper::getInstance().IsPointInLeftOfLine(plbegin->GetPointF2D(), plend->GetPointF2D(), PointF2D(xinner, yinner));
	int nSAFlag = 0;
	if (bLeft)
	{
		nSAFlag |= GSA_RIGHT;
	}
	else
	{
		nSAFlag |= GSA_LEFT;
	}
	saInfo.SetSA(fsa, nSAFlag);
	CallRedrawModify();
}

bool GLine::Isolate()
{
	DeclingByOther();
	plbegin->Isolate();
	plend->Isolate();
	return true;
}

/************************************************************************/
/* GSTRAIGHTLINE                                                        */
/************************************************************************/
GStraightLine::GStraightLine()
{
}

GStraightLine::~GStraightLine()
{
}


void GStraightLine::UpdateMidPoint()
{
	if (pmid)
	{
		pmid->SetPosition((plbegin->getX()+plend->getX())/2.0f, (plbegin->getY()+plend->getY())/2.0f);
	}
}


void GStraightLine::OnRender( int iHighlightLevel/*=0*/ )
{
	DWORD col = getLineColor(iHighlightLevel);
	RenderHelper * prh = &RenderHelper::getInstance();

	int nSavedLineStyle = prh->getLineStyle();
	prh->SetLineStyle(nLineRenderStyle);
	prh->RenderLine(plbegin->getX(), plbegin->getY(), plend->getX(), plend->getY(), col);
	if (saInfo.GetRawSA())
	{
		prh->SetLineStyle(RHLINESTYLE_SLASHLINE);
		PointF2D pt0 = plbegin->GetPointF2D();
		PointF2D pt1 = plend->GetPointF2D();
		if (!pt0.StrictEquals(pt1))
		{
			float fsa = saInfo.GetMulSA();
			PointF2D ptNormal = MathHelper::getInstance().GetNormal(pt0, pt1, fsa);
			float fl = MathHelper::getInstance().LineSegmentLength(pt0, pt1);
			float fmul = saInfo.GetRawSA()/fl;
			PointF2D ptMinus = (pt0-pt1)*fmul+pt0;
			PointF2D ptPlus = (pt1-pt0)*fmul+pt1;
			prh->RenderLine(ptMinus.x+ptNormal.x, ptMinus.y+ptNormal.y, ptPlus.x+ptNormal.x, ptPlus.y+ptNormal.y, col);
		}
	}
	prh->SetLineStyle(nSavedLineStyle);
}

bool GStraightLine::CheckNearTo( float px, float py, float r, float *plx, float *ply, int *isec/*=NULL*/ )
{
//	float xl, yt, xr, yb;
//	GetBoundingBox(&xl, &yt, &xr, &yb);
//	if (!MathHelper::getInstance().PointInRectTwoPoint(px, py, xl-r, yt-r, xr-xl+2*r, yb-yt+2*r))
	if (isec)
	{
		*isec=0;
	}
	return MathHelper::getInstance().PointNearToStraightLine(px, py, plbegin->getX(), plbegin->getY(), plend->getX(), plend->getY(), r, plx, ply);
}

bool GStraightLine::GetBoundingBox( float *xl, float *yt, float *xr, float * yb )
{
	if (!xl || !yt || !xr || !yb)
	{
		return false;
	}
	*xl = min(plbegin->getX(), plend->getX());
	*xr = max(plbegin->getX(), plend->getX());
	*yt = min(plbegin->getY(), plend->getY());
	*yb = max(plbegin->getY(), plend->getY());
	return true;
}

bool GStraightLine::CheckIntersectWithRect( float xl, float yt, float xr, float yb )
{
	return MathHelper::getInstance().LinePartialInRect(plbegin->getX(), plbegin->getY(), plend->getX(), plend->getY(), xl, yt, xr, yb);
}

bool GStraightLine::CheckIntersectStraightStraight( GStraightLine * pLine, list<PointF2D> *pPoints )
{
	float ix, iy;
	int iret = MathHelper::getInstance().LineSegmentIntersect(
		plbegin->getX(), plbegin->getY(), plend->getX(), plend->getY(),
		pLine->plbegin->getX(), pLine->plbegin->getY(), pLine->plend->getX(), pLine->plend->getY(),
		&ix, &iy);
	if (iret == MHLINEINTERSECT_INTERSECT)
	{
		if (pPoints)
		{
			pPoints->push_back(PointF2D(ix, iy));
		}
		return true;
	}
	return false;
}

float GStraightLine::CalculateProportion( float x, float y, int iSec )
{
	return MathHelper::getInstance().CalculateProportionOnStraightLine(plbegin->getX(), plbegin->getY(), plend->getX(), plend->getY(), x, y);
}

float GStraightLine::getLength()
{
	float fLength = MathHelper::getInstance().LineSegmentLength(plbegin->GetPointF2D(), plend->GetPointF2D());
	if (fLength <= 0)
	{
		fLength = M_FLOATEPS;
	}
	return fLength;
}

bool GStraightLine::GetPositionAtProportion( float fProp, PointF2D * pptPos, int*isec/*=NULL*/, QuadBezierPointF2D * pQuadHandles/*=NULL */ )
{
	float fbx = plbegin->getX();
	float fby = plbegin->getY();
	float fex = plend->getX();
	float fey = plend->getY();
	if (pptPos)
	{
		pptPos->x = (fex-fbx)*fProp+fbx;
		pptPos->y = (fey-fby)*fProp+fby;
	}
	if (isec)
	{
		*isec = 0;
	}
	return true;
}

float GStraightLine::CalculateMidPointProportion()
{
	return 0.5f;
}

PointF2D GStraightLine::GetTangentPointF2D( float fProp )
{
	return plend->GetPointF2D()-plbegin->GetPointF2D();
}
/************************************************************************/
/* GBEZEIERLINE                                                         */
/************************************************************************/
GBezierLine::GBezierLine()
{
}

GBezierLine::GBezierLine(GObject * parent, PointF2D pb, PointF2D pe)
{
	ASSERT(parent!=NULL);

	plbegin = new GAnchorPoint(this, pb.x, pb.y);
	plend = new GAnchorPoint(this, pe.x, pe.y);
	pmid = new GMidPoint(this);

	parent->AddChild(this);
	OnInit();
}

GBezierLine::GBezierLine(GObject * parent, PointF2D pb, PointF2D pbh, PointF2D peh, PointF2D pe)
{
	ASSERT(parent!=NULL);

	plbegin = new GAnchorPoint(this, pb.x, pb.y);
	plend = new GAnchorPoint(this, pe.x, pe.y);
	plbegin->SetHandlePosition(pbh.x, pbh.y);
	plend->SetHandlePosition(peh.x, peh.y);
	pmid = new GMidPoint(this);

	parent->AddChild(this);
	OnInit();
}

GBezierLine::GBezierLine( GObject * parent, float cx, float cy, float r, int quadrant )
{
	ASSERT(parent!=NULL);

	plbegin = new GAnchorPoint(this, cx, cy);
	plend = new GAnchorPoint(this, cx, cy);
	pmid = new GMidPoint(this);
	SetPosByQuarterCircle(cx, cy, r, quadrant);

	parent->AddChild(this);

}

GBezierLine::~GBezierLine()
{
}

void GBezierLine::OnPrecisionChanged(float fPrecision)
{
    GStraightLine::OnPrecisionChanged(fPrecision);
    if (plbegin && plend)
    {
        if (!isStraightLine())
        {
            bsinfo.ResetPoints(plbegin->GetPointF2D(), plbegin->GetHandle()->GetPointF2D(), plend->GetHandle()->GetPointF2D(), plend->GetPointF2D(), fPrecision);
			CallModify();
        }
    }
}

void GBezierLine::UpdateMidPoint()
{
	if (pmid)
	{
		if (isStraightLine())
		{
			return GStraightLine::UpdateMidPoint();
		}
		else
		{
			PointF2D pq;
			if (bsinfo.GetSubPointsCount())
			{
				GetPositionAtProportion(0.5f, &pq);
				pmid->SetPosition(pq.x, pq.y);
			}
			else
			{
				DASSERT(false);
				MathHelper::getInstance().CalculateBezier(plbegin->GetPointF2D(), plbegin->GetHandle()->GetPointF2D(), plend->GetHandle()->GetPointF2D(), plend->GetPointF2D(), 0.5f, &pq);
				pmid->SetPosition(pq.x, pq.y);
			}
		}
	}
}

void GBezierLine::OnModify()
{
    if (!lstChildren.empty())
    {
        if (plbegin && plend)
        {
            if (!isStraightLine())
            {
                bsinfo.ResetPoints(plbegin->GetPointF2D(), plbegin->GetHandle()->GetPointF2D(), plend->GetHandle()->GetPointF2D(), plend->GetPointF2D(), MainInterface::getInstance().GetPrecision());
            }
		}
	}
	super::OnModify();
}

void GBezierLine::OnRender( int iHighlightLevel/*=0*/ )
{
	if (isStraightLine())
	{
		GStraightLine::OnRender(iHighlightLevel);
	}
	else
	{
		DWORD col = getLineColor(iHighlightLevel);

		RenderHelper * prh = &RenderHelper::getInstance();

		int nSavedLineStyle = prh->getLineStyle();
		prh->SetLineStyle(nLineRenderStyle);

		prh->RenderBezierByInfo(&bsinfo, col);

		if (saInfo.GetRawSA())
		{
			prh->SetLineStyle(RHLINESTYLE_SLASHLINE);
			prh->RenderBezierByInfo(&bsinfo, col, saInfo.GetMulSA(), saInfo.GetLeftMul());
		}

		prh->SetLineStyle(nSavedLineStyle);
	}
}

bool GBezierLine::CheckNearTo( float px, float py, float r, float *plx, float *ply, int * isec/*=NULL*/ )
{
	if (isStraightLine())
	{
		return GStraightLine::CheckNearTo(px, py, r, plx, ply);
	}
	else
	{
		float nx=0, ny=0;
		bool bFound=false;
		float ndistpow2 = r*r;

		float xl, yt, xr, yb;
		int isection=0;
		GetBoundingBox(&xl, &yt, &xr, &yb);
		MathHelper * pmh = &MathHelper::getInstance();
		if (!pmh->PointInRect(px, py, xl-r, yt-r, xr-xl+2*r, yb-yt+2*r))
		{
			return false;
		}
		int nPoints = bsinfo.GetSubPointsCount();
		for (int i=0; i<nPoints-1; i++)
		{
			float x1 = bsinfo.GetX(i);
			float y1 = bsinfo.GetY(i);
			float x2 = bsinfo.GetX(i+1);
			float y2 = bsinfo.GetY(i+1);
			if (pmh->PointInRectTwoPoint(px, py, x1, y1, x2, y2, r))
			{
				float tnx, tny;
				float distpow2 = MathHelper::getInstance().NearestPointOnStraightLinePow2(px, py, x1, y1, x2, y2, &tnx, &tny);
				if (distpow2 < r*r)
				{
					if (!plx && !ply)
					{
						return true;
					}
					if (distpow2 < ndistpow2)
					{
						bFound = true;
						ndistpow2 = distpow2;
						nx = tnx;
						ny = tny;
						isection = i;
					}
				}
			}
		}
		if (bFound)
		{
			if (plx)
			{
				*plx = nx;
			}
			if (ply)
			{
				*ply = ny;
			}
			if (isec)
			{
				*isec = isection;
			}
			return true;
		}
	}
	return false;
}

bool GBezierLine::CheckIntersectWithRect( float xl, float yt, float xr, float yb )
{
	if (isStraightLine())
	{
		return  GStraightLine::CheckIntersectWithRect(xl, yt, xr, yb);
	}
	else
	{
		float bxl, byt, bxr, byb;
		GetBoundingBox(&bxl, &byt, &bxr, &byb);
		if (bxl > xr || byt > yb || bxr < xl || byb < yt)
		{
			return false;
		}
		int nPoints = bsinfo.GetSubPointsCount();
		for (int i=0; i<nPoints-1; i++)
		{
			if (MathHelper::getInstance().LinePartialInRect(bsinfo.GetX(i), bsinfo.GetY(i), bsinfo.GetX(i+1),	bsinfo.GetY(i+1), xl, yt, xr, yb/*, true*/))
			{
				return true;
			}
		}
	}
	return false;
}

bool GBezierLine::GetBoundingBox( float *xl, float *yt, float *xr, float * yb )
{
	if (isStraightLine())
	{
		return GStraightLine::GetBoundingBox(xl, yt, xr, yb);
	}
	else
	{
		if (!bsinfo.GetSubPointsCount())
		{
			bsinfo.ResetPoints(plbegin->GetPointF2D(), plbegin->GetHandle()->GetPointF2D(), plend->GetHandle()->GetPointF2D(), plend->GetPointF2D(), MainInterface::getInstance().GetPrecision());
//			DASSERT(false);
			return false;
		}
		if (!bsinfo.GetBoundingBox(xl, yt, xr, yb))
		{
			ASSERT(false);
			return false;
		}
	}
	return true;
}

GObject * GBezierLine::CreateNewClone( GObject * pNewParent/*=NULL*/, GObject * pBeforeObj/*=NULL*/ )
{
    _GOBJ_CLONE_PRE(GBezierLine);
	_GOBJ_CLONE_POST();
}

void GBezierLine::SetBeginEnd( float xb, float yb, float xe, float ye, float fAllowance/*=-1*/ )
{
	GLine::SetBeginEnd(xb, yb, xe, ye, fAllowance);
	SetBeginHandlePos(xb, yb, fAllowance);
	SetEndHandlePos(xe, ye, fAllowance);
}

void GBezierLine::SetBeginHandlePos( float x, float y, float fAllowance/*=-1*/ )
{
	plbegin->SetHandlePosition(x, y, fAllowance);
}

void GBezierLine::SetEndHandlePos( float x, float y, float fAllowance/*=-1*/ )
{
	plend->SetHandlePosition(x, y, fAllowance);
}

void GBezierLine::SetPosByQuarterCircle( float cx, float cy, float r, int quadrant )
{
	float pbx, pby, pex, pey, pbhx, pbhy, pehx, pehy;
	float rk = r*M_BEZIERCIRCLE_KAPPA;

	switch (quadrant)
	{
	case QUADRANT_1:
		pbx = cx + r;
		pby = cy;
		pex = cx;
		pey = cy + r;
		pbhx = cx + r;
		pbhy = cy+rk;
		pehx = cx+rk;
		pehy = cy + r;
		break;
	case QUADRANT_2:
		pbx = cx;
		pby = cy + r;
		pex = cx - r;
		pey = cy;
		pbhx = cx-rk;
		pbhy = cy + r;
		pehx = cx - r;
		pehy = cy+rk;
		break;
	case QUADRANT_3:
		pbx = cx - r;
		pby = cy;
		pex = cx;
		pey = cy - r;
		pbhx = cx - r;
		pbhy = cy-rk;
		pehx = cx-rk;
		pehy = cy - r;
		break;
	case QUADRANT_4:
		pbx = cx;
		pby = cy - r;
		pex = cx + r;
		pey = cy;
		pbhx = cx+rk;
		pbhy = cy - r;
		pehx = cx + r;
		pehy = cy-rk;
		break;
	default:
		ASSERT(false);
	}

	SetBeginEnd(pbx, pby, pex, pey);
	SetBeginHandlePos(pbhx, pbhy);
	SetEndHandlePos(pehx, pehy);

}

bool GBezierLine::isStraightLine()
{
	if (plbegin->isHandleIdentical() && plend->isHandleIdentical())
	{
		return true;
	}
	return false;
}

bool GBezierLine::CheckIntersectBezierStraight( GStraightLine * pLine, list<PointF2D> *pPoints )
{
	bool bHaveIntersection = false;

	GBaseNode tbasenode;
	GBezierLine * pfakeline = new GBezierLine(&tbasenode, PointF2D(0, 0), PointF2D(0, 0));

	int npts = bsinfo.GetSubPointsCount();
	for (int i=0; i<npts-1; i++)
	{
		pfakeline->SetBeginEnd(bsinfo.GetX(i), bsinfo.GetY(i), bsinfo.GetX(i+1), bsinfo.GetY(i+1));
		if (pLine->CheckIntersectStraightStraight(pfakeline, pPoints))
		{
			bHaveIntersection = true;
		}
	}
	tbasenode.RemoveAllChildren(true);
	return bHaveIntersection;
}

bool GBezierLine::CheckIntersectBezierBezier( GBezierLine * pLine, list<PointF2D> *pPoints )
{
	bool bHaveIntersection = false;

	GBaseNode tbasenode;
	GBezierLine * pfakeline = new GBezierLine(&tbasenode, PointF2D(0, 0), PointF2D(0, 0));

	int npts = bsinfo.GetSubPointsCount();
	for (int i=0; i<npts-1; i++)
	{
		pfakeline->SetBeginEnd(bsinfo.GetX(i), bsinfo.GetY(i), bsinfo.GetX(i+1), bsinfo.GetY(i+1));
		if (pLine->CheckIntersectBezierStraight(pfakeline, pPoints))
		{
			bHaveIntersection = true;
		}
	}
	tbasenode.RemoveAllChildren(true);
	return bHaveIntersection;
}

float GBezierLine::CalculateProportion( float x, float y, int iSec )
{
	if (isStraightLine())
	{
		return GStraightLine::CalculateProportion(x, y, iSec);
	}
	PointF2D ptPoint(x, y);
	if (ptPoint.StrictEquals(plbegin->GetPointF2D()))
	{
		return 0.0f;
	}
	else if (ptPoint.StrictEquals(plend->GetPointF2D()))
	{
		return 1.0f;
	}
	if (iSec >= 0 && iSec < bsinfo.GetSubPointsCount()-1)
	{
		float fTotalLength = getLength();
		if (!fTotalLength)
		{
			return 0;
		}
		float fsubproportion = MathHelper::getInstance().CalculateProportionOnStraightLine(bsinfo.GetX(iSec), bsinfo.GetY(iSec), bsinfo.GetX(iSec+1), bsinfo.GetY(iSec+1), x, y);
		if (fsubproportion < 0 || fsubproportion > 1)
		{
			return 0;
		}
		float fSectionLength = bsinfo.GetLength(iSec);
		float fPreLength = 0.0f;
		if (iSec >= 1)
		{
			fPreLength = bsinfo.GetLength(0, iSec-1);
		}
		float fProportionLength = fPreLength+fsubproportion * fSectionLength;
		return fProportionLength/fTotalLength;
	}
	return 0;
}

float GBezierLine::getLength()
{
	if (isStraightLine())
	{
		return GStraightLine::getLength();
	}
	float fLength = bsinfo.GetLength();
	if (fLength <= 0)
	{
		fLength = M_FLOATEPS;
	}
	return fLength;
}

bool GBezierLine::GetPositionAtProportion( float fProp, PointF2D * pptPos, int*isec/*=NULL*/, QuadBezierPointF2D * pQuadHandles/*=NULL */ )
{
	if (isStraightLine())
	{
		return GStraightLine::GetPositionAtProportion(fProp, pptPos, isec, pQuadHandles);
	}

	if (fProp < 0.0f)
	{
		fProp = 0.0f;
	}
	if (fProp > 1.0f)
	{
		fProp = 1.0f;
	}

	if (pQuadHandles)
	{
		float s = GetSFromProportion(fProp);
		MathHelper::getInstance().CalculateBezierSubDivision(
			QuadBezierPointF2D(this),
			s,
			pQuadHandles);
	}
	if (fProp == 0.0f)
	{
		if (pptPos)
		{
			*pptPos = plbegin->GetPointF2D();
		}
		if (isec)
		{
			*isec = 0;
		}
		return true;
	}
	if (fProp == 1.0f)
	{
		if (pptPos)
		{
			*pptPos = plend->GetPointF2D();
		}
		if (isec)
		{
			*isec = bsinfo.GetSubPointsCount()-2;
		}
		return true;
	}

	float fTotalLength = bsinfo.GetLength();
	float fProportionLength = fTotalLength*fProp;

	float fCLength=0;
	int i = 0;
	for (; i<bsinfo.GetSubPointsCount()-1; i++)
	{
		fCLength += bsinfo.GetLength(i);
		if (fCLength >= fProportionLength)
		{
			break;
		}
	}
	fCLength -= bsinfo.GetLength(i);
	float fRestLength = fProportionLength-fCLength;
	float fRestProportion = fRestLength/bsinfo.GetLength(i);


	float fbx = bsinfo.GetX(i);
	float fby = bsinfo.GetY(i);
	float fex = bsinfo.GetX(i+1);
	float fey = bsinfo.GetY(i+1);
	if (pptPos)
	{
		pptPos->x = (fex-fbx)*fRestProportion+fbx;
		pptPos->y = (fey-fby)*fRestProportion+fby;
	}
	if (isec)
	{
		*isec = i;
	}


	return true;
}

float GBezierLine::CalculateMidPointProportion()
{
	if (isStraightLine())
	{
		return GStraightLine::CalculateMidPointProportion();
	}
	return 0.5f;
	/*
	if (isStraightLine())
	{
		return 0.5f;
	}
	float lx;
	float ly;
	int isec = 0;
	if (CheckNearTo(pmid->getX(), pmid->getY(), 1.0f, &lx, &ly, &isec))
	{
		return CalculateProportion(pmid->getX(), pmid->getY(), isec);
	}
	return 0;
	*/
}

bool GBezierLine::isLengthCalculated()
{
	return bsinfo.isLengthCalculated();
}

bool GBezierLine::toBezierLine()
{
	if (!isStraightLine())
	{
		return false;
	}
	float xb = plbegin->getX();
	float yb = plbegin->getY();
	float xe = plend->getX();
	float ye = plend->getY();

	float s = 0.25f;
	float sb = s;
	float se = 1-s;

	float hxb = (xe-xb)* sb + xb;
	float hyb = (ye-yb)* sb + yb;
	float hxe = (xe-xb)* se + xb;
	float hye = (ye-yb)* se + yb;
	SetBeginHandlePos(hxb, hyb);
	SetEndHandlePos(hxe, hye);
	return true;
}

PointF2D GBezierLine::GetTangentPointF2D( float fProp )
{
	if (isStraightLine())
	{
		return GStraightLine::GetTangentPointF2D(fProp);
	}

	float s = GetSFromProportion(fProp);
	PointF2D p0 = plbegin->GetPointF2D();
	PointF2D p1 = plbegin->GetHandle()->GetPointF2D();
	PointF2D p2 = plend->GetHandle()->GetPointF2D();
	PointF2D p3 = plend->GetPointF2D();
	//dP(t) / dt =  -3(1-t)^2 * P0 + 3(1-t)^2 * P1 - 6t(1-t) * P1 - 3t^2 * P2 + 6t(1-t) * P2 + 3t^2 * P3
	float s2 = s*s;
	return p0*(-3*(s2-2*s+1))+p1*(3*(3*s2-4*s+1))+p2*(-3*(3*s2-2*s))+p3*(3*s2);
}

bool GBezierLine::CloneData(GObject * pClone, GObject * pNewParent, bool bNoRelationship/*=true*/)
{
	if (super::CloneData(pClone, pNewParent, bNoRelationship))
	{
		GBezierLine * pBezier = (GBezierLine *)pClone;

		pBezier->bsinfo = bsinfo;
		return true;
	}
	return false;
}

GLine * GBezierLine::Clip( float fClipProportion )
{
	if (fClipProportion <= 0 || fClipProportion >= 1)
	{
		return NULL;
	}

	GLine * pClonedLine = (GLine *)CreateNewClone(NULL, this);

	bool bBezier = !isStraightLine();
	
	GHandlePoint * pOEndHandleBind = plend->GetHandle()->getBindWith();

	PointF2D ptBegin = plbegin->GetPointF2D();
	PointF2D ptEnd = plend->GetPointF2D();

	GClingInfo oEndClingToInfo = *plend->getClingInfo();

	list<GPoint *>  * plstEndMergeWith = plend->getMergeWith();
	GPoint * pEndMergeWith = NULL;
	if (!plstEndMergeWith->empty())
	{
		pEndMergeWith = plstEndMergeWith->front();
	}

	list<GPoint *> lstClingBy = clingByList;
	list<GClingInfo> lstCLI;
	if (!lstClingBy.empty())
	{
		for (list<GPoint *>::iterator it=lstClingBy.begin(); it!=lstClingBy.end(); ++it)
		{
			lstCLI.push_back(*(*it)->getClingInfo());
		}
	}
	float fOLength = getLength();

	/*
	PointF2D pSubBh1;
	PointF2D pSubEh1;
	PointF2D pSubBh2;
	PointF2D pSubEh2;
	*/
	QuadBezierPointF2D quadHandles;
	if (bBezier)
	{
		float s = GetSFromProportion(fClipProportion);
		MathHelper::getInstance().CalculateBezierSubDivision(
			QuadBezierPointF2D(this),
			s,
			&quadHandles);
//			&pSubBh1, &pSubEh1, &pSubBh2, &pSubEh2);
	}

	plend->ClingTo(NULL, 0);
	plend->DemergeFrom();
	DeclingByOther();

	PointF2D ptClip;
	int isec;
	GetPositionAtProportion(fClipProportion, &ptClip, &isec);

	SetBeginEnd(ptBegin.x, ptBegin.y, ptClip.x, ptClip.y);
	if (bBezier)
	{
		plbegin->SetHandlePosition(quadHandles.ptb.x, quadHandles.ptb.y);//(pSubBh1.x, pSubBh1.y);
		plend->SetHandlePosition(quadHandles.ptbh.x, quadHandles.ptbh.y);//(pSubEh1.x, pSubEh1.y);
	}

	pClonedLine->SetBeginEnd(ptClip.x, ptClip.y, ptEnd.x, ptEnd.y);
	GAnchorPoint * pNewBeginPoint = pClonedLine->GetBeginPoint();
	GAnchorPoint * pNewEndPoint = pClonedLine->GetEndPoint();
	if (bBezier)
	{
		pNewBeginPoint->SetHandlePosition(quadHandles.pteh.x, quadHandles.pteh.y);//(pSubBh2.x, pSubBh2.y);
		pNewEndPoint->SetHandlePosition(quadHandles.pte.x, quadHandles.pte.y);//(pSubEh2.x, pSubEh2.y);
	}
	if (pOEndHandleBind)
	{
		pOEndHandleBind->BindWith(pNewEndPoint->GetHandle());
	}
	if (bBezier)
	{
		pNewBeginPoint->GetHandle()->BindWith(plend->GetHandle());
	}

	pNewEndPoint->ClingTo(oEndClingToInfo);
	if (pEndMergeWith)
	{
		pNewEndPoint->MergeWith(pEndMergeWith);
	}

	plend->MergeWith(pNewBeginPoint);

	if (!lstClingBy.empty())
	{
		list<GClingInfo>::iterator jt=lstCLI.begin();
		for (list<GPoint *>::iterator it=lstClingBy.begin(); it!=lstClingBy.end(); ++it, ++jt)
		{
			GPoint * pClingByPoint = *it;
			GClingInfo * pcli = &(*jt);
			float fCP = 0;
			if (pcli->CalculateClingProportion(&fCP, fOLength))
			{
				if (fCP < fClipProportion)
				{
					float fNewProp = fCP/fClipProportion;
					ASSERT(fNewProp > 0 && fNewProp <= 1);
					pcli->ApplyChange(this, fNewProp);
					pClingByPoint->ClingTo(*pcli);
				}
				else if (fCP > fClipProportion)
				{
					float fNewProp = (fCP-fClipProportion)/(1-fClipProportion);
					ASSERT(fNewProp > 0 && fNewProp <= 1);
					int nType = pcli->GetClingType();
					pcli->SetClingTo(pClonedLine, fNewProp);
					pcli->ApplyTypeChange(nType);
					pClingByPoint->ClingTo(*pcli);
				}
				else
				{
	//				pClingByPoint->MergeWith(plend);
					// Cannot Undo
				}

			}
		}
	}
	return pClonedLine;
}

bool GBezierLine::JoinLine( GLine * pLine )
{
	if (!pLine)
	{
		return false;
	}

	float fThisLength = getLength();
	if (!fThisLength)
	{
		return false;
	}
	float fThatLength = pLine->getLength();
	if (!fThatLength)
	{
		pLine->RemoveFromParent(true);
		return true;
	}

	float fTotalLength = fThisLength+fThatLength;
	float fCombinePointCP = fThisLength/fTotalLength;

	GAnchorPoint * pThisBegin = GetBeginPoint();
	GAnchorPoint * pThisEnd = GetEndPoint();
	GAnchorPoint * pThatBegin = pLine->GetBeginPoint();
	GAnchorPoint * pThatEnd = pLine->GetEndPoint();

	if (pThisBegin->isMergeWith(pThatBegin))
	{
		SwapBeginEnd();
	}
	else if (pThisBegin->isMergeWith(pThatEnd))
	{
		SwapBeginEnd();
		pLine->SwapBeginEnd();
	}
	else if (pThisEnd->isMergeWith(pThatEnd))
	{
		pLine->SwapBeginEnd();
	}
	else if (!pThisEnd->isMergeWith(pThatBegin))
	{
		return false;
	}

	list<GPoint *> lstCombinPointMergeWith = *(pThisEnd->getMergeWith());
	lstCombinPointMergeWith.remove(pThatBegin);

	list<GPoint *> lstThisClingBy = clingByList;
	list<GPoint *> lstThatClingBy = *(pLine->getClingBy());
	list<GClingInfo> lstThisCLI;
	list<GClingInfo> lstThatCLI;
	if (!lstThisClingBy.empty())
	{
		for (list<GPoint *>::iterator it=lstThisClingBy.begin(); it!=lstThisClingBy.end(); ++it)
		{
			lstThisCLI.push_back(*(*it)->getClingInfo());
		}
	}
	if (!lstThatClingBy.empty())
	{
		for (list<GPoint *>::iterator it=lstThatClingBy.begin(); it!=lstThatClingBy.end(); ++it)
		{
			lstThatCLI.push_back(*(*it)->getClingInfo());
		}
	}

	list<GPoint *> * plstThatEndMergeWith = pThatEnd->getMergeWith();
	GPoint * pThatEndMergeWith = NULL;
	if (!plstThatEndMergeWith->empty())
	{
		pThatEndMergeWith = plstThatEndMergeWith->front();
	}

	PointF2D ptThatEnd = pThatEnd->GetPointF2D();
	GHandlePoint * pThatEndHandle = pThatEnd->GetHandle();
	PointF2D ptThatEndHandle = pThatEndHandle->GetPointF2D();

	GHandlePoint * pThatEndHandleBindWith = pThatEndHandle->getBindWith();

	//////////////////////////////////////////////////////////////////////////
	pLine->RemoveFromParent(true);
	//////////////////////////////////////////////////////////////////////////

	pThisEnd->DemergeFrom();
	pThisEnd->GetHandle()->BindWith();
	DeclingByOther();

	pThisEnd->CallMoveTo(pThisEnd, ptThatEnd.x, ptThatEnd.y, false);
	
	GHandlePoint * pThisBeginHandle = pThisBegin->GetHandle();
	PointF2D ptThisBegin = pThisBegin->GetPointF2D();
	PointF2D ptThisBeginHandle = pThisBeginHandle->GetPointF2D();
	PointF2D ptThisNewBeginHandle = (ptThisBeginHandle-ptThisBegin)*(1.0f/fCombinePointCP)+ptThisBegin;
	pThisBegin->GetHandle()->CallMoveTo(pThisBegin, ptThisNewBeginHandle.x, ptThisNewBeginHandle.y, false);
	PointF2D ptThisNewEndHandle = (ptThatEndHandle-ptThatEnd)*(1.0f/(1-fCombinePointCP))+ptThatEnd;
	pThisEnd->GetHandle()->CallMoveTo(pThisEnd, ptThisNewEndHandle.x, ptThisNewEndHandle.y, false);

	if (pThatEndMergeWith)
	{
		pThisEnd->MergeWith(pThatEndMergeWith);
	}
	if (pThatEndHandleBindWith)
	{
		pThatEndHandleBindWith->BindWith(pThisEnd->GetHandle());
	}

	if (!lstThisClingBy.empty())
	{
		list<GClingInfo>::iterator jt=lstThisCLI.begin();
		for (list<GPoint *>::iterator it=lstThisClingBy.begin(); it!=lstThisClingBy.end(); ++it, ++jt)
		{
			GPoint * pClingByPoint = *it;
			GClingInfo * pcli = &(*jt);
			float fCP = 0;
			if (pcli->CalculateClingProportion(&fCP, fThisLength))
			{
				fCP *= fCombinePointCP;
				pcli->ApplyTypeChange(GCLING_PROPORTION);
				pcli->ApplyChange(this, fCP);
				pClingByPoint->ClingTo(*pcli);
			}
		}
	}
	if (!lstThatClingBy.empty())
	{
		list<GClingInfo>::iterator jt=lstThatCLI.begin();
		for (list<GPoint *>::iterator it=lstThatClingBy.begin(); it!=lstThatClingBy.end(); ++it, ++jt)
		{
			GPoint * pClingByPoint = *it;
			GClingInfo * pcli = &(*jt);
			float fCP = 0;
			if (pcli->CalculateClingProportion(&fCP, fThisLength))
			{
				fCP = fCP*(1-fCombinePointCP)+fCombinePointCP;
				pcli->ApplyTypeChange(GCLING_PROPORTION);
				pcli->ApplyChange(this, fCP);
				pClingByPoint->ClingTo(*pcli);
			}
		}
	}
	if (!lstCombinPointMergeWith.empty())
	{
		for (list<GPoint *>::iterator it=lstCombinPointMergeWith.begin(); it!=lstCombinPointMergeWith.end(); ++it)
		{
			(*it)->ClingTo(this, fCombinePointCP);
		}
	}
	return true;
}

bool GBezierLine::SwapBeginEnd()
{
	GPoint * pOBeginMergeWith = NULL;
	GPoint * pOEndMergeWith = NULL;

	list<GPoint *> * plstMergeWith;
	plstMergeWith = plbegin->getMergeWith();
	if (!plstMergeWith->empty())
	{
		pOBeginMergeWith = plstMergeWith->front();
	}
	plstMergeWith = plend->getMergeWith();
	if (!plstMergeWith->empty())
	{
		pOEndMergeWith = plstMergeWith->front();
	}

	list<GPoint *> lstClingBy = clingByList;
	list<GClingInfo> lstCLI;
	if (!lstClingBy.empty())
	{
		for (list<GPoint *>::iterator it=lstClingBy.begin(); it!=lstClingBy.end(); ++it)
		{
			lstCLI.push_back(*(*it)->getClingInfo());
		}
	}

	bool bBezier = !isStraightLine();

	PointF2D ptOBegin = plbegin->GetPointF2D();
	PointF2D ptOEnd = plend->GetPointF2D();
	PointF2D ptOBeginHandle;
	PointF2D ptOEndHandle;
	GHandlePoint * pOBeginHandleBindWith;
	GHandlePoint * pOEndHandleBindWith;
	if (bBezier)
	{
		GHandlePoint * pBeginHandle = plbegin->GetHandle();
		GHandlePoint * pEndHandle = plend->GetHandle();
		ptOBeginHandle = pBeginHandle->GetPointF2D();
		ptOEndHandle = pEndHandle->GetPointF2D();
		pOBeginHandleBindWith = pBeginHandle->getBindWith();
		pOEndHandleBindWith = pEndHandle->getBindWith();

		pBeginHandle->BindWith();
		pEndHandle->BindWith();
	}

	plbegin->DemergeFrom();
	plend->DemergeFrom();
	DeclingByOther();

	SetBeginEnd(ptOEnd.x, ptOEnd.y, ptOBegin.x, ptOBegin.y);
	if (bBezier)
	{
		plbegin->SetHandlePosition(ptOEndHandle.x, ptOEndHandle.y);
		plend->SetHandlePosition(ptOBeginHandle.x, ptOBeginHandle.y);
		plbegin->GetHandle()->BindWith(pOEndHandleBindWith);
		plend->GetHandle()->BindWith(pOBeginHandleBindWith);
	}
	if (pOEndMergeWith)
	{
		plbegin->MergeWith(pOEndMergeWith);
	}
	if (pOBeginMergeWith)
	{
		plend->MergeWith(pOBeginMergeWith);
	}

	if (!lstClingBy.empty())
	{
		list<GClingInfo>::iterator jt=lstCLI.begin();
		for (list<GPoint *>::iterator it=lstClingBy.begin(); it!=lstClingBy.end(); ++it, ++jt)
		{
			GPoint * pClingByPoint = *it;
			GClingInfo * pcli = &(*jt);
			float fCP = 0;
			if (pcli->CalculateClingProportion(&fCP, getLength()))
			{
				fCP = 1-fCP;
				if (pcli->GetClingType() == GCLING_BEGINOFFSET)
				{
					pcli->ApplyTypeChange(GCLING_ENDOFFSET);
				}
				else if (pcli->GetClingType() == GCLING_ENDOFFSET)
				{
					pcli->ApplyTypeChange(GCLING_BEGINOFFSET);
				}
				pcli->ApplyChange(this, fCP);
				pClingByPoint->ClingTo(*pcli);
			}
		}
	}
	return true;
}

bool GBezierLine::Extend( float tBegin, float tEnd )
{
	ASSERT(tBegin >= 0.0f);
	ASSERT(tEnd >= 0.0f);

	if (tBegin == 0.0f && tEnd == 0.0f)
	{
		return true;
	}

	PointF2D ptNewBegin;
	PointF2D ptNewEnd;

	QuadBezierPointF2D quadNewBeginBezierOutput;
	QuadBezierPointF2D quadNewEndBezierOutput;
	
	GHandlePoint * pBeginHandle = plbegin->GetHandle();
	GHandlePoint * pEndHandle = plend->GetHandle();

	GPoint * pBeginMerge = NULL;
	GPoint * pEndMerge = NULL;

	float fOLength = getLength();

	list<GPoint *> lstClingBy = clingByList;
	list<GClingInfo> lstCLI;
	if (!lstClingBy.empty())
	{
		for (list<GPoint *>::iterator it=lstClingBy.begin(); it!=lstClingBy.end(); ++it)
		{
			lstCLI.push_back(*(*it)->getClingInfo());
		}
	}

	if (!plbegin->getMergeWith()->empty())
	{
		pBeginMerge = plbegin->getMergeWith()->front();
	}
	if (!plend->getMergeWith()->empty())
	{
		pEndMerge = plend->getMergeWith()->front();
	}
	DeclingByOther();

	if (tBegin != 0.0f)
	{
		plbegin->DemergeFrom();

		float tb = -tBegin;
		if (GetPositionAtExtendedProportion(tb, &ptNewBegin, &quadNewBeginBezierOutput))
		{
			plbegin->CallMoveTo(plbegin, ptNewBegin.x, ptNewBegin.y, false);
			if (!isStraightLine())
			{
				pBeginHandle->CallMoveTo(pBeginHandle, quadNewBeginBezierOutput.ptbh.x, quadNewBeginBezierOutput.ptbh.y, false);
				pEndHandle->CallMoveTo(pEndHandle, quadNewBeginBezierOutput.pteh.x, quadNewBeginBezierOutput.pteh.y, false);
			}
//			return true;
		}
		else
		{
			ASSERT(false);
			return false;
		}
	}

	if (tEnd != 0.0f)
	{
		plend->DemergeFrom();

		float te = tEnd;
		if (tBegin != 0.0f)
		{
			te = te/(1+tBegin);
			// tBegin == 1?
//			tEnd = (tEnd-1)/(1.0f-tBegin)+1.0f;
		}
		te += 1.0f;
		if (GetPositionAtExtendedProportion(te, &ptNewEnd, &quadNewEndBezierOutput))
		{
			plend->CallMoveTo(plend, ptNewEnd.x, ptNewEnd.y, false);
			if (!isStraightLine())
			{
				pBeginHandle->CallMoveTo(pBeginHandle, quadNewEndBezierOutput.ptbh.x, quadNewEndBezierOutput.ptbh.y, false);
				pEndHandle->CallMoveTo(pEndHandle, quadNewEndBezierOutput.pteh.x, quadNewEndBezierOutput.pteh.y, false);
			}
//			return true;
		}
		else
		{
			ASSERT(false);
			return false;
		}
	}

	float tTotalInv = 1.0f/(tBegin+tEnd+1);
	if (pBeginMerge)
	{
		pBeginMerge->ClingTo(this, tBegin*tTotalInv);
	}
	if (pEndMerge)
	{
		pEndMerge->ClingTo(this, (tBegin+1)*tTotalInv);
	}
	if (!lstClingBy.empty())
	{
		list<GClingInfo>::iterator jt=lstCLI.begin();
		for (list<GPoint *>::iterator it=lstClingBy.begin(); it!=lstClingBy.end(); ++it, ++jt)
		{
			GPoint * pClingByPoint = *it;
			GClingInfo * pcli = &(*jt);
			float fCP;
			if (pcli->CalculateClingProportion(&fCP, fOLength))
			{
				fCP = (tBegin+fCP)*tTotalInv;
				pcli->ApplyChange(this, fCP);
				pClingByPoint->ClingTo(*pcli);
			}
		}
	}

	return true;
//	return false;
}

bool GBezierLine::GetPositionAtExtendedProportion( float fProp, PointF2D *pptPos, QuadBezierPointF2D * pQuadOutput/*=NULL*/ )
{
	if (isStraightLine())
	{
		return GStraightLine::GetPositionAtExtendedProportion(fProp, pptPos, pQuadOutput);
	}
	if (fProp >= 0.0f && fProp <= 1.0f)
	{
		return GetPositionAtProportion(fProp, pptPos, NULL, pQuadOutput);
	}

	MathHelper * pmh = &MathHelper::getInstance();
	
	QuadBezierPointF2D quadBezier(this);

	PointF2D * pptHandle;

	float fLength = getLength();
	float fAimPropDiff;

	float fMinS;
	float fMaxS;
	float fLL = -1;
	float fRL = -1;
	if (fProp < 0.0f)
	{
		fAimPropDiff = -fProp;
		fMinS = fAimPropDiff*-2;
		fMaxS = 0.0f;
		fRL = fLength;
	}
	else
	{
		fAimPropDiff = fProp-1.0f;
		fMinS = 1.0f;
		fMaxS = fAimPropDiff*2+1.0f;
		fLL = fLength;
	}
	float fAimLength = fLength*(1+fAimPropDiff);

	if (fProp < 0.0f)
	{
		if (pmh->CalculateExtendBezierToAimLength(
			quadBezier,
			fAimLength, fMinS, fMaxS, fLL, fRL,
			QuadBezierPointF2D(),
			quadBezier,
			pptPos, pQuadOutput))
		{
			return true;
		}
	}
	else
	{
		if (pmh->CalculateExtendBezierToAimLength(
			quadBezier,
			fAimLength, fMinS, fMaxS, fLL, fRL,
			quadBezier,
			QuadBezierPointF2D(),
			pptPos, pQuadOutput))
		{
			return true;
		}
	}

	return false;
}

float GBezierLine::GetSFromProportion( float fProp )
{
	if (fProp == 0)
	{
		return 0;
	}
	else if (fProp == 1.0f)
	{
		return 1.0f;
	}
	else if (fProp > 0 && fProp < 1)
	{
		PointF2D ptPos;
		GetPositionAtProportion(fProp, &ptPos);
		MathHelper * pmh = &MathHelper::getInstance();

		PointF2D ptBegin = plbegin->GetPointF2D();
		PointF2D ptEnd = plend->GetPointF2D();
		PointF2D ptBeginHandle = plbegin->GetHandle()->GetPointF2D();
		PointF2D ptEndHandle = plend->GetHandle()->GetPointF2D();

		PointF2D ptA = (ptBeginHandle - ptEndHandle)*3 + ptEnd - ptBegin;
		PointF2D ptB = (ptBegin - ptBeginHandle*2 + ptEndHandle)*3;
		PointF2D ptC = (ptBeginHandle-ptBegin)*3;
		PointF2D ptD = ptBegin-ptPos;

		float s[3];
		int nroot = pmh->SolveCubicEquation(ptA.x, ptB.x, ptC.x, ptD.x, s);
		if (!nroot)
		{
			ASSERT(false);
		}
		if (nroot > 1)
		{
			float fDiffMin = M_FLOATMAX;
			int iMin = 0;
			for (int i=0; i<3; i++)
			{
				PointF2D pq;
				pmh->CalculateBezier(ptBegin, ptBeginHandle, ptEndHandle, ptEnd, s[i], &pq);
				pq = pq-ptPos;
				float fNowDiff = pq.x*pq.x+pq.y*pq.y;
				if (fNowDiff < fDiffMin)
				{
					fDiffMin = fNowDiff;
					iMin = i;
				}
			}
			return s[iMin];
			/*
			float fDiff = 1.0f;
			int iclose = -1;
			for (int i=0; i<3; i++)
			{
				float fNowDiff = fabsf(s[i]-fProp);
				if (fNowDiff < fDiff)
				{
					fDiff = fNowDiff;
					iclose = i;
				}
			}
			return s[iclose];
			*/
		}
		return s[0];
	}

	ASSERT(false);
	return 0;
}

/************************************************************************/
/* GImageLine                                                           */
/************************************************************************/

GImageLine::GImageLine()
{
	pImg = NULL;
	fDim = 1.0f;
}

GImageLine::GImageLine( GObject * parent, const char * filename, float x, float y )
{
	ASSERT(parent!=NULL);

	pImg = NULL;
	fDim = 1.0f;

	SetFile(filename);

	float w = 10;
	float h = 10;
	if (pImg)
	{
		w = pImg->width();
		h = pImg->height();
	}

	plbegin = new GAnchorPoint(this, x-w/2, y-h/2);
	plend = new GAnchorPoint(this, x+w/2, y+h/2);
	pmid = new GMidPoint(this);


	parent->AddChild(this);
	OnInit();
}

GImageLine::~GImageLine()
{
	if (pImg)
	{
		delete pImg;
	}
}

bool GImageLine::SetFile( const char * filename )
{
	strfilename = filename;
	CallRedrawModify();

	return true;
}

GObject * GImageLine::CreateNewClone( GObject * pNewParent/*=NULL*/, GObject * pBeforeObj/*=NULL*/ )
{
	_GOBJ_CLONE_PRE(GImageLine);
	_GOBJ_CLONE_POST();
}

bool GImageLine::CloneData( GObject * pClone, GObject * pNewParent, bool bNoRelationship/*=true*/ )
{
	if (super::CloneData(pClone, pNewParent, bNoRelationship))
	{
		GImageLine * pImageLine = (GImageLine *)pClone;

		pImageLine->strfilename = strfilename;
		return true;
	}
	return false;
}

void GImageLine::OnRender( int iHighlightLevel/* =0 */ )
{
	super::OnRender(iHighlightLevel);

	if (!pImg && strfilename.length())
	{
		pImg = new QImage(strfilename.c_str());
	}

	if (pImg && !iHighlightLevel)
	{
		RenderHelper::getInstance().RenderImage(pImg, plbegin->getX(), plbegin->getY(), plend->getX()-plbegin->getX(), plend->getY()-plbegin->getY(), ARGB((0xff*fDim), 0xff, 0xff, 0xff));
	}
}

void GImageLine::SetDim( float fDimPercentage )
{
	if (fDimPercentage < 0)
	{
		fDimPercentage = 0;
	}
	else if (fDimPercentage > 1)
	{
		fDimPercentage = 1;
	}
	fDim = fDimPercentage;
	CallRedrawModify();
}

/************************************************************************/
/* GSAInfo                                                              */
/************************************************************************/

int GSAInfo::GetLeftMul()
{
	if (nSAFlag & GSA_RIGHT)
	{
		return -1;
	}
	return 1;
}

void GSAInfo::SetSA( float fsa, int flag )
{
	fSA = fsa;
	nSAFlag = flag;
}

void GSAInfo::ClearSA()
{
	SetSA(0, 0);
}

float GSAInfo::GetMulSA()
{
	return fSA * GetLeftMul();
}

int GSAInfo::GetFlag()
{
	return nSAFlag;
}

float GSAInfo::GetRawSA()
{
	return fSA;
}
