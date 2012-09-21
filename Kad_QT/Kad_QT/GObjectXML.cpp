#include "stdafx.h"

#include "GLayer.h"
#include "GLine.h"
#include "GBaseNode.h"
#include "GPiece.h"
#include "GShape.h"

#include "RenderHelper.h"

#include "GObjectManager.h"

/************************************************************************/
/* GObject                                                              */
/************************************************************************/

bool GObject::CallReadXML( QXmlStreamReader &qsr )
{
	return true;
}

bool GObject::CallWriteXML( QXmlStreamWriter &qsw )
{
	StringManager *psm = &StringManager::getInstance();

	qsw.writeStartElement(getTypeName());
	qsw.writeTextElement(psm->GetXMLNodeIDName(), QString::number(nID));
	if (strDisplayName.length())
	{
		qsw.writeTextElement(psm->GetXMLNodeDisplayNameName(), strDisplayName.c_str());
	}

	if (!pParent || !(pParent->lsLineColorSet == lsLineColorSet))
	{
		qsw.writeStartElement(psm->GetXMLNodeLineColorsName());
		for (int i=0; i<_LINECOLOR_ENDINDEX; i++)
		{
			qsw.writeTextElement(QString::number(i), QString::number(lsLineColorSet.dwColors[i]));
		}
		qsw.writeEndElement();
	}

	if (nDisplayState != GOBJ_DISPSTATE_NORMAL)
	{
		qsw.writeTextElement(psm->GetXMLNodeDisplayStateName(), QString::number(nDisplayState));
	}
	if (bDisplayFolded == true)
	{
		qsw.writeTextElement(psm->GetXMLNodeFoldedName(), QString::number(bDisplayFolded));
	}
	
	if (!WriteXML(qsw))
	{
		return false;
	}

	if (!lstChildren.empty())
	{
		qsw.writeStartElement(psm->GetXMLNodeChildrenName());
//		qsw.writeTextElement(psm->GetXMLNodeChildrenCountName(), QString::number(lstChildren.size()));
		for (list<GObject *>::iterator it=lstChildren.begin(); it!=lstChildren.end(); ++it)
		{
			(*it)->CallWriteXML(qsw);
		}
		qsw.writeEndElement();
	}

	qsw.writeEndElement();

	return true;
}

bool GObject::ReadXML( QXmlStreamReader &qsr )
{
	/*
	if (!super::ReadXML(qsr))
	{
		return false;
	}
	*/

	return true;
}

bool GObject::WriteXML( QXmlStreamWriter &qsw )
{
	/*
	if (!super::WriteXML(qsw))
	{
		return false;
	}
	*/

	return true;
}

/************************************************************************/
/* GPoint                                                               */
/************************************************************************/
bool GPoint::ReadXML(QXmlStreamReader &qsr)
{
	if (!super::ReadXML(qsr))
	{
		return false;
	}

	return true;
}

bool GPoint::WriteXML(QXmlStreamWriter &qsw)
{
	if (!super::WriteXML(qsw))
	{
		return false;
	}

	StringManager *psm = &StringManager::getInstance();

	qsw.writeStartElement(QString(GPoint::getTypeName())+"_"+psm->GetXMLNodeAttrName());

	qsw.writeTextElement(psm->GetXMLNodeXName(), QString::number(x));
	qsw.writeTextElement(psm->GetXMLNodeYName(), QString::number(y));

	GLine * pClingTo = clInfo.GetClingTo();
	if (pClingTo)
	{
		qsw.writeStartElement(psm->GetXMLNodeClingInfoName());

		qsw.writeTextElement(psm->GetXMLNodeClingToName(), QString::number(pClingTo->getID()));
		int nClingType = clInfo.GetClingType();
		if (nClingType != GCLING_PROPORTION)
		{
			qsw.writeTextElement(psm->GetXMLNodeClingTypeName(), QString::number(nClingType));
		}
		qsw.writeTextElement(psm->GetXMLNodeClingValName(), QString::number(clInfo.GetClingVal()));

		qsw.writeEndElement();
	}
	if (!mergeWithList.empty())
	{
		qsw.writeStartElement(psm->GetXMLNodeMergeWithName());

		int i=0;
		for (list<GPoint *>::iterator it=mergeWithList.begin(); it!=mergeWithList.end(); ++it)
		{
			qsw.writeTextElement(QString::number(i), QString::number((*it)->getID()));
			i++;
		}

		qsw.writeEndElement();
	}

	qsw.writeEndElement();

	return true;
}

