/**
 * @file
 *
 * @brief
 *
 * @copyright BSD License (see doc/LICENSE.md or http://www.libelektra.org)
 */

#include "newkeycommand.hpp"
#include <kdb.hpp>

NewKeyCommand::NewKeyCommand (TreeModel *model, const QModelIndex &parent, DataContainer * data, bool isBelow, QUndoCommand * parentCommand)
: QUndoCommand (parentCommand)
, m_model(model)
, m_parentNode (qvariant_cast<TreeItemPtr>(model->data(parent, TreeModel::ItemRole)))
, m_newNode (nullptr)
, m_path(model->pathFromIndex(parent))
{
//	TreeViewModel * parentModel = m_parentNode->getChildren ();
	Q_ASSERT(m_model == parent.model());

	kdb::Key newKey = m_model->createNewKey (m_parentNode->name() + "/" + data->newName (), data->newValue (), data->newMetadata ());

	QStringList newNameSplit = m_model->getSplittedKeyname (newKey);
	kdb::Key parentKey = m_parentNode->key();

	if (!parentKey) parentKey = kdb::Key (m_parentNode->name ().toStdString (), KEY_END);

	QStringList parentNameSplit = m_model->getSplittedKeyname (parentKey);

	// check if the new key is directly below the parent
	QSet<QString> diff = newNameSplit.toSet ().subtract (parentNameSplit.toSet ());

	if (diff.count () > 1 || isBelow)
		setText ("newBranch");
	else
		setText ("newKey");

	QString name = cutListAtIndex (newNameSplit, parentNameSplit.count ()).first ();

	m_model->sink(m_parentNode, newNameSplit, newKey.dup ());

	m_newNode = m_parentNode->getChildByName (name);

	QModelIndexList newIndex = m_model->match(parent, TreeModel::NameRole,
	QVariant::fromValue(m_parentNode->name() + "/" + name),-1,Qt::MatchExactly | Qt::MatchRecursive);

	Q_ASSERT(newIndex.count() == 1);
	Q_ASSERT(newIndex.at(0).isValid());

	m_row = newIndex.at(0).row();

	model->removeRow(m_row,newIndex.at(0).parent());//TODO

//	parentModel->removeRow (m_parentNode->getChildIndexByName (m_name));
}

void NewKeyCommand::undo ()
{
	QModelIndex index = m_model->pathToIndex(m_path);

	if (index.isValid())
	{
		// remove new node
		m_model->removeRow (m_row,index);
	}

}

void NewKeyCommand::redo ()
{
	QModelIndex index = m_model->pathToIndex(m_path);

	if (index.isValid())
	{
		QList<TreeItemPtr> items;
		Q_ASSERT(m_newNode);
		items.append(m_newNode);
		m_model->setItemsToInsert(items);

		m_model->insertRows(m_row, items.count(), index);
	//		//	m_model->refreshArrayNumbers();
	//		//	m_model->refresh();
	}
}

QStringList NewKeyCommand::cutListAtIndex (QStringList & list, int index)
{
	for (int i = 0; i < index; i++)
		list.removeFirst ();

	return list;
}
