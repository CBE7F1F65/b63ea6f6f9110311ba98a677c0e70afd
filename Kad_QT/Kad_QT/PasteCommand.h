#pragma once
#include "commandtemplate.h"

class PasteCommand : public CommandTemplate
{
public:
	static PasteCommand& getInstance() { static PasteCommand instance; return instance; }

private:
	PasteCommand();
	~PasteCommand();
	PasteCommand(PasteCommand const&);
	void operator=(PasteCommand const&);

public:
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();	
};
