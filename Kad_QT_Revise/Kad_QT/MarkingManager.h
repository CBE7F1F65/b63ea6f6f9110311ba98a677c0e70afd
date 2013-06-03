#pragma once

#include "MarkingObject.h"

class MarkingManager
{
public:
	static MarkingManager& getInstance() { static MarkingManager instance; return instance; }

private:
	MarkingManager();
	~MarkingManager();
	MarkingManager(MarkingManager const&);
	void operator=(MarkingManager const&);

public:
	bool EnableMarking(MarkingObject * pMarking);
	bool DisableMarking(MarkingObject * pMarking=NULL);
	MarkingObject * FindMarking(GObject * pObj, int nMarkFlag=-1);

	void Update();
	void Render();
	void SetRedraw();
	void OnDeleteNode( GObject * pDeletedObj );
private:
	list<MarkingObject *> lstMarkings;
	bool bRedraw;
	int tarMarking;
};

