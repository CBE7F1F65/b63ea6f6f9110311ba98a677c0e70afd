#pragma once

class LineCommand
{
public:
	static LineCommand& getInstance()
	{
		static LineCommand instance;
		// Guaranteed to be destroyed. Instantiated on first use.
		return instance;
	}

private:
	LineCommand();
	~LineCommand();
	// Don't forget to declare these two.
	LineCommand(LineCommand const&);
	// Don't Implement
	void operator=(LineCommand const&);
	// Don't implement

public:
	
	int Line();
	void DoneLineCommand();
	void LineCommandRenderToTarget();
	void ReleaseTarget();
};