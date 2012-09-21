#include "StdAfx.h"
#include "SaveCommand.h"


SaveCommand::SaveCommand(void)
{
}


SaveCommand::~SaveCommand(void)
{
}

void SaveCommand::OnProcessCommand()
{
	InstantProcessCommand();
}

void SaveCommand::OnDoneCommand()
{
	MainInterface::getInstance().SaveFile(comm==COMM_SAVEAS?true:false);
}