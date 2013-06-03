#pragma once
#include "commandtemplate.h"

class ImageCommand : public CommandTemplate
{
public:
	static ImageCommand& getInstance() { static ImageCommand instance; return instance; }

private:
	ImageCommand();
	~ImageCommand();
	ImageCommand(ImageCommand const&);
	void operator=(ImageCommand const&);


public:
	virtual void OnProcessCommand();
	virtual void OnDoneCommand();
	
};