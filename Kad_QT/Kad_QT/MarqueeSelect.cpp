#include "StdAfx.h"
#include "MarqueeSelect.h"
#include "ColorManager.h"
#include "Command.h"
#include "GObjectPicker.h"
#include "MainInterface.h"
#include "RenderHelper.h"
#include "GObjectManager.h"
#include "GLine.h"
#include "MathHelper.h"

#define MARQSTATE_NONE				0x00
#define MARQSTATE_LEFTKEYDOWN		0x01
#define MARQSTATE_BEGAN				0x02
#define MARQSTATE_TERMINAL			0x03

#define MARQMOVESTATE_NONE			0x00
#define MARQMOVESTATE_BEGAN			0x01

#define MARQNOMOVE_POINT	0x01
#define MARQNOMOVE_LINE		0x02
#define MARQNOMOVE_PIECE	0x03

bool staticPickFilterCallback(GObject * pObj)
{
	return MarqueeSelect::getInstance().PickFilterCallback(pObj); 
}

MarqueeSelect::MarqueeSelect(void)
{
}


MarqueeSelect::~MarqueeSelect(void)
{
}

void MarqueeSelect::DeSelect( GObject * pObj )
{
	for (list<GObject *>::iterator it=selectednodes.begin(); it!=selectednodes.end();)
	{
		if (*it == pObj)
		{
			it = selectednodes.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void MarqueeSelect::DeSelectAll()
{
	selectednodes.clear();
}

void MarqueeSelect::AddSelect( GObject * pObj, int level )
{
	if (!pObj)
	{
		return;
	}
	if (pObj->isSlaveToLine())
	{
		pObj = pObj->getLine();
	}
	else if (pObj->isSlaveToPiece())
	{
		pObj = pObj->getPiece();
	}
	if (!pObj)
	{
		return;
	}

    switch (level)
    {
    case MARQNOMOVE_POINT:
        if (!pObj->isRepresentablePoint())
        {
            return;
        }
        break;
    case MARQNOMOVE_LINE:
        if (!pObj->isRepresentableLine())
        {
            return;
        }
        break;
    case MARQNOMOVE_PIECE:
        if (!pObj->isRepresentablePiece())
        {
            return;
        }
        break;
    default:
        if (!pObj->isRepresentablePoint() && !pObj->isRepresentableLine() && !pObj->isRepresentablePiece())
        {
            return;
        }
    }

	
	for (list<GObject *>::iterator it=selectednodes.begin(); it!=selectednodes.end();)
	{
		if ((*it) == pObj)
		{
			return;
		}
		//////////////////////////////////////////////////////////////////////////
		if (pObj->isPoint() && (*it)->isPoint()
			&& ((GPoint *)pObj)->isMergeWith((GPoint *)*it))
		{
			it = selectednodes.erase(it);
		}
		//////////////////////////////////////////////////////////////////////////
		else if (pObj->isAnchorPoint() && ((GAnchorPoint *)pObj)->GetHandle() == (*it))
		{
			it = selectednodes.erase(it);
		}
		else if (pObj->isHandlePoint() && ((GHandlePoint *)pObj)->GetAnchor() == (*it))
		{
			it = selectednodes.erase(it);
		}
		else
		{
			++it;
		}
	}
	/*
	PushSelectCling(pObj);

	if (pObj->getClingTo())
	{
		AddSelect(pObj->getClingTo());
		return;
	}
	*/
	selectednodes.push_back(pObj);
}

void MarqueeSelect::Update()
{
	CheckValid();

	Command * pcommand = &Command::getInstance();
	int comm = pcommand->GetCurrentCommand();
	if (comm/* && !pcommand->IsCommandPush(comm)*/)
	{
		marqueestate = MARQSTATE_NONE;
		itemmovestate = MARQMOVESTATE_NONE;
		return;
	}

	MainInterface * pmain = &MainInterface::getInstance();
	GObjectPicker * pgp = &GObjectPicker::getInstance();
	GUICoordinate * pguic = &GUICoordinate::getInstance();

	float mousex_c = pguic->GetCursorX_C();
	float mousey_c = pguic->GetCursorY_C();

	bool mlkeydown = pmain->hge->Input_GetDIMouseKey(pmain->cursorleftkeyindex, DIKEY_DOWN);
	bool mlkeynotpressed = !pmain->hge->Input_GetDIMouseKey(pmain->cursorleftkeyindex);

	if (mlkeydown)
	{
		lastmx_c = mousex_c;
		lastmy_c = mousey_c;

		beginx_c = pguic->GetCursorX_C();
		beginy_c = pguic->GetCursorY_C();
		marqueestate = MARQSTATE_LEFTKEYDOWN;
	}
	if (mlkeynotpressed)
	{
		if (marqueestate == MARQSTATE_LEFTKEYDOWN)
		{
			marqueestate = MARQSTATE_NONE;
		}
		itemmovestate = MARQMOVESTATE_NONE;
	}
	
	if (marqueestate != MARQSTATE_BEGAN)
	{
		int iret = pgp->PickPoint(staticPickFilterCallback);
		if (!mlkeynotpressed)
		{
			// moving select or to marquee began
			if (marqueestate == MARQSTATE_LEFTKEYDOWN)
			{
				if (!itemmovestate && !pgp->GetPickedObj())
				{
					if (mousex_c != beginx_c || mousey_c != beginy_c)
					{
						marqueestate = MARQSTATE_BEGAN;
					}
				}
				else
				{
					marqueestate = MARQSTATE_NONE;
				}
			}
			// Moving select
			if (marqueestate != MARQSTATE_BEGAN)
			{
//				int iret = pgp->PickPoint(/*restrict*/);

				if (mlkeydown)
				{
					GObject * pObj = pgp->GetPickedObj();
					if (!pObj || !CheckObjInSelection(pObj, true, false, true))
					{
						DeSelectAll();
					}
					if (pObj)
					{
						AddSelect(pObj);
						lastmx_c = mousex_c = pgp->GetPickX_C();
						lastmy_c = mousey_c = pgp->GetPickY_C();
					}
				}
				if (itemmovestate == MARQMOVESTATE_BEGAN)
				{
					pgp->SetCheckMouseDown(true);
					if (pgp->GetSnappedState())
					{
						mousex_c = pgp->GetPickX_C();
						mousey_c = pgp->GetPickY_C();
					}
					MoveSelected(mousex_c-lastmx_c, mousey_c-lastmy_c);
				}
				else 
				{
					GObject * pPicked = pgp->GetPickedObj();
					if (pPicked)
					{
						if (CheckObjInSelection(pPicked))
						{
							itemmovestate = MARQMOVESTATE_BEGAN;
						}
					}
				}
			}
		}
	}
		/*
		int iret = pgp->PickPoint();
		GObject * pMDPicked = pgp->GetMDownPickedObj();
		if (!(iret & PICKSTATE_READY))
		{
			if (marqueestate == MARQSTATE_LEFTKEYDOWN)
			{
				if (pMDPicked)
				{
					marqueestate = MARQSTATE_TERMINAL;
				}
				else if (mousex_c != beginx_c || mousey_c != beginy_c)
				{
					marqueestate = MARQSTATE_BEGAN;
				}
			}
			if (mlkeydown)
			{
				beginx_c = pguic->GetCursorX_C();
				beginy_c = pguic->GetCursorY_C();
				marqueestate = MARQSTATE_LEFTKEYDOWN;
			}

			// Try to move with picked
			if (pMDPicked)
			{
				if (mlkeydown)
				{
					AddSelect(pMDPicked);
				}
				if (itemmovestate == MARQMOVESTATE_BEGAN)
				{
					MoveSelected(mousex_c-lastmx_c, mousey_c-lastmy_c);
					if (mlkeynotpressed)
					{
						itemmovestate = MARQMOVESTATE_TERMINAL;
					}
				}
				else 
				{
					GObject * pPicked = pgp->GetPickedObj();
					if (pPicked)
					{
						if (CheckObjInSelection(pPicked))
						{
							itemmovestate = MARQMOVESTATE_BEGAN;
						}
					}
				}
			}
		}
		else
		{
			if (!pMDPicked)
			{
				DeSelectAll();
			}
			else
			{
				AddSelect(pMDPicked);
			}
		}
	*/
	if (marqueestate == MARQSTATE_BEGAN)
	{
		if (mlkeynotpressed)
		{
			marqueestate = MARQSTATE_NONE;
			DeSelectAll();
			GObject * pObj = GObjectManager::getInstance().GetMainBaseNode();
			float lx = beginx_c < endx_c ? beginx_c : endx_c;
			float ty = beginy_c < endy_c ? beginy_c : endy_c;
			float rx = beginx_c > endx_c ? beginx_c : endx_c;
			float by = beginy_c > endy_c ? beginy_c : endy_c;
			beginx_c = lx;
			beginy_c = ty;
			endx_c = rx;
			endy_c = by;

			if (pmain->hge->Input_GetDIKey(DIK_LSHIFT) || pmain->hge->Input_GetDIKey(DIK_RSHIFT))
			{
				CheckMarqueeSelect_Pt(pObj);
			}
			else
			{
				CheckMarqueeSelect_Line(pObj);
			}
		}
		endx_c = pguic->GetCursorX_C();
		endy_c = pguic->GetCursorY_C();

	}
	lastmx_c = mousex_c;
	lastmy_c = mousey_c;
}

void MarqueeSelect::Render()
{
	CheckValid();
	for (list<GObject *>::iterator it=selectednodes.begin(); it!=selectednodes.end(); ++it)
	{
		(*it)->CallRender(LINECOLOR_ACTIVE);
	}
	if (marqueestate == MARQSTATE_BEGAN)
	{
		DWORD col = ColorManager::getInstance().GetMarqueeLineColor();
		RenderHelper::getInstance().SetLineStyle(RHLINESTYLE_DOTTEDLINE);
		RenderHelper::getInstance().RenderRect(beginx_c, beginy_c, endx_c-beginx_c, endy_c-beginy_c, col);
		RenderHelper::getInstance().SetLineStyle();
	}
}

void MarqueeSelect::CheckMarqueeSelect_Line( GObject * pObj )
{
	if (!pObj)
	{
		return;
	}
	if (pObj->isRepresentableLine())
	{
		if (((GLine *)pObj)->CheckIntersectWithRect(beginx_c, beginy_c, endx_c, endy_c))
		{
            AddSelect(pObj, MARQNOMOVE_LINE);
//			selectednodes.push_back(pObj);
		}
	}
	for (list<GObject *>::iterator it=pObj->getChildren()->begin(); it!=pObj->getChildren()->end(); ++it)
	{
		CheckMarqueeSelect_Line(*it);
	}
}

void MarqueeSelect::CheckMarqueeSelect_Pt( GObject * pObj )
{
	if (!pObj)
	{
		return;
	}
	if (pObj->isRepresentablePoint())
	{
		if (MathHelper::getInstance().PointInRect(pObj->getX(), pObj->getY(), beginx_c, beginy_c, endx_c-beginx_c, endy_c-beginy_c))
		{
			if (!pObj->isHandlePoint())
			{
                AddSelect(pObj, MARQNOMOVE_POINT);
//				selectednodes.push_back(pObj);
			}
		}
	}
	for (list<GObject *>::iterator it=pObj->getChildren()->begin(); it!=pObj->getChildren()->end(); ++it)
	{
		CheckMarqueeSelect_Pt(*it);
	}
}

void MarqueeSelect::MoveSelected( float movedx_c, float movedy_c )
{
	if (selectednodes.empty())
	{
		return;
	}

	nomoveflag = 0;
	for (list<GObject *>::iterator it=selectednodes.begin(); it!=selectednodes.end(); ++it)
	{
		if ((*it)->isRepresentableLine()/* || (*it)->isSlaveToLine()*/)
		{
			nomoveflag = MARQNOMOVE_POINT;
		}
		if ((*it)->isRepresentablePiece()/* || (*it)->isSlaveToPiece()*/)
		{
			nomoveflag = MARQNOMOVE_LINE;
		}
	}

	int nMoveActionID = GObjectManager::getInstance().GetNextMoveActionID();
	for (list<GObject *>::iterator it=selectednodes.begin(); it!=selectednodes.end(); ++it)
	{
		if ((*it)->isRepresentablePoint())
		{
			DoMovePoint((GPoint *)(*it), movedx_c, movedy_c, nMoveActionID);
		}
		else if ((*it)->isRepresentableLine())
		{
			DoMoveLine((GLine *)(*it), movedx_c, movedy_c, nMoveActionID);
		}
		else if ((*it)->isRepresentablePiece())
		{
			DoMovePiece((GPiece *)(*it), movedx_c, movedy_c, nMoveActionID);
		}
	}
}

bool MarqueeSelect::CheckObjInSelection( GObject * pObj, bool bFindAncestor/*=false*/, bool bFindUpperClass/*=false*/, bool bFindMerge/*=false*/, bool bFindCling/*=false*/ )
{
	if (!pObj)
	{
		return false;
	}
	if (pObj->isSlaveToLine())
	{
		pObj = pObj->getLine();
	}
	else if (pObj->isSlaveToPiece())
	{
		pObj = pObj->getPiece();
	}
	
	GPoint * pItPoint = NULL;
	GLine * pItLine = NULL;
	GPiece * pItPiece = NULL;
	GObject * pItObj = NULL;

	for (list<GObject *>::iterator it=selectednodes.begin(); it!=selectednodes.end(); ++it)
	{
		pItObj = *it;
		if (pItObj == pObj)
		{
			return true;
		}

		if (pItObj->isPoint()) { pItPoint = (GPoint *)pItObj; }
		else if (pItObj->isLine()) { pItLine = (GLine *)pItObj; }
		else if (pItObj->isPiece()) { pItPiece = (GPiece *)pItObj; }

		if (bFindAncestor)
		{
			if (pItObj->isAncestorOf(pObj))
			{
				return true;
			}
		}
		if (bFindUpperClass)
		{
			if (pItPoint)
			{
				if (pItPoint->getLine() == pObj)
				{
					return true;
				}
			}
			if (pItLine)
			{
				if (pItLine->getPiece() == pObj)
				{
					return true;
				}
			}
		}
		if (bFindMerge)
		{
			if (pItPoint)
			{
				if (pObj->isPoint())
				{
					GPoint * pPoint = (GPoint *)pObj;
					if (pPoint->isMergeWith(pItPoint))
					{
						return true;
					}
				}
				if (bFindAncestor || bFindUpperClass)
				{
					if (!pItPoint->getMergeWith()->empty())
					{
						for (list<GPoint *>::iterator jt=pItPoint->getMergeWith()->begin(); jt!=pItPoint->getMergeWith()->end(); ++jt)
						{
							if (bFindAncestor)
							{
								if ((*jt)->isAncestorOf(pObj))
								{
									return true;
								}
							}
							if (bFindUpperClass)
							{
								if ((*jt)->getLine() == pObj)
								{
									return true;
								}
							}
						}
					}
				}
			}
		}
		if (bFindCling)
		{
			if (pItPoint)
			{
				if (pObj->isLine())
				{
					GLine * pLine = (GLine *)pItLine;

					if (pItPoint->isClingTo(pLine))
					{
						return true;
					}

				}
			}
		}
	}
	return false;
}

void MarqueeSelect::DoMovePoint( GPoint * pPoint, float movedx_c, float movedy_c, int nMoveActionID )
{
	if (pPoint->isSlaveToLine())
	{
		DoMoveLine((GLine *)pPoint->getLine(), movedx_c, movedy_c, nMoveActionID);
	}
	else
	{
		if (nomoveflag < MARQNOMOVE_POINT)
		{
			pPoint->CallMoveByOffset(movedx_c, movedy_c, true, nMoveActionID);
		}
	}
}

void MarqueeSelect::DoMoveLine( GLine * pLine, float movedx_c, float movedy_c, int nMoveActionID )
{
	if (pLine->isSlaveToPiece())
	{
		DoMovePiece((GPiece *)pLine->getPiece(), movedx_c, movedy_c, nMoveActionID);
	}
	else
	{
		if (nomoveflag < MARQNOMOVE_LINE)
		{
			pLine->CallMoveByOffset(movedx_c, movedy_c, true, nMoveActionID);
		}
	}
}

void MarqueeSelect::DoMovePiece( GPiece * pPiece, float movedx_c, float movedy_c, int nMoveActionID )
{
	//ToDo!!
	if (nomoveflag < MARQNOMOVE_PIECE)
	{
		pPiece->CallMoveByOffset(movedx_c, movedy_c, true, nMoveActionID);
	}
}

void MarqueeSelect::CheckValid()
{
	GObject * pObj = GObjectManager::getInstance().GetMainBaseNode();
	for (list<GObject *>::iterator it=selectednodes.begin(); it!=selectednodes.end();)
	{
		if (!pObj->isAncestorOf(*it))
		{
			it = selectednodes.erase(it);
		}
		else
		{
			++it;
		}
	}
}

bool MarqueeSelect::PickFilterCallback( GObject * pObj )
{
	if (itemmovestate == MARQMOVESTATE_BEGAN)
	{
		if (pObj->isHandlePoint())
		{
			return false;
		}
		return !CheckObjInSelection(pObj, true, true, true, true);
	}
	return true;
}

void MarqueeSelect::OnDeleteNode( GObject * pObj )
{
	for (list<GObject *>::iterator it=selectednodes.begin(); it!=selectednodes.end();)
	{
		if ((*it) == pObj)
		{
			it = selectednodes.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void MarqueeSelect::PushSelectCling( GObject * pObj )
{
	/*
	if (!pObj)
	{
		return;
	}
	if (!pObj->getChildren()->empty())
	{
		for (list<GObject *>::iterator it=pObj->getChildren()->begin(); it!=pObj->getChildren()->end(); ++it)
		{
			PushSelectCling(*it);
		}
	}
	if (pObj->getClingBy()->empty())
	{
		return;
	}
	for (list<GObject *>::iterator it=pObj->getClingBy()->begin(); it!=pObj->getClingBy()->end(); ++it)
	{
		selectednodes.push_back(*it);
	}
	*/
}