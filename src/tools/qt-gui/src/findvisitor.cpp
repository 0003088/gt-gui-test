#include "findvisitor.hpp"
#include "treeviewmodel.hpp"

FindVisitor::FindVisitor(TreeViewModel *searchResults, const QString &term) :
	m_searchResults(searchResults),
	m_term(term)

{
}

void FindVisitor::visit(ConfigNode &node)
{
	bool termFound = false;

	if (node.getName().contains(m_term) || node.getValue().toString().contains(m_term))
	{
		termFound = true;
	}

	if(node.getMetaKeys() && !termFound)
	{
		foreach (ConfigNodePtr metaNode, node.getMetaKeys()->model())
		{
			if(metaNode->getName().contains(m_term) || metaNode->getValue().toString().contains(m_term))
			{
				termFound = true;
				break;
			}

		}

	}

	if(termFound)
		//let the other model delete this node
		m_searchResults->model().append(ConfigNodePtr(&node, &ConfigNode::deleter));
}

void FindVisitor::visit(TreeViewModel *model)
{
	foreach (ConfigNodePtr node, model->model())
	{
		node->accept(*this);
	}
}
