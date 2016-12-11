/**
 * @file
 *
 * @brief
 *
 * @copyright BSD License (see doc/LICENSE.md or http://www.libelektra.org)
 */

#include "printvisitor.hpp"

#include "treeitem.hpp"
#include "treemodel.hpp"

void PrintVisitor::visit (TreeItem &item)
{
	QStringList path = item.name().split ("/");
	QString name;

	foreach (QString s, path)
		name += " ";

	name += item.baseName();

	std::cout << name.toStdString () << std::endl;
}
