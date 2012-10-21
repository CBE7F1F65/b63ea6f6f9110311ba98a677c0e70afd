#include "stdafx.h"

#include "GLayer.h"
#include "GLine.h"
#include "GBaseNode.h"
#include "GPiece.h"
#include "GShape.h"

#include "RenderHelper.h"

#include "GObjectManager.h"
#include "GObjectXML.h"

#define GOBJXML_NODEPREFIX	"NODE_"
#define GOBJXML_ATTRPREFIX	"ATTR_"
#define GOBJXML_PROPPREFIX	"PROP_"
#define GOBJXML_PROPSPREFIX	"PRPS_"
#define GOBJXML_PREFIXLENGTH	5

/************************************************************************/
/* XMLBase                                                              */
/************************************************************************/

GObjectXMLBase::GObjectXMLBase()
{
	pbasenode = NULL;
}

GObjectXMLBase::~GObjectXMLBase()
{
	DestroyNodes();
}

bool GObjectXMLBase::ReadXML( QXmlStreamReader &qsr )
{
	DestroyNodes();
	int nowindent = 0;
	int nchilddepth = 0;

	pbasenode = new GObjectXMLNode("");
	GObjectXMLNode * pnowbase = pbasenode;

	list<GObjectXMLNode *>lstDepth;

	QString strStartName;
	QString strValueName;
	while(!qsr.atEnd())
	{
		qsr.readNext();

		if (qsr.hasError())
		{
			return false;
		}
		if (qsr.isStartElement())
		{
			strStartName = qsr.name().toString();
			if (strStartName.startsWith(GOBJXML_PROPPREFIX))
			{
				strValueName = qsr.readElementText();
				pnowbase->AddProperty(strStartName.right(strStartName.length()-GOBJXML_PREFIXLENGTH), strValueName);
			}
			else if (strStartName.startsWith(GOBJXML_NODEPREFIX))
			{
				pnowbase->AddChild(strStartName.right(strStartName.length()-GOBJXML_PREFIXLENGTH));
				lstDepth.push_back(pnowbase);
				pnowbase = &pnowbase->lstChildren.back();
			}
			else
			{
				ASSERT(pnowbase);
				pnowbase->PushPrefix(strStartName.right(strStartName.length()-GOBJXML_PREFIXLENGTH));
			}
			nowindent++;
		}
		if (qsr.isEndElement())
		{
			nowindent--;
			if (nowindent >= 0)
			{
				strStartName = qsr.name().toString();
				if (strStartName.startsWith(GOBJXML_PROPPREFIX))
				{
				}
				else if (strStartName.startsWith(GOBJXML_NODEPREFIX))
				{
					pnowbase = lstDepth.back();
					lstDepth.pop_back();
				}
				else
				{
					pnowbase->PopPrefix();
				}
			}
		}
	}


	return true;
}

void GObjectXMLBase::DestroyNodes()
{
	if (pbasenode)
	{
		if (!pbasenode->DestroyNodes())
		{
			ASSERT(false);
		}
		delete pbasenode;
		pbasenode = NULL;
	}
}

/************************************************************************/
/* XMLNode                                                              */
/************************************************************************/

bool GObjectXMLNode::AddChild( QString name )
{
	GObjectXMLNode _node(name);
	lstChildren.push_back(_node);
	return true;
}

bool GObjectXMLNode::AddProperty( QString name, QString value )
{
	GObjectXMLProperty _prop(name, value, &lstPrefix);
	lstProp.push_back(_prop);
	return true;
}

bool GObjectXMLNode::PushPrefix( QString name )
{
	lstPrefix.push_back(name);
	return true;
}

bool GObjectXMLNode::PopPrefix()
{
	if (lstPrefix.empty())
	{
		ASSERT(false);
		return false;
	}
	else
	{
		lstPrefix.pop_back();
	}
	return true;
}

bool GObjectXMLNode::DestroyNodes()
{
	lstChildren.clear();
	ASSERT(lstPrefix.empty());
	lstPrefix.clear();
	lstProp.clear();
	return true;
}

QString GObjectXMLNode::GetValue( QString propname, QString strPrefix/*=""*/ )
{
	if (!lstProp.empty())
	{
		QString strPropName = strPrefix+propname;
		for (list<GObjectXMLProperty>::iterator it=lstProp.begin(); it!=lstProp.end(); ++it)
		{
			if (strPropName == it->getName())
			{
				return it->getValue();
			}
		}
	}
	/*
	if (strPrefix.length())
	{
		for (list<GObjectXMLProperty>::iterator it=lstProp.begin(); it!=lstProp.end(); ++it)
		{
			if (propname == it->getName())
			{
				return it->getValue();
			}
		}
	}
	*/
	return "";
}

