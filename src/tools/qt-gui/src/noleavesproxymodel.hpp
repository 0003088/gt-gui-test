#ifndef NOLEAVESPROXYMODEL_HPP
#define NOLEAVESPROXYMODEL_HPP

#include <QSortFilterProxyModel>

class NoLeavesProxyModel : public QSortFilterProxyModel
{
	Q_OBJECT

public:
	explicit NoLeavesProxyModel(QObject *parent = 0) {Q_UNUSED(parent)}

	bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
	void setSourceModel(QAbstractItemModel *sourceModel);

public slots:
	void invalidateFilter();
	void textFilterChanged(const QString &text);
};

#endif // NOLEAVESPROXYMODEL_HPP
