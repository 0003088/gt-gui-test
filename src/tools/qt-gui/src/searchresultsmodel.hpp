#ifndef SEARCHRESULTSMODEL_HPP
#define SEARCHRESULTSMODEL_HPP

#include <QObject>
#include <QAbstractListModel>
#include <QDebug>
#include <QSharedPointer>
#include "metamodel.hpp"

class TreeItem;

class SearchResultsModel : public QAbstractListModel
{
	Q_OBJECT

public:
	enum SearchResultsModelRoles
	{
		NameRole = Qt::UserRole + 1
	};

	explicit SearchResultsModel(const QObject* parent = nullptr) {Q_UNUSED(parent)}
	SearchResultsModel(const MetaModel &other) {Q_UNUSED(other)}

	int						rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant				data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	Q_INVOKABLE QVariantMap	get(const int &idx) const;
	QHash<int, QByteArray>	roleNames() const;
	void					appendRow(QSharedPointer<TreeItem> item);

private:
	QList<QSharedPointer<TreeItem> >		m_model;
};

#endif // SEARCHRESULTSMODEL_HPP
