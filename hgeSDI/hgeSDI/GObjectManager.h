#pragma once

#include "GObject.h"
#include "MainDependency.h"

class GObjectManager
{
public:
	static GObjectManager& getInstance()
	{
		static GObjectManager instance;
		// Guaranteed to be destroyed. Instantiated on first use.
		return instance;
	}

private:
	GObjectManager();
	~GObjectManager();
	// Don't forget to declare these two.
	GObjectManager(GObjectManager const&);
	// Don't Implement
	void operator=(GObjectManager const&);
	// Don't implement

public:

	void Init();
	void Release();
	void Update();
	void Render();
	void Delete();

	void OnTreeChanged(GObject * changingbase);

	void AddNodeToDelete(GObject * node);

	void MoveToUnDoList(GObject * node);

	GObject basenode;
	list<GObject*> nodetodelete;

	GObject undobasenode;

	HTARGET tarobjs;
};