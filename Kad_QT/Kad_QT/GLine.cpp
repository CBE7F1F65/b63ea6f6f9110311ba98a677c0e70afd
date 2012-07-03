#include "StdAfx.h"
#include "GLine.h"

#include "RenderHelper.h"
#include "RenderTargetManager.h"
#include "ColorManager.h"
#include "MainInterface.h"
#include "Command.h"
/************************************************************************/
/* GLINE                                                                */
/************************************************************************/
GLine::GLine(void)
{
	plbegin = NULL;
	plend = NULL;
	pmid = NULL;
}

GLine::~GLine(void)
{
}

void GLine::SetBeginEnd( float xb, float yb, float xe, float ye )
{
	if (plbegin)
	{
		plbegin->CallMoveTo(xb, yb, false);
	}
	if (plend)
	{
		plend->CallMoveTo(xe, ye, false);
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

bool GLine::MoveTo( float newx, float newy, bool bTry )
{
	if (!canMove())
	{
		return false;
	}

	float xoffset = newx - getX();
	float yoffset = newy - getY();

	ToggleTryMoveState(bTry);

	bool bret;
	bret = plbegin->CallMoveByOffset(xoffset, yoffset, false);
	if (!bret)
	{
		return false;
	}
	bret = plend->CallMoveByOffset(xoffset, yoffset, false);
//	UpdateMidPoint();
	return bret;
}

bool GLine::CallMoveTo( float newx, float newy, bool bTry )
{
	if (!clingByList.empty())
	{
		float xoffset = newx - getX();
		float yoffset = newy - getY();
		for (list<GPoint *>::iterator it=clingByList.begin(); it!=clingByList.end(); ++it)
		{
			(*it)->CallMoveByOffset(xoffset, yoffset, bTry);
		}
	}
	return MoveTo(newx, newy, bTry);
}

void GLine::OnModify()
{
    if (!listChildren.empty())
    {
        UpdateMidPoint();
    }
	GObject::OnModify();
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
	DeclingByOther();
}

bool GLine::AddClingBy( GPoint * pPoint )
{
	ASSERT(pPoint);
	for (list<GPoint *>::iterator it=clingByList.begin(); it!=clingByList.end(); ++it)
	{
		if (*it == pPoint)
		{
			DASSERT(true);
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

GObject * GLine::getPiece()
{
	GObject * pObj = pParent;
	while (pObj)
	{
		if (pObj->isPiece())
		{
			return pObj;
		}
		pObj = pObj->getParent();
	}
	return NULL;
};
/************************************************************************/
/* GSTRAIGHTLINE                                                        */
/************************************************************************/
GStraightLine::GStraightLine()
{
}

GStraightLine::GStraightLine( GObject * parent, PointF2D pb, PointF2D pe )
{
	ASSERT(parent!=NULL);

	plbegin = new GAnchorPoint(this, pb.x, pb.y);
	plend = new GAnchorPoint(this, pe.x, pe.y);
	pmid = new GMidPoint(this);

	parent->AddChild(this);
	OnInit();
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
	RenderHelper::getInstance().RenderLine(plbegin->getX(), plbegin->getY(), plend->getX(), plend->getY(), col);
}

bool GStraightLine::Clone( GObject * pNewParent )
{
	_GOBJ_CLONE_PRE(GStraightLine);

    _node->plbegin = NULL;
    _node->plend = NULL;

	_GOBJ_CLONE_POST_NORET();
	list<GObject *>::reverse_iterator it=_node->listChildren.rbegin();
	_node->plbegin = (GAnchorPoint *)*it;
	++it;
	_node->plend = (GAnchorPoint *)*it;
	++it;
	_node->pmid = (GMidPoint *)*it;
	return true;
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

void GStraightLine::GetBoundingBox( float *xl, float *yt, float *xr, float * yb )
{
	if (!xl || !yt || !xr || !yb)
	{
		return;
	}
	*xl = min(plbegin->getX(), plend->getX());
	*xr = max(plbegin->getX(), plend->getX());
	*yt = min(plbegin->getY(), plend->getY());
	*yb = max(plbegin->getY(), plend->getY());
}

bool GStraightLine::CheckIntersectWithRect( float xl, float yt, float xr, float yb )
{
	return MathHelper::getInstance().LinePartialInRect(plbegin->getX(), plbegin->getY(), plend->getX(), plend->getY(), xl, yt, xr, yb);
}

bool GStraightLine::CheckIntersectStraightStraight( GStraightLine * pLine, list<PointF2D> *pPoints )
{
	float ix, iy;
	if (MathHelper::getInstance().LineSegmentIntersect(
		plbegin->getX(), plbegin->getY(), plend->getX(), plend->getY(),
		pLine->plbegin->getX(), pLine->plbegin->getY(), pLine->plend->getX(), pLine->plend->getY(),
		&ix, &iy))
	{
		if (pPoints)
		{
			pPoints->push_back(PointF2D(ix, iy));
		}
		return true;
	}
	return false;
}
/************************************************************************/
/* GBEZEIERLINE                                                         */
/************************************************************************/
GBezierLine::GBezierLine()
{
	ASSERT(true);
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
			MathHelper::getInstance().CalculateBezier(plbegin->GetPointF2D(), plbegin->GetHandle()->GetPointF2D(), plend->GetHandle()->GetPointF2D(), plend->GetPointF2D(), 0.5f, &pq);
			pmid->SetPosition(pq.x, pq.y);
		}
	}
}

void GBezierLine::OnModify()
{
	GStraightLine::OnModify();

    if (!listChildren.empty())
    {
        if (plbegin && plend)
        {
            if (!isStraightLine())
            {
                bsinfo.ResetPoints(plbegin->GetPointF2D(), plbegin->GetHandle()->GetPointF2D(), plend->GetHandle()->GetPointF2D(), plend->GetPointF2D(), MainInterface::getInstance().GetPrecision());
            }
		}
	}
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
		RenderHelper::getInstance().RenderBezierByInfo(&bsinfo, col);
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

void GBezierLine::GetBoundingBox( float *xl, float *yt, float *xr, float * yb )
{
	if (isStraightLine())
	{
		GStraightLine::GetBoundingBox(xl, yt, xr, yb);
		return;
	}
	else
	{
		if (!bsinfo.GetBoundingBox(xl, yt, xr, yb))
		{
			ASSERT(true);
		}
	}
}

bool GBezierLine::Clone( GObject * pNewParent )
{
    _GOBJ_CLONE_PRE(GBezierLine);

    _node->plbegin = NULL;
    _node->plend = NULL;
    _node->pmid = NULL;

	_GOBJ_CLONE_POST_NORET();
	list<GObject *>::reverse_iterator it=_node->listChildren.rbegin();
	_node->plbegin = (GAnchorPoint *)*it;
	++it;
	_node->plend = (GAnchorPoint *)*it;
	++it;
	_node->pmid = (GMidPoint *)*it;
	return true;	
}

void GBezierLine::SetBeginEnd( float xb, float yb, float xe, float ye )
{
	GLine::SetBeginEnd(xb, yb, xe, ye);
	SetBeginHandlePos(xb, yb);
	SetEndHandlePos(xe, ye);
}

void GBezierLine::SetBeginHandlePos( float x, float y )
{
	plbegin->SetHandlePosition(x, y);
}

void GBezierLine::SetEndHandlePos( float x, float y )
{
	plend->SetHandlePosition(x, y);
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
	return false;
}

bool GBezierLine::CheckIntersectBezierBezier( GBezierLine * pLine, list<PointF2D> *pPoints )
{
	return false;
}