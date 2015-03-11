#ifndef GUIBACKEND_HPP
#define GUIBACKEND_HPP

#include <QObject>
#include <QStringList>
#include <backend.hpp>
#include "treeviewmodel.hpp"

/**
 * @brief The GUIBackend class
 */

class GUIBackend : public QObject
{
	Q_OBJECT

public:
	/**
	 * @brief GUIBackend
	 * @param parentBackend
	 */
	explicit GUIBackend(QObject *parentBackend = 0);

	/**
	 * @brief GUIBackend
	 * @param other
	 */
	GUIBackend(const GUIBackend &other) : QObject() {Q_UNUSED(other)}

	/**
	 * @brief Creates a new backend on a mountpoint.
	 *
	 * @param mountpoint The mountpoint of the new backend.
	 */
	Q_INVOKABLE void			createBackend(const QString &mountpoint);

	/**
	 * @brief Add path to a backend fallback file.
	 *
	 * @param path The path to a backend fallback file.
	 */
	Q_INVOKABLE void			addPath(const QString &path);

	/**
	 * @brief Add a plugin to a backend.
	 *
	 * @param name The name of the plugin.
	 */
	Q_INVOKABLE void			addPlugin(QString name);

	/**
	 * @brief Provides information about a plugin.
	 *
	 * @param pluginName The plugin.
	 * @return The information about the plugin.
	 */
	Q_INVOKABLE QString			pluginInfo(QString pluginName) const;

	/**
	 * @brief Returns a list of currently used mountpoints.
	 *
	 * @return A list of currently used mountpoints.
	 */
	Q_INVOKABLE QString			mountPoints() const;

	/**
	 * @brief Returns a list of all currently available plugins.
	 *
	 * @param includeStorage Determines if storage plugins should be included in the list.
	 * @param includeResolver Determines if resolver plugins should be included in the list.
	 *
	 * @return A list of all currently available plugins.
	 */
	Q_INVOKABLE QStringList 	availablePlugins(bool includeStorage, bool includeResolver) const;

	/**
	 * @brief Returns a list of all currently available namefilters.
	 *
	 * @return A list of all currently available namefilters.
	 */
	Q_INVOKABLE QStringList 	nameFilters();

	/**
	 * @brief Writes the current backend permanently to storage.
	 */
	Q_INVOKABLE void			serialise(TreeViewModel *model);

	/**
	 * @brief Returns if the current backend is validated.
	 *
	 * @return If the current backend is validated.
	 */
	Q_INVOKABLE bool			validated();

	/**
	 * @brief pluginConfigModel
	 * @return
	 */
	Q_INVOKABLE TreeViewModel*	pluginConfigModel() const;

private:
	QSharedPointer<kdb::tools::Backend>	m_backend;
	kdb::KeySet							m_mountConf;
	kdb::KDB							m_kdb;
	QString								m_name;
	TreeViewModel*						m_pluginConfigModel;

	/**
	 * @brief resetModel
	 */
	void								resetModel();

signals:
	/**
	 * @brief showMessage
	 * @param title
	 * @param text
	 * @param detailedText
	 */
	void showMessage(QString title, QString text, QString detailedText) const;
};

#endif // GUIBACKEND_HPP
