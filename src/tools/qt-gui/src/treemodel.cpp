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

TreeModel::TreeModel(QObject *parentModel) : m_root ("/", KEY_END), m_kdb ()
{
	Q_UNUSED(parentModel);
}

TreeModel::TreeModel(MergingKDB *kdb, QObject *parentModel) : m_root ("/", KEY_END), m_kdb (kdb)
{
	Q_UNUSED(parentModel);
	m_rootItem = TreeItemPtr(new TreeItem("root", "root", 0, TreeItemPtr(NULL)));
	connectDBus ();
}

TreeModel::TreeModel (const TreeModel & other)
: QAbstractItemModel (), m_rootItem (other.m_rootItem), m_root (other.m_root), m_kdb (other.m_kdb)
{
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
	Q_UNUSED(parent)
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

QVariant TreeModel::find(const QString &term)
{
//	auto searchResults = new TreeViewModel;
//	FindVisitor fVisit (searchResults, term);
//	accept (fVisit);

//	if (searchResults->rowCount () == 0)
//	{
//		searchResults->model ().append (
//		ConfigNodePtr (new ConfigNode ("NotfoundNode", tr ("There were no results matching your query."), nullptr, this)));
//	}

//	QQmlEngine::setObjectOwnership (searchResults, QQmlApplicationEngine::CppOwnership);

//	return QVariant::fromValue (searchResults);
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
	kdb::KeySet config;
	m_kdb->get (config, m_root);
	populateModel (config);
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

QStringList TreeModel::mountedBackends()
{
	KeySet keySet = collectCurrentKeySet ();

	Backends::BackendInfoVector mtab = Backends::getBackendInfo (keySet);

	QStringList mountedBends;

	for (Backends::BackendInfoVector::const_iterator it = mtab.begin (); it != mtab.end (); ++it)
	{
		mountedBends.append (QString::fromStdString (it->name));
	}

	// cannot read the size of the QStringList in QML
	if (mountedBends.isEmpty ()) mountedBends.append ("empty");

	return mountedBends;
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

MergeConflictStrategy *TreeModel::getMergeStrategy(const QString &mergeStrategy)
{
	if (mergeStrategy == "Preserve")
		return new AutoMergeStrategy ();
	else if (mergeStrategy == "Ours")
		return new OneSideStrategy (OURS);
	else if (mergeStrategy == "Theirs")
		return new OneSideStrategy (THEIRS);
	else if (mergeStrategy == "Base")
		return new OneSideStrategy (BASE);
	else if (mergeStrategy == "None")
		return nullptr;

	return nullptr;
}

void TreeModel::connectDBus()
{
	if (QDBusConnection::sessionBus ().connect (QString (), "/org/libelektra/configuration", "org.libelektra", QString (), this,
	SLOT (configChanged (QString))))
	{
		ELEKTRA_LOG ("Successfully connected to DBus");
	}
	else
	{
		ELEKTRA_LOG ("Failed to connect to DBus");
	}
}

void TreeModel::configChanged(QString msg)
{
	Q_UNUSED (msg)
	ELEKTRA_LOG ("config changed: %s", msg.toLocal8Bit ().data ());

	synchronize ();
//	refresh ();
}

QList<TreeItemPtr> TreeModel::getItemsToInsert() const
{
	return m_itemsToInsert;
}

KeySet TreeModel::collectCurrentKeySet()
{
//	KeySetVisitor ksVisit;
//	accept (ksVisit);

//	return ksVisit.getKeySet ();
}

void TreeModel::setItemsToInsert(const QList<TreeItemPtr> &itemsToInsert)
{
	m_itemsToInsert = itemsToInsert;
}

void TreeModel::accept(Visitor &visitor)
{
//	visitor.visit (this);
}

void TreeModel::synchronize()
{
	KeySet ours = collectCurrentKeySet ();

	try
	{
#if DEBUG && VERBOSE
		std::cout << "guitest: start" << std::endl;
		printKeys (ours, ours, ours);
#endif

		ThreeWayMerge merger;
		AutoMergeConfiguration configuration;
		configuration.configureMerger (merger);

		// write our config
		m_kdb->synchronize (ours, m_root, merger);

#if DEBUG && VERBOSE
		std::cout << "guitest: after get" << std::endl;
		printKeys (ours, ours, ours);
#endif

		createNewNodes (ours);
	}
	catch (MergingKDBException const & exc)
	{
		QStringList conflicts = getConflicts (exc.getConflicts ());
		emit showMessage (tr ("Error"), tr ("Synchronizing failed, conflicts occured."), conflicts.join ("\n"));
	}
	catch (KDBException const & e)
	{
		emit showMessage (tr ("Error"), tr ("Synchronizing failed, could not write merged configuration."), e.what ());
	}
}

QStringList TreeModel::getConflicts (KeySet const & conflictSet)
{
	QStringList conflicts;
	conflictSet.rewind ();
	Key current;

	while ((current = conflictSet.next ()))
	{
		QString ourConflict = QString::fromStdString (current.getMeta<string> ("conflict/operation/our"));
		QString theirConflict = QString::fromStdString (current.getMeta<string> ("conflict/operation/their"));

		conflicts.append (QString::fromStdString (current.getName ()));
		conflicts.append ("Ours: " + ourConflict + ", Theirs " + theirConflict);
		conflicts.append ("\n");
	}
	return conflicts;
}

void TreeModel::exportConfiguration(QModelIndex idx, QString format, QString file)
{
//	KeySet ks = collectCurrentKeySet ();
//	Key root = parentModel->model ().at (idx)->getKey ();

//	// Node is only a filler
//	if (!root) root = Key (parentModel->model ().at (idx)->getPath ().toStdString (), KEY_END);

//	KeySet part (ks.cut (root));

//	string formatString = format.toStdString ();
//	string fileString = file.remove ("file://").toStdString ();

//	Modules modules;
//	PluginPtr plugin = modules.load (formatString);

//	Key errorKey (root);
//	errorKey.setString (fileString);

//	plugin->set (part, errorKey);

//	stringstream ws;
//	stringstream es;
//	QString warnings;
//	QString errors;

//	printWarnings (ws, errorKey);
//	warnings = QString::fromStdString (ws.str ());
//	printError (es, errorKey);
//	errors = QString::fromStdString (es.str ());

//	if (errors.isEmpty () && !warnings.isEmpty ())
//		emit showMessage (tr ("Information"), tr ("Successfully exported configuration below %1 to %2, warnings were issued.")
//	.arg (QString::fromStdString (root.getName ()), file),
//	"");
//	else if (!errors.isEmpty () && warnings.isEmpty ())
//		emit showMessage (
//	tr ("Error"),
//	tr ("Failed to export configuration below %1 to %2.").arg (QString::fromStdString (root.getName ()), file), errors);
//	else if (!errors.isEmpty () && !warnings.isEmpty ())
//		emit showMessage (
//	tr ("Error"),
//	tr ("Failed to export configuration below %1 to %2.").arg (QString::fromStdString (root.getName ()), file),
//	warnings + "\n" + errors);
}

void TreeModel::importConfiguration(const QString &name, QString &file, QString &format, const QVariantList &mergeStrategies)
{
	Key root (name.toStdString (), KEY_END);
	KeySet originalKeys = collectCurrentKeySet ();
	KeySet base = originalKeys.cut (root);
	printWarnings (cerr, root);

	KeySet importedKeys;

	string formatString = format.toStdString ();
	string fileString = file.remove ("file://").toStdString ();

	Modules modules;
	PluginPtr plugin = modules.load (formatString);

	Key errorKey (root);
	errorKey.setString (fileString);

	plugin->get (importedKeys, errorKey);

	stringstream ws;
	stringstream es;
	QString warnings;
	QString errors;

	printWarnings (ws, errorKey);
	warnings = QString::fromStdString (ws.str ());
	printError (es, errorKey);
	errors = QString::fromStdString (es.str ());

	if (!errors.isEmpty ())
	{
		emit showMessage (tr ("Error"),
		tr ("Failed to import configuration from %1 to %2.").arg (file, QString::fromStdString (root.getName ())),
		errors);
		return;
	}

	ThreeWayMerge merger;

	foreach (QVariant s, mergeStrategies)
	{
		MergeConflictStrategy * strategy = getMergeStrategy (s.toString ());

		if (strategy) merger.addConflictStrategy (strategy);
	}

	MergeResult result;

	try
	{
		result = merger.mergeKeySet (
		MergeTask (BaseMergeKeys (base, root), OurMergeKeys (base, root), TheirMergeKeys (importedKeys, root), root));
	}
	catch (...) // TODO: Which exceptions are possible?
	{
		emit showMessage (tr ("Error"), tr ("Could not merge keys."), "");
	}

	if (!result.hasConflicts ())
	{
		createNewNodes (result.getMergedKeys ());

		if (importedKeys.size () > 0)
			emit showMessage (tr ("Information"), tr ("Successfully imported %1 keys.").arg (importedKeys.size ()), "");
	}
	else
	{
		KeySet conflictSet = result.getConflictSet ();
		QStringList conflicts;
		conflictSet.rewind ();
		Key current;

		while ((current = conflictSet.next ()))
		{
			QString ourConflict = QString::fromStdString (current.getMeta<string> ("conflict/operation/our"));
			QString theirConflict = QString::fromStdString (current.getMeta<string> ("conflict/operation/their"));

			conflicts.append (QString::fromStdString (current.getName ()));
			conflicts.append ("Ours: " + ourConflict + ", Theirs " + theirConflict);
			conflicts.append ("\n");
		}

		emit showMessage (
		tr ("Error"),
		tr ("The were conflicts importing %1 (%2 format) into %3, no configuration was imported.").arg (file, format, name),
		conflicts.join ("\n"));
	}
}

void TreeModel::unMountBackend(QString backendName)
{
	KeySet keySet = collectCurrentKeySet ();
	Backends::umount (backendName.toStdString (), keySet);
	populateModel (keySet);
	synchronize ();
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

//bool TreeModel::hasChildren(const QModelIndex &parent) const
//{
//	TreeItem* parentItem = getItem(parent);

//	Q_ASSERT(parentItem);

//	return parentItem->childCount() > 0;
//}

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

