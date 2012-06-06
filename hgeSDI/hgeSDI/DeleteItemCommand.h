#pragma once
#include "commandtemplate.h"

class DeleteItemCommand :
	public CommandTemplate
{
public:
	static DeleteItemCommand& getInstance()
	{
		static DeleteItemCommand instance;
		// Guaranteed to be destroyed. Instantiated on first use.
		return instance;
	}

private:
	DeleteItemCommand();
	~DeleteItemCommand();
	// Don't forget to declare these two.
	DeleteItemCommand(DeleteItemCommand const&);
	// Don't Implement
	void operator=(DeleteItemCommand const&);
	// Don't implement

public:
	
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
	virtual void OnProcessUnDoCommand(RevertableCommand * rc);

};

