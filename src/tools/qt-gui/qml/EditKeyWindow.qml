import QtQuick 2.5

KeyWindow {

	title: qsTr("Edit Key")

	keyName: (selectedNode === null || treeModel.data(selectedNode,257) === undefined) ? "" : treeModel.data(selectedNode,257)
	keyValue: (selectedNode === null || treeModel.data(selectedNode,259) === undefined) ? "" : treeModel.data(selectedNode,259)

	function populateMetaArea() {
		if (treeModel.data(selectedNode,266)){
			for (var i = 0; i < treeModel.data(selectedNode,266).rowCount(); i++){
				qmlMetaKeyModel.append({"metaName" : treeModel.data(selectedNode,266).get(i).metaName, "metaValue" : treeModel.data(selectedNode,266).get(i).metaValue})
			}
		}
	}

	function editAccepted() {

		var metaData = {}
		var index = selectedNode

//		if (accessFromSearchResults)
//			index = selectedNode.parentModel.getIndexByName(selectedNode.name)
//		else
//			index = selectedNode.index

		//collect metadata in a map
		for (var i = 0; i < qmlMetaKeyModel.count; i++){
			metaData[qmlMetaKeyModel.get(i).metaName] = qmlMetaKeyModel.get(i).metaValue
		}

//		//create undo command
		if (isEdited){
			container.clearData()

			container.setOldName(keyName.toString())
			container.setOldValue(keyValue.toString())
			container.setOldMetadata(selectedNode.metaValue)

			container.setNewName(nameTextField.text)
			container.setNewValue(valueTextField.text)
			container.setNewMetadata(metaData)

			undoManager.createEditKeyCommand(treeModel, index, container)
		}

		if (!error){
			qmlMetaKeyModel.clear()
			selectedNode = null

//			if (accessFromSearchResults){
//				searchResultsListView.model.refresh()
//				searchResultsSelectedItem = searchResultsListView.model.get(searchResultsListView.currentIndex)
//			}

			visible = false
			accessFromSearchResults = false
			nameTextField.readOnly = false
			nameTextField.textColor = activePalette.text

			qmlMetaKeyModel.clear()

////			if (keyAreaView.model !== null && !accessFromSearchResults){
////				keyAreaSelectedItem = keyAreaView.model.get(keyAreaView.currentRow)
////			}
		}
	}
}
