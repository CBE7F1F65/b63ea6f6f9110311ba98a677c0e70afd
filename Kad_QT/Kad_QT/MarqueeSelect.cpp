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
#include "MarkingManager.h"

#define MARQSTATE_NONE				0x00
#define MARQSTATE_LEFTKEYDOWN		0x01
#define MARQSTATE_BEGAN				0x02
#define MARQSTATE_TERMINAL			0x03

#define MARQMOVESTATE_NONE			0x00
#define MARQMOVESTATE_BEGAN			0x01

#define MARQNOMOVE_POINT	0x01
#define MARQNOMOVE_LINE		0x02
#define MARQNOMOVE_PIECE	0x03


MarqueeSelect::MarqueeSelect(void)
{
	pBeginObj = NULL;
	pMarkingOffset = NULL;
	pMarkingLine = NULL;
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
	bool bctrldown = pmain->hge->Input_GetDIKey(DIK_LCONTROL) || pmain->hge->Input_GetDIKey(DIK_RCONTROL);
	bool baltdown = pmain->hge->Input_GetDIKey(DIK_LMENU) || pmain->hge->Input_GetDIKey(DIK_RMENU);

	if (mlkeydown)
	{
		lastmx_c = mousex_c;
		lastmy_c = mousey_c;

		beginx_c = pgp->GetPickX_C();//pguic->GetCursorX_C();
		beginy_c = pgp->GetPickY_C();//pguic->GetCursorY_C();

		pBeginObj = pgp->GetPickedObj();

		marqueestate = MARQSTATE_LEFTKEYDOWN;
	}
	if (mlkeynotpressed)
	{
		if (marqueestate == MARQSTATE_LEFTKEYDOWN)
		{
			marqueestate = MARQSTATE_NONE;
		}
		if (itemmovestate == MARQMOVESTATE_BEGAN)
		{
			EndMove();
		}
		itemmovestate = MARQMOVESTATE_NONE;
		pBeginObj = NULL;
	}
	
	if (marqueestate != MARQSTATE_BEGAN)
	{
		int iret = pgp->PickPoint(staticMarqPickFilterCallback);

        if (itemmovestate==MARQMOVESTATE_NONE && (baltdown || pmain->hge->Input_GetDIMouseKey(pmain->cursorrightkeyindex, DIKEY_DOWN)))
        {
            GObject * pPickedObj = pgp->GetPickedObj();
            MainInterface::getInstance().CallShowNodeInfo(pPickedObj, !baltdown);
        }
        else
        {
            MainInterface::getInstance().CallShowNodeInfo(NULL);
        }

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
					if (!bctrldown && !baltdown && (!pBeginObj || !CheckObjInSelection(pBeginObj, true, false, true)))
					{
						DeSelectAll();
					}
					if (pBeginObj)
					{
						if (baltdown)
						{
							DeSelect(pBeginObj);
						}
						else
						{
							AddSelect(pBeginObj);
							lastmx_c = mousex_c = pgp->GetPickX_C();
							lastmy_c = mousey_c = pgp->GetPickY_C();
						}
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
					BeginMove(mousex_c, mousey_c);
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
							pgp->SetCheckMouseDown(true);
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
//			DeSelectAll();
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
				CheckMarqueeSelect_Pt(pObj, !baltdown);
			}
			else
			{
				CheckMarqueeSelect_Line(pObj, !baltdown);
			}
		}
		endx_c = pguic->GetCursorX_C();
		endy_c = pguic->GetCursorY_C();

	}

	if (pmain->hge->Input_GetDIKey(DIK_DELETE, DIKEY_UP))
	{
		if (!pcommand->GetCurrentCommand())
		{
			if (!selectednodes.empty())
			{
				pmain->OnCommandWithParam(
					COMM_DELETEITEM,
					NULL);
				for (list<GObject *>::iterator it=selectednodes.begin(); it!=selectednodes.end(); ++it)
				{
					pmain->OnCommandSingleParamI((*it)->getID());
				}
				pmain->OnCommandSingleParamI(-1);
			}
		}
	}

	lastmx_c = mousex_c;
	lastmy_c = mousey_c;
}

void MarqueeSelect::Render()
{
	CheckValid();
	for (list<GObject *>::iterator it=selectednodes.begin(); it!=selectednodes.end(); ++it)
	{
		GObject * pObj = *it;
		pObj->CallRender(LINECOLOR_ACTIVE);
		if (pObj->isHandlePoint())
		{
			GHandlePoint * pHandle = (GHandlePoint *)pObj;
			GHandlePoint * pOtherHandle = pHandle->getBindWith();
			if (pOtherHandle)
			{
				pOtherHandle->CallRender(LINECOLOR_ACTIVE);
			}
		}
	}
	if (marqueestate == MARQSTATE_BEGAN)
	{
		DWORD col = ColorManager::getInstance().GetMarqueeLineColor();
		RenderHelper::getInstance().SetLineStyle(RHLINESTYLE_DOTTEDLINE);
		RenderHelper::getInstance().RenderRect(beginx_c, beginy_c, endx_c-beginx_c, endy_c-beginy_c, col);
		RenderHelper::getInstance().SetLineStyle();
	}
}

