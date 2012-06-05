#pragma once
#include "commandtemplate.h"

class NewLayerCommand :
	public CommandTemplate
{
public:
	static NewLayerCommand& getInstance()
	{
		static NewLayerCommand instance;
		// Guaranteed to be destroyed. Instantiated on first use.
		return instance;
	}

private:
	NewLayerCommand();
	~NewLayerCommand();
	// Don't forget to declare these two.
	NewLayerCommand(NewLayerCommand const&);
	// Don't Implement
	void operator=(NewLayerCommand const&);
	// Don't implement

public:

	virtual void OnProcessCommand();
	virtual void OnDoneCommand();

}; 