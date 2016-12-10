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

bool OnlyLeavesProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
	if(!source_parent.isValid())
		return false;

	QModelIndex index = sourceModel()->index(source_row, filterKeyColumn(), source_parent);

	if(index.isValid())
	{
		if((sourceModel()->data(index, TreeModel::ChildrenAreLeavesRole).toBool()))
			return true;
	}

	return false;
}
