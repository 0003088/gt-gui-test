#include "onlyleavesproxymodel.hpp"
#include "treemodel.hpp"

void OnlyLeavesProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
	QSortFilterProxyModel::setSourceModel(sourceModel);

	connect(this->sourceModel(), SIGNAL(invalidateFilter()), this, SLOT(invalidateFilter()));
}

void OnlyLeavesProxyModel::invalidateFilter()
{
	QSortFilterProxyModel::invalidateFilter();
}

void OnlyLeavesProxyModel::textFilterChanged(const QString &text)
{
	QRegExp regex(text,Qt::CaseInsensitive);

	setFilterRegExp(regex);
}

bool OnlyLeavesProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
	if(!source_parent.isValid())
		return false;

	QModelIndex index = sourceModel()->index(source_row, filterKeyColumn(), source_parent);

	if(index.isValid())
	{
		return sourceModel()->data(index, TreeModel::ChildrenAreLeavesRole).toBool()
		&& sourceModel()->data(index, TreeModel::BaseNameRole).toString().contains(filterRegExp());
	}
}
