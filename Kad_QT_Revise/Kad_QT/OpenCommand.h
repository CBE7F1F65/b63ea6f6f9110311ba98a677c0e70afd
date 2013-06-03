#pragma once
#include "commandtemplate.h"

class OpenCommand :
	public CommandTemplate
{
public:
	static OpenCommand& getInstance() { static OpenCommand instance; return instance; }

private:
	OpenCommand();
	~OpenCommand();
	OpenCommand(OpenCommand const&);
	void operator=(OpenCommand const&);

public:
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();	
};