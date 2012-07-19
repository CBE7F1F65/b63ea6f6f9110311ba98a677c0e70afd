#include "StdAfx.h"
#include "GLayer.h"

#include "StringManager.h"
#include <sstream>
#include "ColorManager.h"


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

bool GLayer::Clone( GObject * pNewParent )
{
	_GOBJ_CLONE_PRE(GLayer);
	_node->setInitial(layerID, strDisplayName.c_str());
	_GOBJ_CLONE_POST();
}