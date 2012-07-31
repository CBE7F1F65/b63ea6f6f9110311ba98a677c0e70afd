#pragma once
#include "commandtemplate.h"

class DeleteItemCommand :
	public CommandTemplate
{
public:
	static DeleteItemCommand& getInstance() { static DeleteItemCommand instance; return instance; }

private:
	DeleteItemCommand();
	~DeleteItemCommand();
	DeleteItemCommand(DeleteItemCommand const&);
	void operator=(DeleteItemCommand const&);

public:
	
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();

};

