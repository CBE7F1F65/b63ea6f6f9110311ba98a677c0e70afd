#include "StdAfx.h"
#include "CommandTemplate.h"


CommandTemplate::CommandTemplate(void)
{
	pguic = &GUICoordinate::getInstance();
	pmain = &MainInterface::getInstance();
	pcommand = &Command::getInstance();
}


CommandTemplate::~CommandTemplate(void)
{
}

void CommandTemplate::ReleaseTarget()
{
	pcommand->SetRenderTarget(0);
}

void CommandTemplate::RenderToTarget()
{

}