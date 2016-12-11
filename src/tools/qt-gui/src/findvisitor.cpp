/**
 * @file
 *
 * @brief
 *
 * @copyright BSD License (see doc/LICENSE.md or http://www.libelektra.org)
 */

#include "findvisitor.hpp"
#include "treeitem.hpp"

FindVisitor::FindVisitor (SearchResultsModel *searchResults, QString term) : m_searchResults (searchResults), m_term (std::move (term))
{
}

void FindVisitor::visit (TreeItem &item)
{
	bool termFound = false;

	if (item.name().contains (m_term) || item.value().toString ().contains (m_term))
	{
		termFound = true;
	}

	if (item.metaData() && !termFound)
	{
		foreach (MetaItemPtr metaNode, item.metaData()->children())
		{
			if (metaNode->metaName().contains (m_term) || metaNode->metaValue().toString ().contains (m_term))
			{
				termFound = true;
				break;
			}
		}
	}

	if (termFound)
		// let the other model delete this node
		m_searchResults->appendRow(TreeItemPtr(&item));
}
