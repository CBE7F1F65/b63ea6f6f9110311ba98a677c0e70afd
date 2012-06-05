#pragma once
#include "commandtemplate.h"
#include "GLayer.h"

class SetWorkingLayerCommand :
	public CommandTemplate
{
public:
	static SetWorkingLayerCommand& getInstance()
	{
		static SetWorkingLayerCommand instance;
		// Guaranteed to be destroyed. Instantiated on first use.
		return instance;
	}

private:
	SetWorkingLayerCommand();
	~SetWorkingLayerCommand();
	// Don't forget to declare these two.
	SetWorkingLayerCommand(SetWorkingLayerCommand const&);
	// Don't Implement
	void operator=(SetWorkingLayerCommand const&);
	// Don't implement

public:
	
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
	virtual void OnProcessUnDoCommand(RevertableCommand * rc);

	GLayer * lastActiveLayer;
};

