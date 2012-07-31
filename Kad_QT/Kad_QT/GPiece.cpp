#include "StdAfx.h"
#include "GPiece.h"
#include "StringManager.h"

#include "GBaseNode.h"

#include "GObjectManager.h"

GPiece::GPiece(void)
{
}

GPiece::~GPiece(void)
{
}

const char * GPiece::getDisplayName()
{
	if (strDisplayName.length())
	{
		return strDisplayName.c_str();
	}
	return StringManager::getInstance().GetNNPieceName();
}

GObject * GPiece::CreateNewClone( GObject * pNewParent/*=NULL*/, GObject * pBeforeObj/*=NULL*/ )
{
	_GOBJ_CLONE_PRE(GPiece);
	_GOBJ_CLONE_POST();
}

void GPiece::MovePiece( float movedx_c, float movedy_c, bool trymode )
{
	//ToDo!!
}