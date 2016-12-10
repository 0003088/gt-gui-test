#include "treemodel.hpp"

#include <backends.hpp>
#include <kdbconfig.h> // for DEBUG and VERBOSE
#include <kdbease.h>
#include <kdbproposal.h> // for namespaces
#include <modules.hpp>
#include <plugin.hpp>
#include <plugins.hpp>
#include <toolexcept.hpp>

#include "kdblogger.h"
#include <merging/automergeconfiguration.hpp>
#include <merging/automergestrategy.hpp>
#include <merging/mergeconflictstrategy.hpp>
#include <merging/onesidestrategy.hpp>
#include <merging/onesidevaluestrategy.hpp>
#include <merging/threewaymerge.hpp>

using namespace std;
using namespace kdb;
using namespace kdb::tools;
using namespace kdb::tools::merging;

TreeModel::TreeModel(QObject *parent) : QAbstractItemModel(parent)
{
	m_rootItem = TreeItemPtr(new TreeItem("root", "root", 0, TreeItemPtr(NULL)));
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	TreeItem* parentItem = getItem(parent);
	TreeItem* childItem = parentItem->child(row).data();

	Q_ASSERT(parentItem);

	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &child) const
{
	if (!child.isValid())
		return QModelIndex();

	TreeItem* childItem = getItem(child);
	TreeItem* parentItem = childItem->parent().data();

	Q_ASSERT(parentItem);
	Q_ASSERT(childItem);

	if(parentItem == m_rootItem.data())
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
	TreeItem* parentItem = getItem(parent);

	Q_ASSERT(parentItem);

	return parentItem->childCount();
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
	return m_rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	TreeItem* item = getItem(index);

	Q_ASSERT(item);

	switch (role)
	{
	case BaseNameRole:
		return QVariant::fromValue(item->baseName());

	case NameRole:
		return QVariant::fromValue(item->name());

	case ValueRole:
		return QVariant::fromValue(item->value());

	case ChildrenRole:
		return QVariant::fromValue(item->children());

	case ParentRole:
		return QVariant::fromValue(item->parent());

	case ChildCountRole:
		return QVariant::fromValue(item->childCount());

	case ChildrenAreLeavesRole:
		return QVariant::fromValue(item->childrenAreLeaves());

	case RootRole:
		return QVariant::fromValue(m_rootItem);

	case IsNullRole: {
		if(item->key())
			return QVariant::fromValue(false);
		return QVariant::fromValue(true);
	}

	case MetaDataRole:{
		QQmlApplicationEngine::setObjectOwnership(item->metaData(), QQmlApplicationEngine::CppOwnership);
		return QVariant::fromValue(item->metaData());
	}

	case ItemRole:{
		QQmlApplicationEngine::setObjectOwnership(item, QQmlApplicationEngine::CppOwnership);
		return QVariant::fromValue(item->parent()->child(item->row()));
	}

	case HierarchyRole:
		return QVariant::fromValue(item->name().count("/"));

	default:
		return QVariant::fromValue(item);
	}
}

bool TreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (!index.isValid())
		return false;

	TreeItem* item = getItem(index);

	Q_ASSERT(item);

	switch (role)
	{
	case BaseNameRole:
		if(item->baseName() != value.toString())
		{
			item->setBaseName(value.toString());
			item->setIsDirty(true);
		}
		break;

	case ValueRole:
		if(item->value() != value)
			item->setValue(value);
		break;

	case MetaDataRole:
		item->setMetaData(value.toMap());
		break;
	}

	emit dataChanged(index, index);

	return true;
}

void TreeModel::populateModel()
{
	try
	{
		kdb::KeySet config;
		m_kdb.get(config, "/");
		populateModel(config);
	}
	catch(kdb::KDBException const& e)
	{
		//emit showMessage(QObject::tr("Error"), QObject::tr("Populating model failed, could not read from configuration."), e.what());
		//throw;
	}
}

void TreeModel::populateModel(KeySet const & keySet)
{
	//	GUIBasicKeySet::setBasic(keySet);

	m_rootItem->clear();

	using namespace ckdb; // for namespaces

	for (int i=KEY_NS_FIRST; i<=KEY_NS_LAST; ++i)
	{
		elektraNamespace ns = static_cast<elektraNamespace>(i);
		TreeItemPtr toAdd;

		switch (ns)
		{
		case KEY_NS_SPEC:
			toAdd = TreeItemPtr(new TreeItem("spec", "spec", 0, m_rootItem));
			break;
		case KEY_NS_PROC:
			// TODO: add generic commandline parsing
			break;
		case KEY_NS_DIR:
			toAdd = TreeItemPtr(new TreeItem("dir", "dir", 0, m_rootItem));
			break;
		case KEY_NS_USER:
			toAdd = TreeItemPtr(new TreeItem("user", "user", 0, m_rootItem));
			break;
		case KEY_NS_SYSTEM:
			toAdd = TreeItemPtr(new TreeItem("system", "system", 0, m_rootItem));
			break;
		case KEY_NS_EMPTY:
			break;
		case KEY_NS_NONE:
			break;
		case KEY_NS_META:
			break;
		case KEY_NS_CASCADING:
			break;
		}
		if (toAdd)
		{
			m_rootItem->appendChild(toAdd);
		}

	}

	createNewNodes(keySet);
}

void TreeModel::createNewNodes(KeySet keySet)
{
	keySet.rewind();

	while (keySet.next())
	{
		Key k = keySet.current();
		QStringList keys = getSplittedKeyname(k);
		QString rootString = keys.takeFirst();

		for(int i = 0; i < m_rootItem->childCount(); i++)
		{
			if(rootString == m_rootItem->child(i)->baseName())
				sink(m_rootItem->child(i), keys, k);
		}
	}
}

