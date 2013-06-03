#include "StdAfx.h"
#include "SaveCommand.h"


SaveCommand::SaveCommand(void)
{
}


SaveCommand::~SaveCommand(void)
{
}

void SaveCommand::OnProcessCommand()
{
	InstantProcessCommand();
}

void SaveCommand::OnDoneCommand()
{
	MainInterface::getInstance().SaveFile(comm==COMM_SAVEAS?true:false);
	/*
	list<GObject *> lstObj;
	
	for (int j=1; j<4011; j++)
	{
		pcommand->SetParamI(CSP_DUMP_I_INDEXES+j-1, j, CWP_INDEX);
	}
	pcommand->SetParamI(CSP_DUMP_I_INDEXES+4011, -1, CWP_INDEX);


	int index = pcommand->GetParamI(CSP_DUMP_I_INDEXES);
	list<GObject *> lobjs;
	int i=0;
	while (index >= 0)
	{
		GObject * pObj = pgm->FindObjectByID(index);
		float tx, ty, tt, tb;
		if (pObj && pObj->GetBoundingBox(&tx, &ty, &tt, &tb))
		{
			lobjs.push_back(pObj);
			if (pObj->isLine())
			{
				GLine * pLine = (GLine *)pObj;
				list<GPoint *> * pClingByList = pLine->getClingBy();
				if (pClingByList && !pClingByList->empty())
				{
					for (list<GPoint *>::iterator it=pClingByList->begin(); it!=pClingByList->end(); ++it)
					{
						GPoint * pClingByPoint = *it;
						if (pClingByPoint->isNotch())
						{
							lobjs.push_back(pClingByPoint);
						}
					}
				}
			}
		}
		i++;
		index = pcommand->GetParamI(CSP_DUMP_I_INDEXES+i);
	}


	if (pgm->Dump(lobjs, true));
	*/
}