void MarqueeSelect::CheckMarqueeSelect_Line( GObject * pObj, bool bAdd/*=true */ )
{
	if (!pObj)
	{
		return;
	}
	if (pObj->isRepresentableLine())
	{
		if (((GLine *)pObj)->CheckIntersectWithRect(beginx_c, beginy_c, endx_c, endy_c))
		{
			if (bAdd)
			{
				AddSelect(pObj, MARQNOMOVE_LINE);
			}
			else
			{
				DeSelect(pObj);
			}
//			selectednodes.push_back(pObj);
		}
	}
	for (list<GObject *>::iterator it=pObj->getChildren()->begin(); it!=pObj->getChildren()->end(); ++it)
	{
		CheckMarqueeSelect_Line(*it, bAdd);
	}
}

void MarqueeSelect::CheckMarqueeSelect_Pt( GObject * pObj, bool bAdd/*=true */ )
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
				if (bAdd)
				{
					AddSelect(pObj, MARQNOMOVE_POINT);
				}
				else
				{
					DeSelect(pObj);
				}
//				selectednodes.push_back(pObj);
			}
		}
	}
	for (list<GObject *>::iterator it=pObj->getChildren()->begin(); it!=pObj->getChildren()->end(); ++it)
	{
		CheckMarqueeSelect_Pt(*it, bAdd);
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

	int nMoveActionID = GObjectManager::getInstance().GetNextMoveActionID(GMMATYPE_MOVE);
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

bool MarqueeSelect::CheckObjInSelection( GObject * pTestObj, bool bFindAncestor/*=false*/, bool bFindUpperClass/*=false*/, bool bFindMerge/*=false*/, bool bFindCling/*=false*/, list<GObject *> * plstObj/*=NULL*/ )
{
	if (!pTestObj)
	{
		return false;
	}
	if (pTestObj->isSlaveToLine())
	{
		pTestObj = pTestObj->getLine();
	}
	else if (pTestObj->isSlaveToPiece())
	{
		pTestObj = pTestObj->getPiece();
	}

	if (!plstObj)
	{
		plstObj = &selectednodes;
	}
	
	GPoint * pItPoint = NULL;
	GLine * pItLine = NULL;
	GPiece * pItPiece = NULL;
	GObject * pItObj = NULL;

	for (list<GObject *>::iterator it=plstObj->begin(); it!=plstObj->end(); ++it)
	{
		pItObj = *it;
		if (pItObj == pTestObj)
		{
			return true;
		}

		if (pItObj->isPoint()) { pItPoint = (GPoint *)pItObj; }
		else if (pItObj->isLine()) { pItLine = (GLine *)pItObj; }
		else if (pItObj->isPiece()) { pItPiece = (GPiece *)pItObj; }

		if (bFindAncestor)
		{
			if (pItObj->isAncestorOf(pTestObj))
			{
				return true;
			}
			if (bFindMerge)
			{
				if (pTestObj->isPoint())
				{
					GPoint * pTestPoint = (GPoint *)pTestObj;
					list<GPoint *> * lstMerge = pTestPoint->getMergeWith();
					if (!lstMerge->empty())
					{
						for (list<GPoint *>::iterator it=lstMerge->begin(); it!=lstMerge->end(); ++it)
						{
							if (pItObj->isAncestorOf(*it))
							{
								return true;
							}
						}
					}
				}
			}
		}
		if (bFindUpperClass)
		{
			if (pItPoint)
			{
				if (pItPoint->getLine() == pTestObj)
				{
					return true;
				}
			}
			if (pItLine)
			{
				if (pItLine->getPiece() == pTestObj)
				{
					return true;
				}
			}
		}
		if (bFindMerge)
		{
			if (pItPoint)
			{
				if (pTestObj->isPoint())
				{
					GPoint * pPoint = (GPoint *)pTestObj;
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
								if ((*jt)->isAncestorOf(pTestObj))
								{
									return true;
								}
							}
							if (bFindUpperClass)
							{
								if ((*jt)->getLine() == pTestObj)
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
			GLine * pItAssumeLine = pItLine;
			if (pItPoint)
			{
				if (pItPoint->isMidPoint() || pItPoint->isAnchorPoint())
				{
					pItAssumeLine = pItPoint->getLine();
				}
			}
			if (pItAssumeLine)
			{
				if (pTestObj->isPoint())
				{
					if (pItAssumeLine->isClingBy((GPoint *)pTestObj))
					{
						return true;
					}
				}
			}
			/*
			if (pItPoint)
			{
				if (pTestObj->isLine())
				{
					GLine * pLine = (GLine *)pTestObj;

					if (pItPoint->isClingTo(pLine))
					{
						return true;
					}

				}
			}
			*/
		}
	}
	return false;
}

void MarqueeSelect::DoMovePoint( GPoint * pPoint, float movedx_c, float movedy_c, int nMoveActionID )
{
	if (pPoint->isSlaveToLine())
	{
		DoMoveLine(pPoint->getLine(), movedx_c, movedy_c, nMoveActionID);
	}
	else
	{
		if (nomoveflag < MARQNOMOVE_POINT)
		{
			pPoint->CallMoveByOffset(pPoint, movedx_c, movedy_c, true, nMoveActionID);
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
			pLine->CallMoveByOffset(pLine, movedx_c, movedy_c, true, nMoveActionID);
		}
	}
}

void MarqueeSelect::DoMovePiece( GPiece * pPiece, float movedx_c, float movedy_c, int nMoveActionID )
{
	//ToDo!!
	if (nomoveflag < MARQNOMOVE_PIECE)
	{
		pPiece->CallMoveByOffset(pPiece, movedx_c, movedy_c, true, nMoveActionID);
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

bool MarqueeSelect::staticMarqPickFilterCallback(GObject * pObj)
{
	return getInstance().MarqPickFilterCallback(pObj); 
}

bool MarqueeSelect::MarqPickFilterCallback( GObject * pObj )
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

void MarqueeSelect::OnDeleteNode( GObject * pDeletedObj )
{
	for (list<GObject *>::iterator it=selectednodes.begin(); it!=selectednodes.end();)
	{
		if ((*it) == pDeletedObj)
		{
			it = selectednodes.erase(it);
		}
		else
		{
			++it;
		}
	}
}

bool MarqueeSelect::staticMIDCBLength(MarkingUI * pmui, bool bAccept)
{
	return MarqueeSelect::getInstance().MIDCBLength(pmui, bAccept);
}
bool MarqueeSelect::staticMIDCBAngle(MarkingUI * pmui, bool bAccept)
{
	return MarqueeSelect::getInstance().MIDCBAngle(pmui, bAccept);
}
bool MarqueeSelect::staticMIDCBOffset(MarkingUI * pmui, bool bAccept)
{
	return MarqueeSelect::getInstance().MIDCBOffset(pmui, bAccept);
}

void MarqueeSelect::BeginMove( float nowx, float nowy )
{
	GObjectPicker::getInstance().UnlockLockLine();
	GObjectManager::getInstance().BeginTryMove();
	if (!pMarkingOffset)
	{
		ASSERT(pBeginObj);
		pMarkingOffset = new MarkingOffset(pBeginObj, MARKFLAG_OFFSET);

		pMarkingOffset->getMarkingUI(MARKFLAG_XOFFSET)->SetEditable(true);
		pMarkingOffset->getMarkingUI(MARKFLAG_YOFFSET)->SetEditable(true);
		pMarkingOffset->getMarkingUI(MARKFLAG_XOFFSET)->SetCallback(staticMIDCBOffset);
		pMarkingOffset->getMarkingUI(MARKFLAG_YOFFSET)->SetCallback(staticMIDCBOffset);
		pMarkingOffset->SetMoveOrigin(beginx_c, beginy_c);

		MarkingManager::getInstance().EnableMarking(pMarkingOffset);
	}

	if (!pMarkingLine)
	{
		if (selectednodes.size() == 1)
		{
			ASSERT(pBeginObj);
			bool bAnchor=false;
			bool bStraightLine=true;
			bool bHandle=false;
			int nFlag = 0;
			GLine * pLine = NULL;
			if (pBeginObj->isAnchorPoint())
			{
				bAnchor = true;
				pLine = pBeginObj->getLine();
				ASSERT(pLine);
				nFlag |= MARKFLAG_LENGTH;
				if (!pLine->isStraightLine())
				{
					bStraightLine = false;
				}
				else
				{
					nFlag |= MARKFLAG_ANGLE;
				}
			}
			else if (pBeginObj->isHandlePoint())
			{
				bHandle = true;
				pLine = pBeginObj->getLine();
				nFlag |= MARKFLAG_LENGTH;
			}

			if (pLine)
			{
				pMarkingLine = new MarkingLine(pLine, nFlag);

				if (bAnchor)
				{
					if (bStraightLine)
					{
						pMarkingLine->getMarkingUI(MARKFLAG_ANGLE)->SetEditable(true);
						pMarkingLine->getMarkingUI(MARKFLAG_ANGLE)->SetCallback(staticMIDCBAngle);
					}
				}
				if (bAnchor || bHandle)
				{
					pMarkingLine->getMarkingUI(MARKFLAG_LENGTH)->SetEditable(true);
					pMarkingLine->getMarkingUI(MARKFLAG_LENGTH)->SetCallback(staticMIDCBLength);
				}

				MarkingManager::getInstance().EnableMarking(pMarkingLine);
			}
		}

	}

	pMarkingOffset->SetNowPos(nowx, nowy);
}

void MarqueeSelect::EndMove()
{
	GObjectManager::getInstance().EndTryMove();
	GObjectPicker * pgp = &GObjectPicker::getInstance();
	if (pMarkingOffset)
	{
		MarkingManager::getInstance().DisableMarking(pMarkingOffset);
		pMarkingOffset = NULL;
		pgp->UnlockXAxis();
		pgp->UnlockYAxis();
	}
	if (pMarkingLine)
	{
		MarkingManager::getInstance().DisableMarking(pMarkingLine);
		pMarkingLine = NULL;
		pgp->UnlockLength();
		pgp->UnlockAngles();
	}
}

bool MarqueeSelect::MIDCBLength( MarkingUI * pmui, bool bAccept )
{
	GObjectPicker * pgp = &GObjectPicker::getInstance();
	if (pmui->IsValueLocked())
	{
		bool bOk;
		float fLockedLength = pmui->getFloat(&bOk);
		if (bOk)
		{
			GLine * pLine = pBeginObj->getLine();
			PointF2D ptLineBegin = pLine->GetBeginPoint()->GetPointF2D();
			pgp->SetLockOrigin(ptLineBegin.x, ptLineBegin.y);
			if (pBeginObj->isHandlePoint())
			{
				GHandlePoint * pBeginHandle = pLine->GetBeginPoint()->GetHandle();
				GHandlePoint * pEndHandle = pLine->GetEndPoint()->GetHandle();

				GHandlePoint * pAnotherHandle = pBeginObj==pBeginHandle?pEndHandle:pBeginHandle;
				GHandlePoint * pThisHandle = (GHandlePoint *)pBeginObj;

				bool bRet = MathHelper::getInstance().FindNearestHandlePointForGivenBezierLength(
					fLockedLength, pAnotherHandle->GetAnchor()->GetPointF2D(), pAnotherHandle->GetPointF2D(),
					pThisHandle->GetAnchor()->GetPointF2D(),
					0, 0, NULL, NULL, true);
				if (!bRet)
				{
					pgp->UnlockLength();
					return false;
				}
				else
				{
					pgp->SetLockLength(fLockedLength, pThisHandle);
				}
			}
			else
			{
				pgp->SetLockLength(fLockedLength);
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		pgp->UnlockLength();
	}
	return true;
}

bool MarqueeSelect::MIDCBAngle( MarkingUI * pmui, bool bAccept )
{
	GObjectPicker * pgp = &GObjectPicker::getInstance();
	if (pmui->IsValueLocked())
	{
		bool bOk;
		float fAngle = pmui->getFloat(&bOk);
		if (bOk)
		{
			int nLockedAngle = fAngle * ANGLEBASE_90/90;
			PointF2D ptLineBegin = pBeginObj->getLine()->GetBeginPoint()->GetPointF2D();
			pgp->SetLockOrigin(ptLineBegin.x, ptLineBegin.y);
			pgp->SetLockAngle(nLockedAngle);
		}
		else
		{
			return false;
		}
	}
	else
	{
		pgp->UnlockAngles();
	}
	return true;
}

bool MarqueeSelect::MIDCBOffset( MarkingUI * pmui, bool bAccept )
{
	bool bxoffset = true;
	GObjectPicker * pgp = &GObjectPicker::getInstance();
	if (pmui->getTypeFlag() == MARKFLAG_YOFFSET)
	{
		bxoffset = false;
	}

	if (pmui->IsValueLocked())
	{
		bool bOk;
		float fLockedVal = pmui->getFloat(&bOk);
		if (bOk)
		{
			if (bxoffset)
			{
				pgp->SetLockYAxis(fLockedVal+beginx_c);
			}
			else
			{
				pgp->SetLockXAxis(fLockedVal+beginy_c);
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (bxoffset)
		{
			pgp->UnlockYAxis();
		}
		else
		{
			pgp->UnlockXAxis();
		}
	}
	return true;
}

list<GObject*> * MarqueeSelect::OnGetSelectedNodes()
{
	return &selectednodes;
}