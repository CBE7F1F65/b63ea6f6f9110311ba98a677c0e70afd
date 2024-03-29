#include "StdAfx.h"
#include "GLine.h"

#include "RenderHelper.h"
#include "RenderTargetManager.h"
#include "ColorManager.h"
#include "Main.h"
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
		plbegin->MoveTo(xb, yb, false);
	}
	if (plend)
	{
		plend->MoveTo(xe, ye, false);
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
	bret = plbegin->MoveByOffset(xoffset, yoffset, false);
	if (!bret)
	{
		return false;
	}
	bret = plend->MoveByOffset(xoffset, yoffset, false);
//	UpdateMidPoint();
	return bret;
}

void GLine::OnModify()
{
	UpdateMidPoint();
	GObject::OnModify();
}

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
		pmid->SetPosition((plbegin->x+plend->x)/2.0f, (plbegin->y+plend->y)/2.0f);
	}
}


void GStraightLine::OnRender( int iHighlightLevel/*=0*/ )
{
	DWORD col = getLineColor(iHighlightLevel);
	RenderHelper::getInstance().RenderLine(plbegin->x, plbegin->y, plend->x, plend->y, col);
}

bool GStraightLine::Clone( GObject * pNewParent )
{
	_GOBJ_CLONE_PRE(GStraightLine);
	_GOBJ_CLONE_POST_NORET();
	list<GObject *>::reverse_iterator it=_node->listChildren.rbegin();
	_node->plbegin = (GAnchorPoint *)*it;
	++it;
	_node->plend = (GAnchorPoint *)*it;
	++it;
	_node->pmid = (GMidPoint *)*it;
	return true;
}

bool GStraightLine::CheckNearTo( float px, float py, float r, float *plx, float *ply )
{
//	float xl, yt, xr, yb;
//	GetBoundingBox(&xl, &yt, &xr, &yb);
//	if (!MathHelper::getInstance().PointInRectTwoPoint(px, py, xl-r, yt-r, xr-xl+2*r, yb-yt+2*r))
	if (!MathHelper::getInstance().PointInRectTwoPoint(px, py, plbegin->x, plbegin->y, plend->x, plend->y, r))
	{
		return false;
	}
	float nx, ny;
	float distpow2 = MathHelper::getInstance().NearestPointOnStraightLinePow2(px, py, plbegin->x, plbegin->y, plend->x, plend->y, &nx, &ny);
	if (plx)
	{
		*plx = nx;
	}
	if (ply)
	{
		*ply = ny;
	}
	if (distpow2 < r*r)
	{
		return true;
	}
	return false;
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
	return MathHelper::getInstance().LinePartialInRect(plbegin->x, plbegin->y, plend->x, plend->y, xl, yt, xr, yb);
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
	if (plbegin && plend)
	{
		if (!isStraightLine())
		{
			bsinfo.ResetPoints(plbegin->GetPointF2D(), plbegin->GetHandle()->GetPointF2D(), plend->GetHandle()->GetPointF2D(), plend->GetPointF2D(), MainInterface::getInstance().GetPrecision());
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

bool GBezierLine::CheckNearTo( float px, float py, float r, float *plx, float *ply )
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