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

class NewSubLayerCommand : 
	public CommandTemplate
{
public:
	static NewSubLayerCommand& getInstance()
	{
		static NewSubLayerCommand instance;
		// Guaranteed to be destroyed. Instantiated on first use.
		return instance;
	}

private:
	NewSubLayerCommand();
	~NewSubLayerCommand();
	// Don't forget to declare these two.
	NewSubLayerCommand(NewSubLayerCommand const&);
	// Don't Implement
	void operator=(NewSubLayerCommand const&);
	// Don't implement

public:
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
	

}; 
