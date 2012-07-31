#include "StdAfx.h"
#include "GLayer.h"

#include "GBaseNode.h"

#include "StringManager.h"
#include <sstream>
#include "ColorManager.h"

#include "GObjectManager.h"

GLayer::GLayer(void)
{
	layerID = 0;
}

GLayer::GLayer( int _layerID, const char * layername )
{
	setInitial(_layerID, layername);
}

void GLayer::setInitial( int _layerID, const char * layername )
{
	layerID = _layerID;
	setLineColor(ColorManager::getInstance().GetLayerLineColorSetByIndex(layerID));
	setDisplayName(layername);
}

GLayer::~GLayer(void)
{
}

const char * GLayer::getDisplayName()
{
	if (!strDisplayName.length())
	{
		/*
		strDisplayName = StringManager::getInstance().GetNNLayerName();
		strDisplayName += " ";
		stringstream ss;
		ss << layerID;
		strDisplayName += ss.str();
		*/
		ASSERT(false);
	}
	return strDisplayName.c_str();
}

GObject * GLayer::CreateNewClone( GObject * pNewParent/*=NULL*/, GObject * pBeforeObj/*=NULL*/ )
{
	_GOBJ_CLONE_PRE(GLayer);
	_GOBJ_CLONE_POST();
}

bool GLayer::CloneData( GObject * pClone, GObject * pNewParent, bool bNoRelationship/*=true*/ )
{
	if (super::CloneData(pClone, pNewParent))
	{
		GLayer * pLayer = (GLayer *)pClone;
		pLayer->setInitial(layerID, strDisplayName.c_str());
		return true;
	}
	return false;
}