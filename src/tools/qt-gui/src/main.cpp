/**
 * @file
 *
 * @brief
 *
 * @copyright BSD License (see doc/LICENSE.md or http://www.libelektra.org)
 */

#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QMetaType>
#include <QQmlApplicationEngine>
#include <QSortFilterProxyModel>
#include <QTranslator>
#include <QtQml>
#include <QtTest/qtestcase.h>
#include <kdb.hpp>
#include <merging/mergingkdb.hpp>

#include "QQuickThemeIconProvider.hpp"

#include "confignode.hpp"
#include "datacontainer.hpp"
#include "guibackend.hpp"
#include "guisettings.hpp"
#include "treemodel.hpp"
#include "noleavesproxymodel.hpp"
#include "onlyleavesproxymodel.hpp"
#include "metamodel.hpp"
#include "undomanager.hpp"
#include "modeltest/modeltest.h"

int main (int argc, char * argv[])
{
	QApplication app (argc, argv);

	qRegisterMetaType<TreeModel> ("TreeModel");
	qRegisterMetaType<TreeItem> ("TreeItem");
	qRegisterMetaType<MetaModel>("MetaModel");
	qRegisterMetaType<UndoManager> ("UndoManager");
	qRegisterMetaType<GUIBackend> ("GUIBackend");
	qRegisterMetaType<GUISettings> ("GUISettings");
	qmlRegisterType<DataContainer> ("org.libelektra.qtgui", 1, 0, "DataContainer");

	QString locale = QLocale::system ().name ();

	QTranslator translator;
	translator.load (QString (":/qml/i18n/lang_") + locale + QString (".qm"));
	app.installTranslator (&translator);

	app.setOrganizationName ("ElektraInitiative");
	app.setOrganizationDomain ("libelektra.org");
	app.setApplicationName ("org.libelektra.elektra-qt-editor");

	QQmlApplicationEngine engine;
	QQmlContext * ctxt = engine.rootContext ();
	engine.addImageProvider (QLatin1String ("theme"), new QQuickThemeIconProvider);

	UndoManager manager;
	GUIBackend backend;
	GUISettings settings;
	kdb::tools::merging::MergingKDB kdb;
	TreeModel treeModel;
	NoLeavesProxyModel treeFilter;
	OnlyLeavesProxyModel tableFilter;

	treeFilter.setSourceModel(&treeModel);
	tableFilter.setSourceModel(&treeModel);

	engine.setObjectOwnership(&treeModel, QQmlApplicationEngine::CppOwnership);
	engine.setObjectOwnership(&treeFilter, QQmlApplicationEngine::CppOwnership);
	engine.setObjectOwnership(&tableFilter, QQmlApplicationEngine::CppOwnership);

	ctxt->setContextProperty ("undoManager", &manager);
	ctxt->setContextProperty ("treeModel", &treeModel);
	ctxt->setContextProperty("noLeavesProxyModel", &treeFilter);
	ctxt->setContextProperty("onlyLeavesProxyModel", &tableFilter);
	ctxt->setContextProperty ("guiBackend", &backend);
	ctxt->setContextProperty ("guiSettings", &settings);

	try
	{
		treeModel.populateModel ();
	}
	catch (std::exception const & e)
	{
		QMessageBox msgBox;
		msgBox.setText ("Could not start qt-gui. Failed while reading the whole configuration.");
		msgBox.setInformativeText (e.what ());
		msgBox.setIcon (QMessageBox::Critical);
		msgBox.exec ();
		return 1;
	}

	new ModelTest(&treeModel);
	new ModelTest(&treeFilter);
	new ModelTest(&tableFilter);

	engine.load (QUrl (QStringLiteral ("qrc:/qml/main.qml")));

	return app.exec ();
}
