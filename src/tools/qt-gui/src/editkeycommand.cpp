/**
 * @file
 *
 * @brief
 *
 * @copyright BSD License (see doc/LICENSE.md or http://www.libelektra.org)
 */

#include "editkeycommand.hpp"

EditKeyCommand::EditKeyCommand (TreeModel* model, const QModelIndex &index, DataContainer* data, QUndoCommand* parent)
:  QUndoCommand(parent)
, m_model(model)
, m_index(m_model->pathFromIndex(index))
, m_oldName(data->oldName())
, m_oldValue(data->oldValue())
, m_oldMetaData(data->oldMetadata())
, m_newName(data->newName())
, m_newValue(data->newValue())
, m_newMetaData(data->newMetadata())
{
	setText ("edit");
}

void EditKeyCommand::undo ()
{
	QModelIndex index = m_model->pathToIndex(m_index);

	if (index.isValid())
	{
		m_model->setData(index, m_oldName, TreeModel::BaseNameRole);
		m_model->setData(index, m_oldValue, TreeModel::ValueRole);
		m_model->setData(index, m_oldMetaData, TreeModel::MetaDataRole);
	}
}

void EditKeyCommand::redo ()
{
	QModelIndex index = m_model->pathToIndex(m_index);

	if (index.isValid())
	{
		m_model->setData(index, m_newName, TreeModel::BaseNameRole);
		m_model->setData(index, m_newValue, TreeModel::ValueRole);
		m_model->setData(index, m_newMetaData, TreeModel::MetaDataRole);
	}
}