/************************************************************************/
/* GAnchorPoint                                                         */
/************************************************************************/

bool GAnchorPoint::ReadXML(QXmlStreamReader &qsr)
{
	if (!super::ReadXML(qsr))
	{
		return false;
	}

	return true;
}

bool GAnchorPoint::WriteXML(QXmlStreamWriter &qsw)
{
	if (!super::WriteXML(qsw))
	{
		return false;
	}

	StringManager *psm = &StringManager::getInstance();
	if (pHandle)
	{
		qsw.writeTextElement(psm->GetXMLNodeHandleName(), QString::number(pHandle->getID()));
	}
	return true;
}

/************************************************************************/
/* GHandlePoint                                                         */
/************************************************************************/

bool GHandlePoint::ReadXML(QXmlStreamReader &qsr)
{
	if (!super::ReadXML(qsr))
	{
		return false;
	}

	return true;
}

bool GHandlePoint::WriteXML(QXmlStreamWriter &qsw)
{
	if (!super::WriteXML(qsw))
	{
		return false;
	}

	StringManager *psm = &StringManager::getInstance();
	if (pBindWith)
	{
		qsw.writeTextElement(psm->GetXMLNodeHandleName(), QString::number(pBindWith->getID()));
	}
	return true;
}

/************************************************************************/
/* GLine                                                                */
/************************************************************************/

bool GLine::ReadXML(QXmlStreamReader &qsr)
{
	if (!super::ReadXML(qsr))
	{
		return false;
	}

	return true;
}

bool GLine::WriteXML(QXmlStreamWriter &qsw)
{
	if (!super::WriteXML(qsw))
	{
		return false;
	}
	StringManager *psm = &StringManager::getInstance();

	qsw.writeStartElement(QString(GLine::getTypeName())+"_"+psm->GetXMLNodeAttrName());

	if (plbegin)
	{
		qsw.writeTextElement(psm->GetXMLNodeBeginName(), QString::number(plbegin->getID()));
	}
	if (plend)
	{
		qsw.writeTextElement(psm->GetXMLNodeEndName(), QString::number(plend->getID()));
	}
	if (pmid)
	{
		qsw.writeTextElement(psm->GetXMLNodeMidName(), QString::number(pmid->getID()));
	}
	if (nLineRenderStyle != RHLINESTYLE_LINE)
	{
		qsw.writeTextElement(psm->GetXMLNodeStyleName(), QString::number(nLineRenderStyle));
	}

	qsw.writeEndElement();

	return true;
}

/************************************************************************/
/* GLayer                                                               */
/************************************************************************/

bool GLayer::ReadXML(QXmlStreamReader &qsr)
{
	if (!super::ReadXML(qsr))
	{
		return false;
	}

	return true;
}

bool GLayer::WriteXML(QXmlStreamWriter &qsw)
{
	if (!super::WriteXML(qsw))
	{
		return false;
	}

	StringManager *psm = &StringManager::getInstance();
	qsw.writeStartElement(QString(GLayer::getTypeName())+"_"+psm->GetXMLNodeAttrName());

	qsw.writeTextElement(psm->GetXMLNodeLayerIDName(), QString::number(layerID));

	qsw.writeEndElement();

	return true;
}

/************************************************************************/
/* GetTypeName                                                          */
/************************************************************************/

#define _GXMLGETTYPENAME(TYPENAME)	\
	const char * TYPENAME::getTypeName()	\
{	\
	return #TYPENAME;	\
}

_GXMLGETTYPENAME(GPoint);
_GXMLGETTYPENAME(GMidPoint);
_GXMLGETTYPENAME(GAnchorPoint);
_GXMLGETTYPENAME(GHandlePoint);
_GXMLGETTYPENAME(GLine);
_GXMLGETTYPENAME(GBezierLine);
_GXMLGETTYPENAME(GLayer);
_GXMLGETTYPENAME(GBaseNode);
_GXMLGETTYPENAME(GMainBaseNode);
_GXMLGETTYPENAME(GBaseNodeCopyStack);
_GXMLGETTYPENAME(GPiece);
_GXMLGETTYPENAME(GShape);

#undef _GXMLGETTYPENAME