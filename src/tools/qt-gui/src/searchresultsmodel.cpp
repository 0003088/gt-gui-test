#include "searchresultsmodel.hpp"
#include "treeitem.hpp"

int SearchResultsModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)
	return m_model.count();
}

QVariant SearchResultsModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
	{
		return QVariant();
	}

	TreeItemPtr item = m_model.at(index.row());

	switch (role)
	{
	case NameRole:
		return QVariant::fromValue(item->name());

	default:
		return QVariant();
	}
}

QVariantMap SearchResultsModel::get(const int& idx) const
{
	QVariantMap map;

	foreach (int k, roleNames().keys())
	{
		map[roleNames().value(k)] = data(index(idx, 0), k);
	}

	return map;
}

void SearchResultsModel::appendRow(TreeItemPtr item)
{
	m_model.append(item);
}

QHash<int, QByteArray> SearchResultsModel::roleNames() const
{
	QHash<int, QByteArray> roles;

	roles[NameRole] = "name";

	return roles;
}
