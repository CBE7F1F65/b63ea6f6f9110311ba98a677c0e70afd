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