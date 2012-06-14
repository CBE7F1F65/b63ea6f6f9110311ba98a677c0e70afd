#include "StdAfx.h"
#include "GPiece.h"
#include "StringManager.h"

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

bool GPiece::Clone( GObject * pNewParent )
{
	_GOBJ_CLONE_PRE(GPiece);
	_GOBJ_CLONE_POST();
}

void GPiece::MovePiece( float movedx_c, float movedy_c, bool trymode )
{
	//ToDo!!
}