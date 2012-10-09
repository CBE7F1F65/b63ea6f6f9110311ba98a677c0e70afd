#include "stdafx.h"
#include "CommandTemplate.h"

#include "LineCommand.h"
#include "BezierCommand.h"
#include "InitialCommand.h"
#include "NewLayerCommand.h"
#include "SetWorkingLayerCommand.h"
#include "DeleteItemCommand.h"
#include "ReparentCommand.h"
#include "OutputCommand.h"
#include "SetNodeNameCommand.h"
#include "DuplicateCommand.h"
#include "MoveNodeCommand.h"
#include "MoveNodeBatchCommand.h"
#include "ClingCommand.h"
#include "MergeCommand.h"
#include "DemergeCommand.h"
#include "TestCommand.h"
#include "BindHandleCommand.h"
#include "LineTransformCommand.h"
#include "ClipCommand.h"
#include "RotateNodeBatchCommand.h"
#include "ScaleNodeBatchCommand.h"
#include "ExtendCommand.h"
#include "FlipNodeBatchCommand.h"
#include "RestoreToSnapshotCommand.h"
#include "JoinLineCommand.h"
#include "CircleCommand.h"
#include "SaveCommand.h"
#include "OpenCommand.h"
#include "AddSACommand.h"
#include "DumpCommand.h"

CommandTemplate * CommandTemplate::GetTemplateByCommand( int comm )
{
	switch (comm)
	{
	case COMM_INITIAL:
	case COMM_TERMINAL:
		return &InitialCommand::getInstance();

	case COMM_REPARENT:
		return &ReparentCommand::getInstance();
	case COMM_CLING:
		return &ClingCommand::getInstance();
	case COMM_BINDHANDLE:
		return &BindHandleCommand::getInstance();
	case COMM_MERGE:
		return &MergeCommand::getInstance();
	case COMM_DEMERGE:
		return &DemergeCommand::getInstance();
	case COMM_DELETEITEM:
		return &DeleteItemCommand::getInstance();
	case COMM_SETNODENAME:
		return &SetNodeNameCommand::getInstance();
	case COMM_DUPLICATE:
		return &DuplicateCommand::getInstance();
	case COMM_MOVENODE:
	case COMM_MOVENODEBYOFFSET:
		return &MoveNodeCommand::getInstance();
	case COMM_MOVENODEBYOFFSET_BATCH:
		return &MoveNodeBatchCommand::getInstance();
	case COMM_ROTATENODE_BATCH:
		return &RotateNodeBatchCommand::getInstance();
	case COMM_SCALENODE_BATCH:
		return &ScaleNodeBatchCommand::getInstance();
	case COMM_FLIPNODE_BATCH:
		return &FlipNodeBatchCommand::getInstance();
	case COMM_EXTEND:
		return &ExtendCommand::getInstance();

	case COMM_TOBEZIER:
	case COMM_TOSTRAIGHTLINE:
	case COMM_SWAPBEGINEND:
		return &LineTransformCommand::getInstance();
	case COMM_CLIP:
		return &ClipCommand::getInstance();
	case COMM_JOIN:
		return &JoinLineCommand::getInstance();

	case COMM_NEWLAYER:
	case COMM_NEWSUBLAYER:
		return &NewLayerCommand::getInstance();
	case COMM_SETWORKINGLAYER:
		return &SetWorkingLayerCommand::getInstance();

	case COMM_LINE:
		return &LineCommand::getInstance();
	case COMM_BEZIER:
		return &BezierCommand::getInstance();
	case COMM_CIRCLE:
		return &CircleCommand::getInstance();

	case COMM_ADDSA:
		return &AddSACommand::getInstance();

	case COMM_DUMP:
		return &DumpCommand::getInstance();

	case COMM_RESTORETOSNAPSHOT:
		return &RestoreToSnapshotCommand::getInstance();

	case COMM_SAVE:
	case COMM_SAVEAS:
		return &SaveCommand::getInstance();
	case COMM_OPEN:
		return &OpenCommand::getInstance();

	case COMM_OUTPUT:
		return &OutputCommand::getInstance();
#ifdef DEBUGTEST
	case COMM_TEST:
		return &TestCommand::getInstance();
#endif //DEBUGTEST

	case COMM_PAN:
	case COMM_ZOOMIN:
	case COMM_DOZOOM:
		break;
	default:
		ASSERT(false);
	}
	return NULL;
}