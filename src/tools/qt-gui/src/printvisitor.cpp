/**
 * @file
 *
 * @brief
 *
 * @copyright BSD License (see doc/LICENSE.md or http://www.libelektra.org)
 */

#include "printvisitor.hpp"

#include "confignode.hpp"
#include "treeviewmodel.hpp"

void PrintVisitor::visit (TreeItem &item)
{
	QStringList path = item.name().split ("/");
	QString name;

	foreach (QString s, path)
		name += " ";

	name += item.baseName();

	std::cout << name.toStdString () << std::endl;
}

//void PrintVisitor::visit (TreeViewModel * model)
//{
//	foreach (ConfigNodePtr node, model->model ())
//	{
//		node->accept (*this);
//	}
//}
