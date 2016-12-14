/**
 * @file
 *
 * @brief
 *
 * @copyright BSD License (see doc/LICENSE.md or http://www.libelektra.org)
 */

#ifndef DELETEKEYCOMMAND_HPP
#define DELETEKEYCOMMAND_HPP

#include "treemodel.hpp"
#include <QUndoCommand>

/**
 * @brief The DeleteKeyCommand class. Remembers a node for redo/undo.
 */
class DeleteKeyCommand : public QUndoCommand
{

public:
	/**
	 * @brief The command to delete a ConfigNode.
	 *
	 * @param type Declares if the ConfigNode is a single key or a branch.
	 * @param model The model that holds the ConfigNode that is deleted.
	 * @param index The index of the ConfigNode that is deleted.
	 * @param parent An optional parent command.
	 */
	explicit DeleteKeyCommand (TreeModel* model, const QModelIndex &index, QUndoCommand* parent = nullptr);

	virtual void undo () override;
	virtual void redo () override;

private:
	TreeModel*				m_model;
	TreeItemPtr				m_item;
	TreeItemPtr				m_root;
	int 					m_row;
	Path					m_path;
	bool					m_isRoot;
};

#endif // DELETEKEYCOMMAND_HPP
