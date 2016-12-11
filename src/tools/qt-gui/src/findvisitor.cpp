/**
 * @file
 *
 * @brief
 *
 * @copyright BSD License (see doc/LICENSE.md or http://www.libelektra.org)
 */

#include "findvisitor.hpp"
#include "treemodel.hpp"

FindVisitor::FindVisitor (TreeModel *searchResults, QString term) : m_searchResults (searchResults), m_term (std::move (term))
{
}

void FindVisitor::visit (TreeItem &item)
{
//	bool termFound = false;

//	if (item.name().contains (m_term) || item.value().toString ().contains (m_term))
//	{
//		termFound = true;
//	}

//	if (item.metaData() && !termFound)
//	{
//		foreach (MetaItemPtr metaNode, item.)
//		{
//			if (metaNode->getName ().contains (m_term) || metaNode->getValue ().toString ().contains (m_term))
//			{
//				termFound = true;
//				break;
//			}
//		}
//	}

//	if (termFound)
//		// let the other model delete this node
//		m_searchResults->insertRow (m_searchResults->rowCount (), ConfigNodePtr (&item, &ConfigNode::dontDelete), false);
}

//void FindVisitor::visit (TreeViewModel * model)
//{
//	foreach (ConfigNodePtr node, model->model ())
//	{
//		node->accept (*this);
//	}
//}
