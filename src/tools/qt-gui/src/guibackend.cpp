#include "guibackend.hpp"

#include <markdowndocument.h>
#include <discountmarkdownconverter.h>
#include <backends.hpp>
#include <vector>
#include <string>
#include <QDebug>
#include <QRegExp>

using namespace std;
using namespace kdb;
using namespace kdb::tools;

GUIBackend::GUIBackend(QObject *parentBackend) :
	QObject(parentBackend)
{
	m_pluginConfigModel = new TreeViewModel;
	resetModel();
}

GUIBackend::GUIBackend(const GUIBackend &other)
	: QObject()
{
	Q_UNUSED(other);
}

void GUIBackend::createBackend(const QString &mountpoint)
{
	m_backend = new Backend();

	Key parentKey(Backends::mountpointsPath, KEY_END);

	try
	{
		m_kdb.get(m_mountConf, parentKey);
	}
	catch(KDBException const& ex)
	{
		emit showMessage(tr("Error"), tr("Could not read from configuration."), QString(ex.what()));
	}

	try
	{
		m_backend->setMountpoint(Key(mountpoint.toStdString(), KEY_CASCADING_NAME, KEY_END), m_mountConf);
	}
	catch(MountpointInvalidException const& ex)
	{
		emit showMessage(tr("Error"), tr("The provided mount point is invalid."), ex.what());
	}
	catch(MountpointAlreadyInUseException const& ex)
	{
		emit showMessage(tr("Error"), tr("The provided mount point is one of the already used cascading names."), ex.what());
	}
}

void GUIBackend::addPath(const QString &path)
{
	try
	{
		m_backend->useConfigFile(path.toStdString());
	}
	catch(FileNotValidException const& ex)
	{
		emit showMessage(tr("Error"), tr("The file you have entered is not valid."), ex.what());
	}
	catch(MissingSymbol const& ex)
	{
		emit showMessage(tr("Error"), tr("Could not add file."), ex.what());
	}
}

void GUIBackend::addPlugin(QString name)
{
	name.chop(name.length() - name.indexOf("[") + 1);

	try
	{
		m_backend->addPlugin(name.toStdString(), m_pluginConfigModel->collectCurrentKeySet().dup());
	}
	catch(PluginCheckException const &ex)
	{
		emit showMessage(tr("Error"), tr("Could not add plugin \"%1\".").arg(name), ex.what());
	}

	resetModel();
}

void GUIBackend::serialise(TreeViewModel *model)
{
	Key rootKey (Backends::mountpointsPath, KEY_END);

	try
	{
		m_backend->serialise(rootKey, m_mountConf);
	}
	catch(ToolException &ex)
	{
		emit showMessage(tr("Error"), tr("Could not serialise backend."), ex.what());
	}

	m_mountConf.rewind();

	while (m_mountConf.next())
	{
		Key k = m_mountConf.current().dup();
		QString currentKey = QString::fromStdString(k.getName());
		QStringList keys = currentKey.split("/");
		QString root = keys.takeFirst();

		if (root == "system")
		{
			model->sink(model->model().at(0), keys, "system", k);
		}
		else if (root == "user")
		{
			model->sink(model->model().at(1), keys, "user", k);
		}
	}

	try
	{
		m_kdb.get(m_mountConf, rootKey);
		m_kdb.set(m_mountConf, rootKey);
	}
	catch (kdb::KDBException const& ex)
	{
		emit showMessage(tr("Error"), tr("Could not write backend to configuration."), ex.what());
	}
}

bool GUIBackend::validated()
{
	return m_backend->validated();
}

void GUIBackend::deleteBackend()
{
	delete m_backend;
}

TreeViewModel *GUIBackend::pluginConfigModel() const
{
	return m_pluginConfigModel;
}

void GUIBackend::resetModel()
{
	KeySet emptySet;
	m_pluginConfigModel->populateModel(emptySet);
}

QString GUIBackend::mountPoints() const
{
	Key parentKey(Backends::mountpointsPath, KEY_END);
	KeySet mountConf;
	KDB kdb (parentKey);

	try
	{
		kdb.get(mountConf, parentKey);
	}
	catch(KDBException const& ex)
	{
		emit showMessage(tr("Error"), tr("Could not read from configuration."), QString(ex.what()));
		return "";
	}

	QStringList mPoints;
	mPoints.append("system/elektra");

	mountConf.rewind();

	Key cur;

	while ((cur = mountConf.next()))
	{
		if (cur.getBaseName() == "mountpoint")
		{
			if (cur.getString().at(0) == '/')
			{
				mPoints.append(QString::fromStdString("user" + cur.getString()));
				mPoints.append(QString::fromStdString("system" + cur.getString()));
			}
			else
			{
				mPoints.append(QString::fromStdString(cur.getString()));
			}

		}

	}
	return mPoints.join(", ");
}

QString GUIBackend::pluginInfo(QString pluginName) const
{
	Modules modules;
	KeySet info;
	QString infoString;

	pluginName.chop(pluginName.length() - pluginName.indexOf("[") + 1);

	PluginPtr plugin = modules.load(pluginName.toStdString());

	info = plugin->getInfo();

	Key root;
	root.setName(std::string("system/elektra/modules/") + plugin->name() + "/infos");
	Key k = info.lookup(root);

	if (k)
	{
		while ((k = info.next()) && k.getDirName() == root.getName())
		{
			infoString.append(QString::fromStdString(k.getBaseName()) + ": " + QString::fromStdString(k.getString()) + "\n\n");
		}
	}
	else
		infoString.append(tr("No information found."));

	DiscountMarkdownConverter dmc;
	infoString = dmc.renderAsHtml(dmc.createDocument(infoString, DiscountMarkdownConverter::NoImagesOption));

	return infoString;
}

QStringList GUIBackend::availablePlugins(bool includeStorage, bool includeResolver) const
{
	QStringList availPlugins;
	Modules modules;
	PluginPtr ptr;
	QString type;

	vector<string> pluginVector = listAllAvailablePlugins();

	foreach(string s, pluginVector){
		try
		{
			ptr = modules.load(s);
		}
		catch(NoPlugin &ex)
		{
			break;
		}

		ptr->loadInfo();
		type = QString::fromStdString(ptr->lookupInfo("provides"));

		if(!((!includeStorage && type == "storage") || (!includeResolver && type == "resolver"))){
			availPlugins.append(QString::fromStdString(s) + QString::fromStdString(" [%1]").arg(type));
		}
	}

	availPlugins.sort();

	return availPlugins;
}

QStringList GUIBackend::nameFilters()
{
	QStringList nFilters;
	QStringList plugins = availablePlugins(true, false);

	plugins = plugins.filter("[storage]");
	plugins.replaceInStrings(QRegExp("\\s\\[\\w*\\]"), "");

	foreach(QString plugin, plugins)
	{
		QString pattern;

		if(plugin == "ini" || plugin == "simpleini" || plugin == "ni")
			pattern = "*.ini";
		else if(plugin == "xmltool")
			pattern = "*.xml";
		else if(plugin == "tcl")
			pattern = "*.tcl";
		else if(plugin == "yajl")
			pattern = "*.json";
		else if(plugin == "dump")
			pattern = "*.ecf";
		else
			pattern = "*";

			nFilters.append(QString("%1 (%2)").arg(plugin, pattern));
	}

	nFilters.sort();

	return nFilters;
}
