#ifndef DELETEKEYCOMMAND_HPP
#define DELETEKEYCOMMAND_HPP

#include <QUndoCommand>
#include "treeviewmodel.hpp"

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
	explicit DeleteKeyCommand(const QString& type, TreeViewModel* model, int index, QUndoCommand* parent = 0);

	virtual void undo();
	virtual void redo();

private:
	TreeViewModel*  m_model;
	ConfigNodePtr   m_node;
	int             m_index;
	bool			m_isRoot;
	ConfigNodePtr	m_root;
};

#endif // DELETEKEYCOMMAND_HPP
