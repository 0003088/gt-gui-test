#include "treeitem.hpp"
#include <QtQml>
#include <QDebug>
#include <kdbproposal.h>

using namespace kdb;

TreeItem::TreeItem(const QString &baseName, const QString &name, const kdb::Key &key, TreeItemPtr parent)
: m_baseName(baseName)
, m_name(name)
, m_key(key)
, m_parent(parent)
, m_metaData(NULL)
, m_isDirty(false)
{
	setValue();

	if(m_key)
	{
		m_metaData = new MetaModel(m_key);
		populateMetaModel();
	}
}

TreeItem::TreeItem(const TreeItem &other)
: QObject()
, m_baseName(other.m_baseName)
, m_name(other.m_name)
, m_key(other.key().dup())
, m_parent(NULL)
, m_value(other.m_value)
, m_metaData(NULL)
, m_isDirty(false)
{
	if(other.m_children.count() > 0)
	{
		foreach(TreeItemPtr item, other.m_children)
		{
			m_children.append(TreeItemPtr(new TreeItem(*item)));
		}
	}

	if(other.m_metaData)
	{
		m_metaData = new MetaModel(m_key);
		foreach(MetaItem* item , other.m_metaData->children())
		{
			m_metaData->insertRow(m_metaData->rowCount(), new MetaItem(*item));
		}
	}
}

TreeItem::~TreeItem()
{
	qDebug() << m_name << " destroyed";
	delete m_metaData;
}

QString TreeItem::baseName() const
{
	return m_baseName;
}

void TreeItem::setBaseName(const QString &baseName)
{
	int index = m_name.lastIndexOf("/");

	if(index != -1)
	{
		m_name.replace(index, m_name.length() - index, "/" + baseName);
	}

	if(!m_key)
		m_key = Key(m_name.toStdString(), KEY_END);
	else
		m_key = m_key.dup();

	try{
		if(m_key.getBaseName().compare(baseName.toStdString()) != 0)
			m_key.setBaseName(baseName.toStdString());
	}
	catch(KeyInvalidName const& ex){
		//		emit showMessage(tr("Error"), tr("Could not set name because Keyname \"%1\" is invalid.").arg(name), ex.what());
		return;
	}

	m_baseName = baseName;
}

QString TreeItem::name() const
{
	return m_name;
}

QString TreeItem::toString() const
{
	return QString(m_name + ", " + m_parent->name());
}

QString TreeItem::getCurrentArrayNo() const
{
	TreeItemPtr max (nullptr);

	foreach (TreeItemPtr item, m_children)
	{
		if (item->baseName().startsWith ("#"))
		{
			max = item;
		}
	}

	if (max)
	{
		Key k = max->key();
		ckdb::elektraArrayIncName (k.getKey ());
		return QString::fromStdString (k.getBaseName ());
	}

	return "#0";
}

void TreeItem::setName(const QString &name)
{
	m_name = name;

	setKeyName(name);

	if(m_children.count() > 0)
	{
		foreach(TreeItemPtr item, m_children){
			item->setName(m_name + "/" + item->baseName());
		}
	}
}

QVariant TreeItem::value() const
{
	return m_value;
}

void TreeItem::setValue(const QVariant &value)
{
	if(!m_key)
		m_key = Key(m_name.toStdString(), KEY_END);
	else
		m_key = m_key.dup();

	if(m_key.getString().compare(value.toString().toStdString()) != 0)
	{
		m_key.setString(value.toString().toStdString());
		m_value = value;
	}
}

void TreeItem::setMetaData(const QVariantMap &metaData)
{
	if(!m_metaData)
		m_metaData = new MetaModel(m_key);

	m_metaData->setMetaData(metaData);
}

kdb::Key TreeItem::key() const
{
	return m_key;
}

void TreeItem::setKey(const kdb::Key &key)
{
	m_key = key;
}

QList<TreeItemPtr> TreeItem::children() const
{
	return m_children;
}

MetaModel *TreeItem::metaData() const
{
	return m_metaData;
}

TreeItemPtr TreeItem::parent() const
{
	return m_parent;
}

void TreeItem::setParent(TreeItemPtr parent)
{
	m_parent = parent;
}

TreeItemPtr TreeItem::child(int row) const
{
	if(row < 0 || row > m_children.count()-1)
		return TreeItemPtr();

	return m_children.at(row);
}