GObjectXMLNode * GObjectXMLNode::GetSubNodeByID( int nID )
{
	for (list<GObjectXMLNode>::iterator it=lstChildren.begin(); it!=lstChildren.end(); ++it)
	{
		if ((it->GetValue(StringManager::getInstance().GetXMLNodeIDName())).toInt() == nID)
		{
			return &(*it);
		}
	}
	return NULL;
}

/************************************************************************/
/* XMLProp                                                              */
/************************************************************************/
GObjectXMLProperty::GObjectXMLProperty( QString name, QString value, list<QString> *plstPrefix/*=NULL*/ )
{
	this->name = "";
	if (plstPrefix)
	{
		if (!plstPrefix->empty())
		{
			for (list<QString>::iterator it=plstPrefix->begin(); it!=plstPrefix->end(); ++it)
			{
				this->name += *it;
			}
		}
	}
	this->name += name;
	this->value = value;
}

/************************************************************************/
/* GMainBaseNode                                                        */
/************************************************************************/
bool GMainBaseNode::CallReadXML( QXmlStreamReader &qsr )
{
	GObjectXMLBase::getInstance().ReadXML(qsr);

	GObjectXMLNode * pbase = GObjectXMLBase::getInstance().GetBase();

	ASSERT(pbase->lstChildren.size() == 1);
	pbase = &pbase->lstChildren.back();
	
	GObjectManager::getInstance().SetLockTreeChange();
	this->RemoveAllChildren(true);
	this->BuildXMLChildren(pbase);

	ReadXML(pbase);

	/*
	if (qsr.atEnd())
	{
		return true;
	}
	if (qsr.hasError())
	{
		return false;
	}

	StringManager *psm = &StringManager::getInstance();
	if (qsr.readNextStartElement())
	{
		QString strTypeName = qsr.name().toString();
		if (strTypeName != getTypeName())
		{
			return false;
		}
		
		
		qsr.readNextStartElement();
		bool bOk = false;

		// ID
		if (qsr.name() == psm->GetXMLNodeIDName())
		{
			int attr_id = qsr.readElementText().toInt(&bOk);
			if (!bOk || nID != attr_id)
			{
				return false;
			}
			qsr.readNextStartElement();
		}

		// DisplayName
		if (qsr.name() == psm->GetXMLNodeDisplayNameName())
		{
			QString attr_dname = qsr.readElementText();
			this->setDisplayName(attr_dname.toUtf8());
			qsr.readNextStartElement();
		}

		// LineColor
		if (qsr.name() == psm->GetXMLNodeLineColorsName())
		{
			LineColorSet attr_ls;
			for (int i=0; i<_LINECOLOR_ENDINDEX; i++)
			{
				qsr.readNextStartElement();
				int nowindex = qsr.name().toString().toInt(&bOk);
				if (!bOk || nowindex != i)
				{
					return false;
				}
				DWORD attr_col = qsr.readElementText().toUInt(&bOk);
				if (!bOk)
				{
					return false;
				}
				attr_ls.SetColor(i, attr_col);
			}

			this->setLineColor(attr_ls);
			qsr.readNextStartElement();
		}

		// DisplayState
		if (qsr.name() == psm->GetXMLNodeDisplayStateName())
		{
			int attr_dstate = qsr.readElementText().toInt(&bOk);
			if (bOk)
			{
				this->setDisplayLock(attr_dstate&GOBJ_DISPSTATE_LOCKED);
				this->setDisplayVisible(!(attr_dstate&GOBJ_DISPSTATE_INVISIBLE));
			}
			qsr.readNextStartElement();
		}
		// Folded
		if (qsr.name() == psm->GetXMLNodeFoldedName())
		{
			bool attr_bfolded = qsr	.readElementText().toInt(&bOk);
			if (bOk)
			{
				this->setDisplayFold(attr_bfolded);
			}
			qsr.readNextStartElement();
		}

		// ATTR
		ReadXML(qsr);

		// Children
	}
	*/
	return true;
}

/************************************************************************/
/* GObject                                                              */
/************************************************************************/

