#pragma once

#include "MainDependency.h"

class GObjectXMLProperty
{
public:
	GObjectXMLProperty(QString name, QString value, list<QString> *plstPrefix=NULL);

	QString getName(){return name;};
	QString getValue(){return value;};

	QString name;
	QString value;
};

class GObjectXMLNode
{
public:
	GObjectXMLNode(QString name){this->name=name;};

	bool AddChild(QString name);
	bool AddProperty(QString name, QString value);

	QString GetValue(QString propname, QString strPrefix="");
	GObjectXMLNode * GetSubNodeByID(int nID);

	bool PushPrefix(QString name);
	bool PopPrefix();

	bool DestroyNodes();

	list<GObjectXMLProperty> lstProp;
	list<GObjectXMLNode> lstChildren;
	list<QString> lstPrefix;

	QString name;
};

class GObjectXMLBase
{
public:
	static GObjectXMLBase& getInstance() { static GObjectXMLBase instance; return instance; }

private:
	GObjectXMLBase();
	~GObjectXMLBase();
	GObjectXMLBase(GObjectXMLBase const&);
	void operator=(GObjectXMLBase const&);

public:

	bool ReadXML(QXmlStreamReader &qsr);
	void DestroyNodes();
	GObjectXMLNode * GetBase(){ASSERT(pbasenode);return pbasenode;};

	GObjectXMLNode * pbasenode;
};
