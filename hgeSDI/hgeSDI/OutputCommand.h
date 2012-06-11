#pragma once
#include "commandtemplate.h"

class OutputCommand :
	public CommandTemplate
{
public:
	static OutputCommand& getInstance()
	{
		static OutputCommand instance;
		// Guaranteed to be destroyed. Instantiated on first use.
		return instance;
	}

private:
	OutputCommand();
	~OutputCommand();
	// Don't forget to declare these two.
	OutputCommand(OutputCommand const&);
	// Don't Implement
	void operator=(OutputCommand const&);
	// Don't implement

	virtual void OnProcessCommand();
	virtual void OnDoneCommand();

	bool OutputNodeStructure(GObject * pObj);

public:
	
};

