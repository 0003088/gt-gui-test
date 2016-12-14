#include "noleavesproxymodel.hpp"
#include "treemodel.hpp"

void NoLeavesProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
	QSortFilterProxyModel::setSourceModel(sourceModel);

	connect(this->sourceModel(), SIGNAL(invalidateFilter()), this, SLOT(invalidateFilter()));
}

void NoLeavesProxyModel::invalidateFilter()
{
	QSortFilterProxyModel::invalidateFilter();
}

void NoLeavesProxyModel::textFilterChanged(const QString &text)
{
	QRegExp regex(text,Qt::CaseInsensitive);

	setFilterRegExp(regex);
	invalidateFilter();
}

bool NoLeavesProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
	if(!source_parent.isValid())
		return true;

	QModelIndex index = sourceModel()->index(source_row, filterKeyColumn(), source_parent);

	if(index.isValid())
	{
		bool accept = !(sourceModel()->data(index, TreeModel::ChildCountRole) == 0
		&& !sourceModel()->data(index, TreeModel::SiblingHasChildrenRole).toBool()
		&& sourceModel()->data(index, TreeModel::HierarchyRole) > 1)
		&& (sourceModel()->data(index, TreeModel::BaseNameRole).toString().contains(filterRegExp())
		||	sourceModel()->data(index, TreeModel::ValueRole).toString().contains(filterRegExp()));

		if (sourceModel()->hasChildren(index))
		{
			for (int i = 0; i < sourceModel()->rowCount(index); i++)
			{
				accept = accept || filterAcceptsRow(i,index);
			}

		}

		return accept;
	}
	return true;
}
