#include "StdAfx.h"
#include "GLine.h"

#include "RenderHelper.h"
#include "RenderTargetManager.h"
#include "ColorManager.h"

GLine::GLine(void)
{
	plbegin = NULL;
	plend = NULL;
	pmid = NULL;
}

GLine::GLine( GObject * _parent )
{
	assert(_parent!=NULL);
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
		plbegin->SetPosition(xb, yb);
	}
	if (plend)
	{
		plend->SetPosition(xe, ye);
	}
}
GStraightLine::GStraightLine()
{
}

GStraightLine::GStraightLine( GObject * _parent )
{
	assert(_parent!=NULL);
	_parent->AddChild(this);
	OnInit();
}

GStraightLine::~GStraightLine()
{
}

void GStraightLine::OnInit()
{
	assert(parent!=NULL);

	plbegin = new GEndPoint();
	plend = new GEndPoint();

	pmid = new GPoint();
	pmid->SetMotifyParent(false);

	this->AddChild(plbegin);
	this->AddChild(plend);
	this->AddChild(pmid);
}

void GStraightLine::OnEnter()
{

}

void GStraightLine::UpdateMidPoint()
{
	if (pmid && plbegin && plend)
	{
		pmid->SetPosition((plbegin->x+plend->x)/2.0f, (plbegin->y+plend->y)/2.0f);
	}
}

void GStraightLine::OnUpdate()
{
	if (bModified)
	{
		UpdateMidPoint();
	}
	GObject::OnUpdate();
}

void GStraightLine::OnRender()
{
	if (plbegin && plend)
	{
		RenderHelper::getInstance().RenderLine(plbegin->x, plbegin->y, plend->x, plend->y, ColorManager::GetLayerLineColor());
	}
	GObject::OnRender();
}