bool GObject::BuildXMLChildren( GObjectXMLNode * pnode )
{
	GObjectManager * pgm = &GObjectManager::getInstance();
	if (!pnode->lstChildren.empty())
	{
		StringManager * psm = &StringManager::getInstance();

		for (list<GObjectXMLNode>::reverse_iterator it=pnode->lstChildren.rbegin(); it!=pnode->lstChildren.rend(); ++it)
		{
			if (it->name == GLayer::getStaticTypeName())
			{
				GLayer * pLayer = pgm->NewSubLayer(this, (it->GetValue(psm->GetXMLNodeDisplayNameName())).toUtf8(), (it->GetValue(psm->GetXMLNodeLayerIDName(), GLayer::getStaticTypeName())).toInt());
				pLayer->BuildXMLChildren(&(*it));
			}
			else if (it->name == GBezierLine::getStaticTypeName())
			{
				GBezierLine * pBezier = new GBezierLine(this, PointF2D(), PointF2D());
				pBezier->BuildXMLChildren(&(*it));
			}
		}
	}
	return true;
}

bool GObject::CallWriteXML( QXmlStreamWriter &qsw )
{
	StringManager *psm = &StringManager::getInstance();

	//Type
	qsw.writeStartElement(QString(GOBJXML_NODEPREFIX)+getTypeName());
	// ID
	qsw.writeTextElement(QString(GOBJXML_PROPPREFIX)+psm->GetXMLNodeIDName(), QString::number(nID));
	// DisplayName[OPT]
	if (strDisplayName.length())
	{
		qsw.writeTextElement(QString(GOBJXML_PROPPREFIX)+psm->GetXMLNodeDisplayNameName(), strDisplayName.c_str());
	}
	// LineColor[OPT]
	if (!pParent || !(pParent->lsLineColorSet == lsLineColorSet))
	{
		qsw.writeStartElement(QString(GOBJXML_PROPSPREFIX)+psm->GetXMLNodeLineColorsName());
		for (int i=0; i<_LINECOLOR_ENDINDEX; i++)
		{
			qsw.writeTextElement(QString(GOBJXML_PROPPREFIX)+QString::number(i), QString::number(lsLineColorSet.dwColors[i]));
		}
		qsw.writeEndElement();
	}
	// DisplayState[OPT]
	if (nDisplayState != GOBJ_DISPSTATE_NORMAL)
	{
		qsw.writeTextElement(QString(GOBJXML_PROPPREFIX)+psm->GetXMLNodeDisplayStateName(), QString::number(nDisplayState));
	}
	// Folded[OPT]
	if (bDisplayFolded == true)
	{
		qsw.writeTextElement(QString(GOBJXML_PROPPREFIX)+psm->GetXMLNodeFoldedName(), QString::number(bDisplayFolded));
	}
	
	if (!WriteXML(qsw))
	{
		return false;
	}

	// Children[OPT]
	if (!lstChildren.empty())
	{
		qsw.writeStartElement(QString(GOBJXML_PROPSPREFIX)+psm->GetXMLNodeChildrenName());
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

bool GObject::ReadXML( GObjectXMLNode * pnode )
{
	/*
	if (!super::ReadXML(pnode))
	{
		return false;
	}
	*/

	StringManager * psm = &StringManager::getInstance();

	// ID
	int prop_id = pnode->GetValue(psm->GetXMLNodeIDName()).toInt();
	if (nID != prop_id)
	{
		ASSERT(false);
		return false;
	}

	QString strValue;
	QString strPrefix;

	// DisplayName
	strValue = pnode->GetValue(psm->GetXMLNodeDisplayNameName());
	if (strValue.length())
	{
		setDisplayName(strValue.toUtf8());
	}
	// LineColor
	strPrefix = psm->GetXMLNodeLineColorsName();
	LineColorSet prop_ls;
	bool bHaveLS = true;
	for (int i=0; i<_LINECOLOR_ENDINDEX; i++)
	{
		strValue = pnode->GetValue(QString::number(i), strPrefix);
		if (!strValue.length())
		{
			bHaveLS = false;
			break;
		}
		prop_ls.SetColor(i, strValue.toUInt());
	}
	if (bHaveLS)
	{
		this->setLineColor(prop_ls);
	}
	// DisplayState
	strValue = pnode->GetValue(psm->GetXMLNodeDisplayStateName());
	if (strValue.length())
	{
		int prop_ds = strValue.toInt();
		this->setDisplayLock(prop_ds&GOBJ_DISPSTATE_LOCKED);
		this->setDisplayVisible(!(prop_ds&GOBJ_DISPSTATE_INVISIBLE));
	}
	// Folded
	strValue = pnode->GetValue(psm->GetXMLNodeFoldedName());
	if (strValue.length())
	{
		bool prop_folded = strValue.toInt();
		this->setDisplayFold(prop_folded);
	}

	if (!lstChildren.empty())
	{
		for (list<GObject *>::iterator it=lstChildren.begin(); it!=lstChildren.end(); ++it)
		{
			GObjectXMLNode * pSubNode = pnode->GetSubNodeByID((*it)->getID());
			if (!pSubNode || !(*it)->ReadXML(pSubNode))
			{
				return false;
			}
		}
	}

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
bool GPoint::ReadXML( GObjectXMLNode * pnode )
{
	if (!super::ReadXML(pnode))
	{
		return false;
	}

	StringManager *psm = &StringManager::getInstance();
	GObjectManager *pgm = &GObjectManager::getInstance();

	QString strPrefix = getStaticTypeName();
	QString strValue;

	// XY
	float prop_x = pnode->GetValue(psm->GetXMLNodeXName(), strPrefix).toFloat();
	float prop_y = pnode->GetValue(psm->GetXMLNodeYName(), strPrefix).toFloat();
	this->SetPosition(prop_x, prop_y);
	// ClingTo
	QString strClingToPrefix = strPrefix+psm->GetXMLNodeClingInfoName();
	strValue = pnode->GetValue(psm->GetXMLNodeClingToName(), strClingToPrefix);
	if (strValue.length())
	{
		int prop_clingto = strValue.toInt();
		GObject * prop_pClingTo = pgm->FindObjectByID(prop_clingto);
		ASSERT(prop_pClingTo->isLine());

		int prop_clingtype = GCLING_PROPORTION;
		strValue = pnode->GetValue(psm->GetXMLNodeClingTypeName(), strClingToPrefix);
		if (strValue.length())
		{
			prop_clingtype = strValue.toInt();
		}
		strValue = pnode->GetValue(psm->GetXMLNodeClingValName(), strClingToPrefix);
		float prop_clingval = strValue.toFloat();

		this->ClingTo((GLine *)prop_pClingTo, prop_clingval, prop_clingtype);
	}
	// MergeWith
	QString strMergeWithPrefix = strPrefix+psm->GetXMLNodeMergeWithName();
	int nMerge = 0;
	while (true)
	{
		strValue = pnode->GetValue(QString::number(nMerge), strMergeWithPrefix);
		if (strValue.length())
		{
			nMerge++;
			int prop_mergeid = strValue.toInt();
			GObject * prop_pMergeWith = pgm->FindObjectByID(prop_mergeid);
			ASSERT(prop_pMergeWith->isPoint());

			this->MergeWith((GPoint *)prop_pMergeWith);
		}
		else
		{
			break;
		}
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

	qsw.writeStartElement(QString(GOBJXML_ATTRPREFIX)+GPoint::getTypeName());

	// XY
	qsw.writeTextElement(QString(GOBJXML_PROPPREFIX)+psm->GetXMLNodeXName(), QString::number(x));
	qsw.writeTextElement(QString(GOBJXML_PROPPREFIX)+psm->GetXMLNodeYName(), QString::number(y));

	// ClingTo[OPT]
	GLine * pClingTo = clInfo.GetClingTo();
	if (pClingTo)
	{
		qsw.writeStartElement(QString(GOBJXML_PROPSPREFIX)+psm->GetXMLNodeClingInfoName());

		qsw.writeTextElement(QString(GOBJXML_PROPPREFIX)+psm->GetXMLNodeClingToName(), QString::number(pClingTo->getID()));
		int nClingType = clInfo.GetClingType();
		if (nClingType != GCLING_PROPORTION)
		{
			qsw.writeTextElement(QString(GOBJXML_PROPPREFIX)+psm->GetXMLNodeClingTypeName(), QString::number(nClingType));
		}
		qsw.writeTextElement(QString(GOBJXML_PROPPREFIX)+psm->GetXMLNodeClingValName(), QString::number(clInfo.GetClingVal()));

		qsw.writeEndElement();
	}
	// Merge[OPT]
	if (!mergeWithList.empty())
	{
		qsw.writeStartElement(QString(GOBJXML_PROPSPREFIX)+psm->GetXMLNodeMergeWithName());

		int i=0;
		for (list<GPoint *>::iterator it=mergeWithList.begin(); it!=mergeWithList.end(); ++it)
		{
			qsw.writeTextElement(QString(GOBJXML_PROPPREFIX)+QString::number(i), QString::number((*it)->getID()));
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

bool GAnchorPoint::ReadXML( GObjectXMLNode * pnode )
{
	if (!super::ReadXML(pnode))
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
	/*
	StringManager *psm = &StringManager::getInstance();
	if (pHandle)
	{
		qsw.writeTextElement(QString(GOBJXML_PROPPREFIX)+psm->GetXMLNodeHandleName(), QString::number(pHandle->getID()));
	}
	*/
	return true;
}

/************************************************************************/
/* GHandlePoint                                                         */
/************************************************************************/

bool GHandlePoint::ReadXML( GObjectXMLNode * pnode )
{
	if (!super::ReadXML(pnode))
	{
		return false;
	}

	StringManager *psm = &StringManager::getInstance();
	GObjectManager *pgm = &GObjectManager::getInstance();

	QString strPrefix = getStaticTypeName();
	QString strValue;

	// BindWith
	strValue = pnode->GetValue(psm->GetXMLNodeBindWithName(), strPrefix);
	if (strValue.length())
	{
		int prop_bindid = strValue.toInt();
		GObject * prop_pBindWith = pgm->FindObjectByID(prop_bindid);
		ASSERT(prop_pBindWith->isHandlePoint());

		this->BindWith((GHandlePoint *)prop_pBindWith);
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
	// BindWith
	if (pBindWith)
	{
		qsw.writeTextElement(QString(GOBJXML_PROPPREFIX)+psm->GetXMLNodeHandleName(), QString::number(pBindWith->getID()));
	}
	return true;
}

/************************************************************************/
/* GLine                                                                */
/************************************************************************/

bool GLine::ReadXML( GObjectXMLNode * pnode )
{
	if (!super::ReadXML(pnode))
	{
		return false;
	}

	StringManager *psm = &StringManager::getInstance();

	// RenderStyle
	QString strPrefix = getStaticTypeName();
	QString strValue;

	strValue = pnode->GetValue(psm->GetXMLNodeStyleName(), strPrefix);
	if (strValue.length())
	{
		int prop_rs = strValue.toInt();
		this->SetLineRenderStyle(prop_rs);
	}
	// SAInfo
	QString strSAInfoPrefix = strPrefix + psm->GetXMLNodeSAName();
	strValue = pnode->GetValue(psm->GetXMLNodeSASizeName(), strSAInfoPrefix);
	if (strValue.length())
	{
		float prop_fsa = strValue.toFloat();
		strValue = pnode-> GetValue(psm->GetXMLNodeSAFlagName(), strSAInfoPrefix);
		int prop_nflag = strValue.toInt();
		this->saInfo.SetSA(prop_fsa, prop_nflag);
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

	/*
	if (plbegin)
	{
		qsw.writeTextElement(QString(GOBJXML_PROPPREFIX)+psm->GetXMLNodeBeginName(), QString::number(plbegin->getID()));
	}
	if (plend)
	{
		qsw.writeTextElement(QString(GOBJXML_PROPPREFIX)+psm->GetXMLNodeEndName(), QString::number(plend->getID()));
	}
	if (pmid)
	{
		qsw.writeTextElement(QString(GOBJXML_PROPPREFIX)+psm->GetXMLNodeMidName(), QString::number(pmid->getID()));
	}
	*/

	qsw.writeStartElement(QString(GOBJXML_ATTRPREFIX) + GLine::getTypeName());
	// RenderStyle[OPT]
	if (nLineRenderStyle != RHLINESTYLE_LINE)
	{
		qsw.writeTextElement(QString(GOBJXML_PROPPREFIX)+psm->GetXMLNodeStyleName(), QString::number(nLineRenderStyle));
	}
	// SA[OPT]
	if (saInfo.GetRawSA())
	{
		qsw.writeStartElement(QString(GOBJXML_PROPSPREFIX)+psm->GetXMLNodeSAName());
		qsw.writeTextElement(QString(GOBJXML_PROPPREFIX)+psm->GetXMLNodeSAFlagName(), QString::number(saInfo.GetFlag()));
		qsw.writeTextElement(QString(GOBJXML_PROPPREFIX)+psm->GetXMLNodeSASizeName(), QString::number(saInfo.GetRawSA()));
		qsw.writeEndElement();
	}
	qsw.writeEndElement();


	return true;
}

/************************************************************************/
/* GLayer                                                               */
/************************************************************************/

bool GLayer::ReadXML( GObjectXMLNode * pnode )
{
	if (!super::ReadXML(pnode))
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
	qsw.writeStartElement(QString(GOBJXML_ATTRPREFIX) + GLayer::getTypeName());

	qsw.writeTextElement(QString(GOBJXML_PROPPREFIX)+psm->GetXMLNodeLayerIDName(), QString::number(layerID));

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
}	\
const char * TYPENAME::getStaticTypeName()	\
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
