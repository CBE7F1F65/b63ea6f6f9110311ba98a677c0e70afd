#include "StdAfx.h"
#include "GPiece.h"
#include "StringManager.h"

GPiece::GPiece(void)
{
}

GPiece::~GPiece(void)
{
}

const char * GPiece::GetTypeName()
{
	return StringManager::getInstance().GetNNPieceName();
}