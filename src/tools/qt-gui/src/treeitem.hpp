#ifndef TREEITEM_HPP
#define TREEITEM_HPP

#include <QObject>
#include <kdb.hpp>
#include <kdbease.h>
#define QT_SHAREDPOINTER_TRACK_POINTERS
#include <QSharedPointer>
#include "metamodel.hpp"
#include "findvisitor.hpp"
#include "keysetvisitor.hpp"
#include "printvisitor.hpp"

class Visitor;

class TreeItem : public QObject
{
	Q_OBJECT

public:
	explicit TreeItem(const QString& baseName, const QString& name, const kdb::Key& key, QSharedPointer<TreeItem> parent);
	TreeItem(const TreeItem& other);
	TreeItem() {}
	~TreeItem();

	QString								baseName() const;
	QString								name() const;
	QString								toString() const;
	QString								getCurrentArrayNo () const;

	QVariant							value() const;

	kdb::Key							key() const;

	QSharedPointer<TreeItem>			parent() const;
	QSharedPointer<TreeItem>			child(int row) const;
	QSharedPointer<TreeItem>			getChildByName(QString &name) const;

	int									childCount() const;
	int									columnCount() const;
	int									row() const;

	void								setBaseName(const QString &baseName);
	void								setName(const QString &name);
	void								setValue(const QVariant &value);
	void								setMetaData(const QVariantMap &metaData);
	void								deleteKeyMetaData(const QString &name);
	void								setKey(const kdb::Key &key);
	void								setParent(QSharedPointer<TreeItem> parent);
	void								setIsDirty(bool value);
	void								appendChild(QSharedPointer<TreeItem> child);
	void								updateNode(const kdb::Key &key);
	void								clear();
	void								refreshArrayNumbers ();
	void								accept (Visitor & visitor);

	bool								hasChild(const QString &name) const;
	bool								isDirty() const;
	bool								childrenAreLeaves() const;
	bool								insertChild(int index, QSharedPointer<TreeItem> item);
	bool								insertChildren(int index, QList<QSharedPointer<TreeItem> > items);
	bool								removeChildren(int row, int count);
	bool 								siblingHasChildren() const;

	QList<QSharedPointer<TreeItem> >	children() const;

	MetaModel							*metaData() const;

private:
	QString								m_baseName;
	QString								m_name;
	QVariant							m_value;
	kdb::Key							m_key;

	QList<QSharedPointer<TreeItem> >	m_children;
	QSharedPointer<TreeItem>			m_parent;
	MetaModel*							m_metaData;
	bool								m_isDirty;

	void								populateMetaModel();
	void								setValue();
	void								setKeyName(const QString &name);
	int									getRowByName() const;
};

Q_DECLARE_METATYPE(TreeItem)

typedef QSharedPointer<TreeItem> TreeItemPtr;

#endif // TREEITEM_HPP