void TreeModel::sink(TreeItemPtr item, QStringList keys, const Key& key)
{
	if (keys.length() == 0)
		return;

	bool isLeaf = (keys.length() == 1);

	QString baseName =  keys.takeFirst();

	if (item->hasChild(baseName))// && !item->getChildByName(baseName)->isDirty())
	{
		sink(item->getChildByName(baseName), keys, key);
	}
	else
	{
		//		if(item->hasChild(name))
		//			item->children()->removeRow(item->getChildIndexByName(name));//TODO

		TreeItemPtr newNode;

		if(isLeaf)
			newNode = TreeItemPtr(new TreeItem(baseName, (item->name() + "/" + baseName), key, item));
		else
			newNode = TreeItemPtr(new TreeItem(baseName, (item->name() + "/" + baseName), NULL, item));

		item->appendChild(newNode);

		sink(newNode, keys, key);
	}
}

QStringList TreeModel::getSplittedKeyname(const Key &key)
{
	QStringList names;

	for (Key::iterator i = key.begin(); i != key.end(); ++i)
	{
		names.append(QString::fromStdString(*i));
	}

	return names;
}

TreeItem* TreeModel::getItem(const QModelIndex &index) const
{
	if(index.isValid()) {
		TreeItem* item = static_cast<TreeItem*>(index.internalPointer());

		if (item)
			return item;
	}

	return m_rootItem.data();
}

TreeItemPtr TreeModel::getItemPtr(const QModelIndex &index) const
{
	if(index.isValid()) {
		TreeItemPtr item = qvariant_cast<TreeItemPtr>(data(index, ItemRole));

		if (item)
			return item;
	}

	return m_rootItem;
}

QList<TreeItemPtr> TreeModel::getItemsToInsert() const
{
	return m_itemsToInsert;
}

void TreeModel::setItemsToInsert(const QList<TreeItemPtr> &itemsToInsert)
{
	m_itemsToInsert = itemsToInsert;
}

//bool TreeModel::insertRow(int row, const QModelIndex& parent, TreeItemPtr item, bool addParent)
//{
//	TreeItem *parentItem = getItem(parent);
//	Q_ASSERT(parentItem);
//	bool success;
//	bool noChildren = false;

//	if(parentItem->childCount() == 0)
//		noChildren = true;

//	if(noChildren){
//		qDebug() << "latbc";
//		emit layoutAboutToBeChanged();
//	}

//	beginInsertRows(parent, row, row);
//	if(addParent)
//	{
//		TreeItemPtr p = getItemPtr(parent);
//		qDebug() << p->name();
//		item->setParent(p);
//	}
//	success = parentItem->insertChild(row, item);
//	endInsertRows();

//	if(noChildren){
//		qDebug() << "lc";
//		emit layoutChanged();
//	}

//	return success;
//}

bool TreeModel::insertRows(int row, int count, const QModelIndex &parent)
{
	TreeItem *parentItem = getItem(parent);
	Q_ASSERT(parentItem);
	bool success = true;
	bool noChildren = parentItem->childCount() == 0;

	QList<TreeItemPtr> items = getItemsToInsert();

	if(items.isEmpty())
	{
		qDebug() << "No items to insert";
		return false;
	}

	Q_ASSERT(count == items.count());

	beginInsertRows(parent, row, row + count - 1);
	foreach (TreeItemPtr item, items) {
		TreeItemPtr p = getItemPtr(parent);
		Q_ASSERT(p);
		item->setParent(p);
	}
	success = parentItem->insertChildren(row, items);
	endInsertRows();

	if(noChildren){
		emit invalidateFilter();
	}

	return success;
}

bool TreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
	TreeItem *parentItem = getItem(parent);
	Q_ASSERT(parentItem);
	bool success = true;
	bool noChildren = parentItem->childCount() == count;

	beginRemoveRows(parent, row, row + count - 1);
	success = parentItem->removeChildren(row, count);
	endRemoveRows();

	if(noChildren){
		emit invalidateFilter();
	}

	return success;
}

bool TreeModel::hasChildren(const QModelIndex &parent) const
{
	TreeItem* parentItem = getItem(parent);

	Q_ASSERT(parentItem);

	return parentItem->childCount() > 0;
}

Qt::ItemFlags TreeModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;

	return  Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | QAbstractItemModel::flags(index);
}

Path TreeModel::pathFromIndex(const QModelIndex &index){

	QModelIndex iter = index;
	Path path;

	while(iter.isValid()){
		path.prepend(PathItem(iter.row(), iter.column()));
		iter = iter.parent();
	}

	return path;
}

QModelIndex TreeModel::pathToIndex(const Path &path){

	QModelIndex iter;

	for(int i = 0; i < path.size(); i++){
		iter = this->index(path[i].first, path[i].second, iter);
	}

	return iter;
}

QHash<int, QByteArray> TreeModel::roleNames() const
{
	QHash<int, QByteArray> roles;

	roles[BaseNameRole] = "baseName";//257
	roles[NameRole] = "name";
	roles[ValueRole] = "value";
	roles[ChildrenRole] = "children";//260
	roles[ParentRole] = "parent";
	roles[ChildCountRole] = "childCount";
	roles[ChildrenAreLeavesRole] = "childrenAreLeaves";//263
	roles[RootRole] = "root";
	roles[IsNullRole] = "isNull";
	roles[MetaDataRole] = "metaData";//266
	roles[ItemRole] = "item";
	roles[HierarchyRole] = "hierarchy";
	qDebug() << "BaseNameRole" << BaseNameRole;
	return roles;
}

