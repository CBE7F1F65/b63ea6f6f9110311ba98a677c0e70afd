#include "StdAfx.h"
#include "GLayer.h"

#include "StringManager.h"
#include <sstream>


GLayer::GLayer(void)
{
	layerID = 0;
}

GLayer::GLayer( int _layerID, DWORD linecolor, const char * layername )
{
	setInitial(_layerID, linecolor, layername);
}

void GLayer::setInitial( int _layerID, DWORD linecolor, const char * layername )
{
	layerID = _layerID;
	setLineColor(linecolor);
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
		ASSERT(true);
	}
	return strDisplayName.c_str();
}

bool GLayer::Clone( GObject * pNewParent )
{
	_GOBJ_CLONE_PRE(GLayer);
	_node->setInitial(layerID, dwLineColor, strDisplayName.c_str());
	_GOBJ_CLONE_POST();
}