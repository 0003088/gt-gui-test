#ifndef ONLYLEAVESPROXYMODEL_HPP
#define ONLYLEAVESPROXYMODEL_HPP

#include <QSortFilterProxyModel>

class OnlyLeavesProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	explicit OnlyLeavesProxyModel(QObject *parent = 0) {Q_UNUSED(parent)}

	bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
	void setSourceModel(QAbstractItemModel *sourceModel);

public slots:
	void invalidateFilter();
};

#endif // ONLYLEAVESPROXYMODEL_HPP
