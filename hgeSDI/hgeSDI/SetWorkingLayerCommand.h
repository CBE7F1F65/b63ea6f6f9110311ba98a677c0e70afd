#pragma once
#include "commandtemplate.h"
#include "GLayer.h"

class SetWorkingLayerCommand :
	public CommandTemplate
{
public:
	static SetWorkingLayerCommand& getInstance() { static SetWorkingLayerCommand instance; return instance; }

private:
	SetWorkingLayerCommand();
	~SetWorkingLayerCommand();
	SetWorkingLayerCommand(SetWorkingLayerCommand const&);
	void operator=(SetWorkingLayerCommand const&);

public:
	
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
	virtual void OnProcessUnDoCommand(RevertableCommand * rc);

	GLayer * lastActiveLayer;
};

