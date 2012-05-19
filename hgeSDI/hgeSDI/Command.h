#pragma once

enum{
	COMM_NULL = 0,

	COMM_LINE,

	COMM_FORCEDWORD = 0x7fffffff,
};

class Command
{
public:
	Command();
	~Command();

	static int DispatchCommand(DWORD comm);
	static DWORD currentcommand;
	static int step;
};