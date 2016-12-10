#ifndef METAMODEL_HPP
#define METAMODEL_HPP

#include <QObject>
#include <QAbstractListModel>
#include <kdb.hpp>
#include "metaitem.hpp"

class MetaModel : public QAbstractListModel
{
	Q_OBJECT

public:
	enum MetaModelRoles
	{
		MetaNameRole = Qt::UserRole + 1,
		MetaValueRole
	};

	explicit MetaModel(kdb::Key key, const QObject* parent = 0);
	MetaModel(const QObject* parent = 0) {Q_UNUSED(parent)}
	MetaModel(const MetaModel &other) {Q_UNUSED(other)}

	int												rowCount(const QModelIndex &parent = QModelIndex()) const;

	QVariant										data(const QModelIndex &index, int role = Qt::DisplayRole) const;

	void											clear();
	void											insertRow(int row, MetaItem *item);
	void											setMetaData(const QVariantMap &metaData);

	QHash<int, QByteArray>							roleNames() const;

	Q_INVOKABLE	QVariantMap							get(const int &idx) const;

	QList<MetaItem*>								children() const;


private:
	kdb::Key										m_key;
	QList<MetaItem*>								m_model;
	void											deleteKeyMetaData(const QString &name);
};

Q_DECLARE_METATYPE(MetaModel)

#endif // METAMODEL_HPP
