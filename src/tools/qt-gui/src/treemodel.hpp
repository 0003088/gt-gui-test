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

	explicit TreeModel (QObject * parentModel = nullptr);

	explicit TreeModel (kdb::tools::merging::MergingKDB * kdb, QObject * parentModel = nullptr);

	TreeModel (TreeModel const & other);

	QModelIndex				index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex				parent(const QModelIndex &child) const;
	QModelIndex				pathToIndex(const Path &path);

	int						rowCount(const QModelIndex &parent = QModelIndex()) const;
	int						columnCount(const QModelIndex &parent = QModelIndex()) const;

	QVariant				data(const QModelIndex &index, int role) const;
	Q_INVOKABLE QVariant	find (const QString & term);

	bool					setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
	//	bool					insertRow(int row, const QModelIndex &parent, TreeItemPtr item, bool addParent = true);
	bool					insertRows(int row, int count, const QModelIndex &parent);
	bool					removeRows(int row, int count, const QModelIndex &parent);
	//bool					hasChildren(const QModelIndex &parent) const;

	void					sink(TreeItemPtr item, QStringList keys, const kdb::Key &key);
	void					populateModel(const kdb::KeySet &keySet);
	void					populateModel();
	void					createNewNodes(kdb::KeySet keySet);
	void					setItemsToInsert(const QList<TreeItemPtr> &itemsToInsert);
	Q_INVOKABLE void		synchronize ();
	Q_INVOKABLE void		exportConfiguration (QModelIndex idx, QString format, QString file);
	Q_INVOKABLE void		importConfiguration (const QString & name, QString & file, QString & format, const QVariantList & mergeStrategies);
	Q_INVOKABLE void		unMountBackend (QString backendName);

	QStringList				getSplittedKeyname(const kdb::Key &key);
	Q_INVOKABLE QStringList mountedBackends ();

	Qt::ItemFlags			flags(const QModelIndex &index) const;

	QHash<int, QByteArray>	roleNames() const;

	Path					pathFromIndex(const QModelIndex &index);

	QList<TreeItemPtr>		getItemsToInsert() const;

	kdb::KeySet				collectCurrentKeySet ();

signals:
	void					invalidateFilter() const;
	void					showMessage (QString title, QString text, QString detailedText) const;

private:
	TreeItemPtr							m_rootItem;
	TreeItemPtr							getItemPtr(const QModelIndex &index) const;
	TreeItem						*	getItem(const QModelIndex &index) const;

	kdb::Key							m_root;
	//kdb::KDB				m_kdb;
	kdb::tools::merging::MergingKDB *	m_kdb;
	kdb::tools::merging::MergeConflictStrategy * getMergeStrategy (const QString & mergeStrategy);

	QList<TreeItemPtr>					m_itemsToInsert;

	void								connectDBus ();

	QStringList							getConflicts(const kdb::KeySet &conflictSet);

public slots:
	void configChanged (QString msg);

};

Q_DECLARE_METATYPE(TreeModel)

#endif // TREEMODEL_HPP
