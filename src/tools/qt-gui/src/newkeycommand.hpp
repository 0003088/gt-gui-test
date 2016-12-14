/**
 * @file
 *
 * @brief
 *
 * @copyright BSD License (see doc/LICENSE.md or http://www.libelektra.org)
 */

#ifndef NEWKEYCOMMAND_HPP
#define NEWKEYCOMMAND_HPP

#include "datacontainer.hpp"
#include "treemodel.hpp"
#include <QUndoCommand>

/**
 * @brief The NewKeyCommand class. Will create a new ConfigNode.
 */

class NewKeyCommand : public QUndoCommand
{

public:
	/**
	 * @brief The default constructor.
	 *
	 * @param model The TreeViewModel that will contain the new ConfigNode.
	 * @param index The index of the new ConfigNode.
	 * @param isBelow Is set when a ConfigNode should be below another ConfigNode. Will trigger an update of the treeview.
	 * @param parent An optional parent.
	 */
	explicit NewKeyCommand (TreeModel * model, const QModelIndex &parent, DataContainer * data, bool isBelow, QUndoCommand * parentCommand = nullptr);

	virtual void undo () override;
	virtual void redo () override;

private:
	TreeModel	*m_model;
	TreeItemPtr	m_parentNode;
	TreeItemPtr	m_newNode;
//	QString		m_name;
//	QString		m_value;
//	QVariantMap	m_metaData;
	Path		m_path;

	/**
	 * @brief cutListAtIndex Helper method that cuts a QStringList at an index and returns the rest of the list.
	 * @param list The QStringList to cut.
	 * @param index The index where to cut the QStringList.
	 * @return The cut QStringList.
	 */
	QStringList cutListAtIndex (QStringList & list, int index);
};

#endif // NEWKEYCOMMAND_HPP
