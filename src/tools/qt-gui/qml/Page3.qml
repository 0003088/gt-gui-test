import QtQuick 2.2

WizardTemplate {
	id: page3

	wizardText.text: qsTr("Please enter a path to a file in the filesystem. This file is used by all plugins of this backend. " +
						  "File names might be absolute or relative and have a different location for user and system pathes. " +
						  "The actual path will be located dynamically by the resolver plugin.\n\n" +
						  "Note that you cannot undo creating the backend. If you want to remove a backend use the unmount option.")

	label.text: qsTr("Path: ")

	buttonRow.nextButton.visible: false
	buttonRow.finishButton.visible: true
	buttonRow.finishButton.action.enabled: textField.text !== ""
	buttonRow.finishButton.action.onTriggered: {
		guiBackend.addPath(textField.text)

		if(!error){
			guiBackend.serialise(externTreeModel)

			if(!error){
				wizardLoader.close()
				guiBackend.deleteBackend()
				loader.source = "Page1.qml"
				externTreeModel.refresh()
			}

		}

	}
	buttonVisible: true
	fileDialog.onAccepted: textField.text = fileDialog.fileUrl.toString().replace("file://", "")
}


