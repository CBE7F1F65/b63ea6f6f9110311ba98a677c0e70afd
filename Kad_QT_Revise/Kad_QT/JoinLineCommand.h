#pragma once
#include "commandtemplate.h"

class JoinLineCommand : public CommandTemplate
{
public:
	static JoinLineCommand& getInstance() { static JoinLineCommand instance; return instance; }

private:
	JoinLineCommand();
	~JoinLineCommand();
	JoinLineCommand(JoinLineCommand const&);
	void operator=(JoinLineCommand const&);

public:
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
	virtual void OnClearTemp();
	virtual void RenderToTarget();

	static bool staticPickFilterFunc(GObject * pObj);
	bool PickFilterFunc(GObject * pObj);

private:
	GLine * pFirst;
	
};

