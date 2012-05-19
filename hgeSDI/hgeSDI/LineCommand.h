#pragma once

class LineCommand
{
public:
	LineCommand();
	~LineCommand();

	static LineCommand * getInstance();

	int Line();
};