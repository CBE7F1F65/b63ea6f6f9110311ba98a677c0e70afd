#include "StdAfx.h"
#include "GLine.h"

#include "RenderHelper.h"
#include "RenderTargetManager.h"
#include "ColorManager.h"
#include "Main.h"
#include "Command.h"

GLine::GLine(void)
{
	plbegin = NULL;
	plend = NULL;
	pmid = NULL;
}

GLine::GLine( GObject * _parent )
{
	ASSERT(_parent!=NULL);
	_parent->AddChild(this);
	OnInit();
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

GStraightLine::GStraightLine()
{
}

GStraightLine::GStraightLine( GObject * _parent )
{
	ASSERT(_parent!=NULL);
	_parent->AddChild(this);
	OnInit();
}

GStraightLine::~GStraightLine()
{
}

void GStraightLine::OnInit()
{
	ASSERT(pParent!=NULL);

	plbegin = new GEndPoint();
	plend = new GEndPoint();

	pmid = new GMidPoint();

	this->AddChild(plbegin);
	this->AddChild(plend);
	this->AddChild(pmid);
}

void GStraightLine::UpdateMidPoint()
{
	if (pmid && plbegin && plend)
	{
		pmid->SetPosition((plbegin->x+plend->x)/2.0f, (plbegin->y+plend->y)/2.0f);
	}
}


void GStraightLine::OnRender( int iHighlightLevel/*=0*/ )
{
	if (plbegin && plend)
	{
		DWORD col = getLineColor(iHighlightLevel);
		RenderHelper::getInstance().RenderLine(plbegin->x, plbegin->y, plend->x, plend->y, col);
	}
}

bool GStraightLine::Clone( GObject * pNewParent )
{
	_GOBJ_CLONE_PRE(GStraightLine);
	_GOBJ_CLONE_POST_NORET();
	list<GObject *>::reverse_iterator it=_node->listChildren.rbegin();
	_node->plbegin = (GPoint *)*it;
	++it;
	_node->plend = (GPoint *)*it;
	++it;
	_node->pmid = (GPoint *)*it;
	return true;
}

bool GStraightLine::CheckNearTo( float px, float py, float r, float *plx, float *ply )
{
	float xl, yt, xr, yb;
	GetBoundingBox(&xl, &yt, &xr, &yb);
	if (!MathHelper::getInstance().PointInRect(px, py, xl-r, yt-r, xr-xl+2*r, yb-yt+2*r))
	{
		return false;
	}
	float nx, ny;
	float dist = MathHelper::getInstance().NearestPointOnLine(px, py, plbegin->x, plbegin->y, plend->x, plend->y, &nx, &ny);
	if (plx)
	{
		*plx = nx;
	}
	if (ply)
	{
		*ply = ny;
	}
	if (dist < r)
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