int TreeItem::childCount() const
{
	return m_children.count();
}

int TreeItem::columnCount() const
{
	return 1;
}

int TreeItem::row() const
{
	if (m_parent)
		return getRowByName();

	return 0;
}

bool TreeItem::hasChild(const QString& name) const
{
	if(childCount() > 0)
	{
		foreach (TreeItemPtr node, m_children)
		{
			if (node->baseName() == name)
			{
				return true;
			}

		}

	}

	return false;
}

TreeItemPtr TreeItem::getChildByName(QString& name) const
{
	if(childCount() > 0)
	{
		foreach (TreeItemPtr node, m_children)
		{
			if (node->baseName() == name)
			{
				return node;
			}

		}

	}

	return TreeItemPtr();
}

bool TreeItem::isDirty() const
{
	return m_isDirty;
}

bool TreeItem::childrenAreLeaves() const
{
	foreach(TreeItemPtr item, m_children)
	{
		if(item->childCount() > 0)
			return false;
	}

	return true;
}

bool TreeItem::insertChildren(int index, QList<QSharedPointer<TreeItem> > items)
{
	if (index < 0 || index > m_children.count())
		return false;

	foreach(TreeItemPtr item, items)
	{
		m_children.insert(index, item);
	}

	return true;
}

void TreeItem::setIsDirty(bool value)
{
	m_isDirty = value;
}

void TreeItem::appendChild(TreeItemPtr child)
{
	m_children.append(child);
}

void TreeItem::clear()
{
	m_children.clear();
}

void TreeItem::refreshArrayNumbers()
{
	QList<TreeItemPtr> arrayElements;

	foreach (TreeItemPtr node, m_children)
	{
		if (node->baseName().startsWith ("#"))
		{
			arrayElements.append (node);
		}
	}

	if (!arrayElements.isEmpty ())
	{

		arrayElements.at (0)->setBaseName ("#0");

		if (arrayElements.count () > 1)
		{

			for (int i = 1; i < arrayElements.count (); i++)
			{
				Key k = arrayElements.at (i - 1)->key ().dup ();
				ckdb::elektraArrayIncName (k.getKey ());
				arrayElements.at (i)->setBaseName (QString::fromStdString (k.getBaseName ()));
			}
		}
	}
}

void TreeItem::populateMetaModel()
{
	if (m_key)
	{
		m_key.rewindMeta();
		m_metaData->clear();

		while (m_key.nextMeta())
		{
			MetaItem* item = new MetaItem(QString::fromStdString(m_key.currentMeta().getName()), QVariant::fromValue(QString::fromStdString(m_key.currentMeta().getString())));
			m_metaData->insertRow(m_metaData->rowCount(), item);
		}
	}
}

void TreeItem::setValue()
{
	if (m_key && m_key.isString())
		m_value = QVariant::fromValue(QString::fromStdString(m_key.getString()));
	else if (m_key && m_key.isBinary())
		m_value = QVariant::fromValue(QString::fromStdString(m_key.getBinary()));
}

void TreeItem::setKeyName(const QString &name)
{
	if(m_key)
	{
		try
		{
			m_key.setName(name.toStdString());
		}
		catch(KeyInvalidName ex)
		{
			//			emit showMessage(tr("Error"), tr("Could not set name because Keyname \"%1\" is invalid.").arg(name), ex.what());
			return;
		}
	}
}

int TreeItem::getRowByName() const
{
	if(m_parent)
	{
		foreach(TreeItemPtr item, m_parent->children())
			if(item->name() == m_name)
				return m_parent->children().indexOf(item);
	}

	return 0;
}

void TreeItem::updateNode(const Key& key)
{
	m_key = key;
	setValue();
	populateMetaModel();
}

bool TreeItem::insertChild(int index, TreeItemPtr item)
{
	if (index < 0 || index > m_children.count())
		return false;

	m_children.insert(index, item);

	return true;
}

bool TreeItem::removeChildren(int row, int count)
{
	if(row < 0 || row + count > m_children.count())
		return false;

	for(int i = 0; i < count; i++)
	{
		m_children.at(row)->setParent(TreeItemPtr());
		m_children.removeAt(row);
	}

	return true;
}
