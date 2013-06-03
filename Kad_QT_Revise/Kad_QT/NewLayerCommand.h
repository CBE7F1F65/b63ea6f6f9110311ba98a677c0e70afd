#pragma once
#include "commandtemplate.h"

class NewLayerCommand :
	public CommandTemplate
{
public:
	static NewLayerCommand& getInstance() { static NewLayerCommand instance; return instance; }

private:
	NewLayerCommand();
	~NewLayerCommand();
	NewLayerCommand(NewLayerCommand const&);
	void operator=(NewLayerCommand const&);

public:

	virtual void OnProcessCommand();
	virtual void OnDoneCommand();

}; 