#include "StdAfx.h"
#include "OutputCommand.h"

#include <sstream>

OutputCommand::OutputCommand(void)
{
}


OutputCommand::~OutputCommand(void)
{
}

void OutputCommand::OnProcessCommand()
{
	InstantProcessCommand();
}

void OutputCommand::OnDoneCommand()
{

	OutputNodeStructure(pgm->pBaseNode);

}

bool OutputCommand::OutputNodeStructure( GObject * pObj )
{
	if (!pObj || pObj->isAttributeNode())
	{
		return false;
	}
	stringstream ss;
	ss << pObj->getDisplayName() << " ID: " << pObj->getID() << " [" << (int)pObj << "], " << (int)pObj->getChildren()->size() << "; ";
	pmain->CallAppendCommandLogText(ss.str().c_str());
	for (list<GObject *>::iterator it=pObj->getChildren()->begin(); it!=pObj->getChildren()->end(); ++it)
	{
		OutputNodeStructure(*it);
	}
	return true;
}