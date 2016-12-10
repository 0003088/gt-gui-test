#ifndef TREEMODEL_HPP
#define TREEMODEL_HPP

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QDebug>
#include <QList>
#include <QtDBus/QtDBus>
#include <QtQml>
#include <backend.hpp>
#include <kdb.hpp>
#include <keyio.hpp>

#include <merging/automergeconfiguration.hpp>
#include <merging/mergingkdb.hpp>

#include "treeitem.hpp"
//#include "findvisitor.hpp"
//#include "keysetvisitor.hpp"
//#include "printvisitor.hpp"

class Visitor;

typedef QPair<int, int> PathItem;
typedef QList<PathItem> Path;

class TreeModel : public QAbstractItemModel
{

	Q_OBJECT

public:
	enum TreeModelRoles
	{
		BaseNameRole = Qt::UserRole + 1,
		NameRole,
		ValueRole,
		ChildrenRole,
		ParentRole,
		ChildCountRole,
		ChildrenAreLeavesRole,
		RootRole,
		IsNullRole,
		MetaDataRole,
		ItemRole,
		HierarchyRole
	};

	explicit TreeModel(QObject *parent = 0);
	TreeModel(const TreeModel &other) {Q_UNUSED(other)}

	QModelIndex				index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex				parent(const QModelIndex &child) const;
	QModelIndex				pathToIndex(const Path &path);

	int						rowCount(const QModelIndex &parent = QModelIndex()) const;
	int						columnCount(const QModelIndex &parent = QModelIndex()) const;

	QVariant				data(const QModelIndex &index, int role) const;

	bool					setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
	//	bool					insertRow(int row, const QModelIndex &parent, TreeItemPtr item, bool addParent = true);
	bool					insertRows(int row, int count, const QModelIndex &parent);
	bool					removeRows(int row, int count, const QModelIndex &parent);
	bool					hasChildren(const QModelIndex &parent) const;

	void					sink(TreeItemPtr item, QStringList keys, const kdb::Key &key);
	void					populateModel(const kdb::KeySet &keySet);
	void					populateModel();
	void					createNewNodes(kdb::KeySet keySet);
	void					setItemsToInsert(const QList<TreeItemPtr> &itemsToInsert);

	QStringList				getSplittedKeyname(const kdb::Key &key);

	Qt::ItemFlags			flags(const QModelIndex &index) const;

	QHash<int, QByteArray>	roleNames() const;

	Path					pathFromIndex(const QModelIndex &index);

	QList<TreeItemPtr>		getItemsToInsert() const;

signals:
	void					invalidateFilter() const;

private:
	TreeItemPtr				m_rootItem;
	kdb::KDB				m_kdb;
	TreeItem				*getItem(const QModelIndex &index) const;
	TreeItemPtr				getItemPtr(const QModelIndex &index) const;
	QList<TreeItemPtr>		m_itemsToInsert;
};

Q_DECLARE_METATYPE(TreeModel)

#endif // TREEMODEL_HPP
