#include "StdAfx.h"
#include "GShape.h"
#include "StringManager.h"

GShape::GShape(void)
{
}

GShape::~GShape(void)
{
}

const char * GShape::GetTypeName()
{
	return StringManager::getInstance().GetNNShapeName();
}