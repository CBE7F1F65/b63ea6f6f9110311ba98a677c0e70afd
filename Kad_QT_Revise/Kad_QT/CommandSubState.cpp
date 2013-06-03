#include "StdAfx.h"
#include "CommandSubState.h"
#include "GObjectPicker.h"

CSS_PickVoid::CSS_PickVoid( FillCommandParamInfo * pToInfo, FillCommandParamInfo * xParamInfo, FillCommandParamInfo * yParamInfo )
{
	ASSERT(pToInfo);
	ASSERT(xParamInfo);
	ASSERT(yParamInfo);

	SetToFCP(pToInfo);
	xFCP = *xParamInfo;
	yFCP = *yParamInfo;
}

CSS_PickVoid::~CSS_PickVoid(void)
{
}

bool CSS_PickVoid::ExcuteSubState()
{
	return true;
}

bool CSS_PickVoid::SetPickParam( int iret, bool bReady )
{
	if (bReady)
	{
		CommandTemplate * pct = getCommandTemplate();
		GObjectPicker * pgp = &GObjectPicker::getInstance();
		GObject * pPicked = pgp->GetPickedObj();

		if (pPicked->isPoint())
		{
			pct->pcommand->SetParamI(xFCP.index, pPicked->getID());
		}
		pct->pcommand->SetParamX(xFCP.index, pgp->GetPickX_C(), xFCP.fillprompt);
		pct->pcommand->SetParamY(yFCP.index, pgp->GetPickY_C(), yFCP.fillprompt);

		pct->pcommand->StepTo(tofcp.tostep, tofcp.wantprompt, tofcp.pushback);

		return true;
	}
	return false;
}