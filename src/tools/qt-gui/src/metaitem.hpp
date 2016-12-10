#ifndef METAITEM_HPP
#define METAITEM_HPP

#include <QObject>
#include <QVariant>
#include <QString>
#define QT_SHAREDPOINTER_TRACK_POINTERS
#include <QSharedPointer>

class MetaItem : public QObject
{
	Q_OBJECT

public:
	explicit MetaItem(const QString& metaName, const QVariant& metaValue, const QObject* parent = nullptr);
	MetaItem (const MetaItem & other);
	MetaItem() {}
	~MetaItem() {}

	QString		metaName() const;
	QVariant	metaValue() const;

	void		setMetaName(const QString &metaName);
	void		setMetaValue(const QVariant &metaValue);

private:
	QString		m_metaName;
	QVariant	m_metaValue;
};

typedef QSharedPointer<MetaItem> MetaItemPtr;

#endif // METAITEM_HPP
