import QtQuick 2.6
import QtQuick.Controls 1.5
import QtQml.Models 2.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import QtQuick.Window 2.0
import Qt.labs.folderlistmodel 2.1
import "MainFunctions.js" as MFunctions
import "HelperFunctions.js" as Helper
import "ErrorDialogCreator.js" as ErrorDialog
import org.libelektra.qtgui 1.0

ApplicationWindow {
	id: mainWindow

	visible: true
	width: Screen.desktopAvailableWidth
	height: Screen.desktopAvailableHeight

	title: "Elektra Qt Editor (%1)".arg(mode)

	onClosing: {
		//		if (!undoManager.isClean()){
		//			close.accepted = false
		//			exitDialog.open()
		//		}
		//		else
		Qt.quit()
	}

	//**Properties*********************************************************************************************//

	property int    keyAreaHeight: Math.ceil(mainRow.height*0.7 - defaultSpacing)
	property int    deltaKeyAreaHeight: Math.ceil(mainRow.height*0.5 - defaultSpacing)
	property int    keyAreaWidth: Math.ceil(mainRow.width*0.7 - defaultSpacing)
	property int    metaAreaHeight: Math.floor(mainRow.height*0.3)
	property int    deltaMetaAreaHeight: Math.floor(mainRow.height*0.25 - defaultSpacing)
	property int    searchResultsAreaHeight: Math.ceil(mainRow.height*0.25)
	property var    keyAreaSelectedItem: null
	property var    searchResultsSelectedItem: null
	property bool   isPasted: false
	property bool	error: false
	property bool	helpMode: false

	property string version: "0.0.13 (beta)"
	property string mode: guiActions.toggleViewerAction.checked ? "Viewer Mode" : "Edit Mode"

	//Spacing & Margins recommended by KDE HIG
	property int    defaultMargins: 8
	property int    defaultSpacing: defaultMargins*0.5

	DataContainer {
		id:container
	}

	//**Signals & Slots****************************************************************************************//

	//Set up slots to catch signals from objects
	Connections {
		target: treeModel

		onShowMessage: {
			ErrorDialog.showMessage(title, text, detailedText)
		}
		//		onUpdateIndicator: {
		//			treeView.updateIndicator()
		//		}
		onInvalidateFilter: {
			treeView.updateIndicator()
			treeViewSelection.reset()
			keyViewSelection.reset()
		}
	}

	Connections {
		target: treeViewSelection

		onSelectionChanged: {
			path.selectionChanged()
		}
	}

	Connections {
		target: keyViewSelection

		onSelectionChanged: {
			path.selectionChanged()
		}
	}

	Connections {
		target: guiBackend

		onShowMessage: {
			ErrorDialog.showMessage(title, text, detailedText)
		}
	}

	//	Connections {
	//		target: treeView.currentNode === null ? null : treeView.currentNode.parentModel

	//		onShowMessage: {
	//			ErrorDialog.showMessage(title, text, detailedText)
	//		}
	//		onUpdateIndicator: {
	//			treeView.updateIndicator()
	//		}
	//	}

	//	Connections {
	//		target: treeView.currentNode === null ? null : treeView.currentNode.children

	//		onShowMessage: {
	//			ErrorDialog.showMessage(title, text, detailedText)
	//		}
	//		onUpdateIndicator: {
	//			treeView.updateIndicator()
	//		}
	//	}

	//**Colors*************************************************************************************************//

	//Get access to system colors
	SystemPalette {
		id: activePalette
		colorGroup: SystemPalette.Active
	}
	SystemPalette {
		id: inactivePalette
		colorGroup: SystemPalette.Inactive
	}
	SystemPalette {
		id: disabledPalette
		colorGroup: SystemPalette.Disabled
	}

	//**Windows************************************************************************************************//

	NewKeyWindow {
		id: newKeyWindow

		onVisibleChanged: {
			if (visible === true){

				nameTextField.forceActiveFocus()
			}
		}
	}

	EditKeyWindow {
		id: editKeyWindow

		onVisibleChanged: {
			if (visible === true){
				if (nameTextField.text.charAt(0) === '#'){
					editKeyWindow.title = qsTr("Edit Array Entry")
					nameTextField.textColor = disabledPalette.text
					nameTextField.readOnly = true
					valueTextField.forceActiveFocus()
				}
			}
		}
	}

	NewKeyWindow {
		id: newArrayWindow

		title: qsTr("Create new Array Entry")

		onVisibleChanged: {
			if (visible === true){
				nameTextField.readOnly = true
				nameTextField.textColor = disabledPalette.text
				nameTextField.text = selectedNode.children.getCurrentArrayNo()
				valueTextField.forceActiveFocus()
			}
		}
	}

	UnmountBackendWindow {
		id: unmountBackendWindow

		okButton.action.onTriggered: unmountBackendWindow.close()
	}

	WizardLoader {
		id: wizardLoader
	}

	AboutWindow {
		id: aboutWindow
	}

	PluginInfo {
		id: pluginInfo
	}

	AppearanceSettingsWindow {
		id: appearanceSettingsWindow
	}

	//**Dialogs************************************************************************************************//

	ExportDialog {
		id: exportDialog
	}

	ImportDialog {
		id: importDialog
	}

	ErrorDialog {
		id: errorDialog
	}

	ExitDialog {
		id: exitDialog
	}

	ColorDialog {
		id: colorDialog

		property string type

		title: qsTr("Please choose a color")
		modality: Qt.ApplicationModal

		/* This is a workaround, as of now (Qt5.6) does not set color before onAccepted */
		onCurrentColorChanged: {
			colorDialog.color = currentColor
		}

		onAccepted: {
			if (type === "highlight" && guiSettings.highlightColor !== colorDialog.color) {
				guiSettings.highlightColor = colorDialog.color
				appearanceSettingsWindow.optionEdited = true
			}
			else if (type === "frame" && guiSettings.frameColor !== colorDialog.color) {
				guiSettings.frameColor =  colorDialog.color
				appearanceSettingsWindow.optionEdited = true
			}
			else if (type === "nodeWith" && guiSettings.nodeWithKeyColor !== colorDialog.color) {
				guiSettings.nodeWithKeyColor = colorDialog.color
				appearanceSettingsWindow.optionEdited = true
			}
			else if (type === "nodeWithout" && guiSettings.nodeWithoutKeyColor !== colorDialog.color) {
				guiSettings.nodeWithoutKeyColor = colorDialog.color
				appearanceSettingsWindow.optionEdited = true
			}

			close()
		}
		onRejected: {
			close()
		}
	}

	//**Actions************************************************************************************************//

	GUIActions {
		id: guiActions
	}

	//**Menus & Toolbars***************************************************************************************//

	menuBar: MainMenuBar {
		id:mainMenuBar
	}

	toolBar: MainToolBar {
		id:mainToolbar
	}

	TreeContextMenu {
		id: treeContextMenu
	}

	KeyAreaContextMenu {
		id: keyAreaContextMenu
	}

	statusBar: StatusBar {
		id:mainStatusBar

		RowLayout {
			id: statusBarRow

			Label {
				id: path

				signal selectionChanged()

				anchors.fill: parent
				anchors.leftMargin: defaultMargins
				onSelectionChanged: text = !treeViewSelection.hasSelection ? "" : treeModel.data(noLeavesProxyModel.mapToSource(treeViewSelection.currentIndex), 258) +
																			 (!keyViewSelection.hasSelection ? "" :  "/" + treeModel.data(onlyLeavesProxyModel.mapToSource(keyViewSelection.currentIndex), 257))
			}
		}
	}

	//Search Results Area Context Menu

	Menu {
		id: searchResultsContextMenu

		MenuItem {
			id: srcmEdit
			action: guiActions.editAction
		}
		MenuItem {
			id: srcmDelete
			action: guiActions.deleteAction
		}
	}

	//**Layouts & Views****************************************************************************************//

	Row {
		id: mainRow

		anchors.fill: parent
		anchors.margins: defaultMargins
		spacing: defaultSpacing

		BasicRectangle {
			id:treeViewRectangle

			width: Math.ceil(parent.width*0.3)
			height: parent.height

			TreeView {
				id: treeView

				signal updateIndicator()

				anchors.fill: parent
				anchors.margins: 1
				headerVisible: false
				backgroundVisible: false
				alternatingRowColors: false
				frameVisible: false

				model: noLeavesProxyModel

				selection: ItemSelectionModel {
					id: treeViewSelection

					model: noLeavesProxyModel

					signal updateRoot()

					onCurrentChanged: {
						tableView.rootIndex = onlyLeavesProxyModel.mapFromSource(noLeavesProxyModel.mapToSource(currentIndex))
						keyViewSelection.clearSelection()
						metaView.model = null
					}
					onUpdateRoot: {
						tableView.rootIndex = onlyLeavesProxyModel.mapFromSource(noLeavesProxyModel.mapToSource(currentIndex))
					}
				}

				itemDelegate: Row {
					Label {
						id: label

						anchors.verticalCenter: parent.verticalCenter
						color: noLeavesProxyModel !== null ? (noLeavesProxyModel.data(styleData.index, 265) ? disabledPalette.windowText : activePalette.text) : ""
						text: styleData.value
					}
					Item {
						id: spacer
						width: defaultSpacing
						height: label.height
					}
					Indicator {
						id: indicator

						signal updateIndicator()

						Component.onCompleted: treeView.updateIndicator.connect(updateIndicator)

						paintcolor: label.color
						width: label.font.pixelSize*0.85
						height: width
						anchors.verticalCenter: label.verticalCenter
						opacity: (noLeavesProxyModel !== null ? (treeModel.data(noLeavesProxyModel.mapToSource(styleData.index), 262) > 0 && treeModel.data(noLeavesProxyModel.mapToSource(styleData.index), 263) ? 1 : 0) : 0)
						onUpdateIndicator: {
							paintcolor = label.color
							opacity = (noLeavesProxyModel !== null ? (treeModel.data(noLeavesProxyModel.mapToSource(styleData.index), 262) > 0 && treeModel.data(noLeavesProxyModel.mapToSource(styleData.index), 263) ? 1 : 0) : 0)

						}
					}
				}

				TableViewColumn {
					role: "baseName"
				}
			}
			HelpArea {
				helpText: qsTr("This is a tree view of the Key Database. It shows\nthe entire tree of the Key Database and allows\nyou to traverse the keys.")
			}
		}
		Column {
			id: keyMetaColumn

			spacing: defaultSpacing

			BasicRectangle {
				id: keyArea

				width: keyAreaWidth
				height: keyAreaHeight

				TreeView {
					id: tableView

					anchors.fill: parent
					anchors.margins: 1
					frameVisible: false
					alternatingRowColors: false
					backgroundVisible: false
					onCurrentIndexChanged: metaView.model = treeModel.data(onlyLeavesProxyModel.mapToSource(currentIndex), 266)
					model: onlyLeavesProxyModel

					selection: ItemSelectionModel {
						id: keyViewSelection
						model: onlyLeavesProxyModel
					}

					TableViewColumn {
						id: nameColumn

						role: "baseName"
						title: qsTr("Key Name")
						width: Math.ceil(keyArea.width*0.5 - defaultSpacing*0.5)
					}
					TableViewColumn {
						id: valueColumn

						role: "value"
						title: qsTr("Key Value")
						width: Math.ceil(keyArea.width*0.5 - defaultSpacing*0.5)
					}
				}
				HelpArea {
					helpText: qsTr("This view shows the keys in the current selected KeySet.\nIt shows the Key's name on the left and its value on the\nright. Double-clicking a Key will let you edit it.")
				}
			}
			BasicRectangle {
				id: metaArea

				width: keyAreaWidth
				height: metaAreaHeight

				TableView {
					id: metaView

					anchors.fill: parent
					anchors.margins: 1
					frameVisible: false
					alternatingRowColors: false
					backgroundVisible: false
					selectionMode: SelectionMode.NoSelection

					TableViewColumn {
						id: metaNameColumn

						role: "metaName"
						title: qsTr("Metakey Name")
						width: Math.ceil(metaArea.width*0.5 - defaultSpacing*0.5)
					}
					TableViewColumn {
						id: metaValueColumn

						role: "metaValue"
						title: qsTr("Metakey Value")
						width: Math.ceil(metaArea.width*0.5 - defaultSpacing*0.5)
					}
				}
				HelpArea {
					helpText: qsTr("This shows any metakeys attached to a Key. You can\nedit this metadata the same way you normally edit a\nKey, by double-clicking the key in the above view.")
				}
			}
			BasicRectangle {
				id: searchResultsArea

				width: keyAreaWidth
				height: searchResultsAreaHeight
				visible: false

				SequentialAnimation {
					id: searchResultsColorAnimation

					ColorAnimation {
						target: searchResultsArea
						property: "border.color"
						to: (searchResultsArea.visible &&  searchResultsView.model !== null) ? (searchResultsView.model.get(0).name === "NotfoundNode" ? "red" : "green") : activePalette.dark
						duration: 0
					}
					ColorAnimation {
						target: searchResultsArea
						property: "border.color"
						to: activePalette.dark
						duration: 1000
					}
				}

				ToolButton {
					id: searchResultsCloseButton

					iconSource: "icons/dialog-close.png"
					iconName: Helper.useIcon("dialog-close")
					anchors.right: parent.right
					anchors.top: parent.top
					anchors.margins: defaultSpacing
					tooltip: qsTr("Close")
					onClicked: {
						keyMetaColumn.state = ""
						searchResultsSelectedItem = null
						searchResultsListView.model.discardModel()
						searchResultsListView.model = null
					}
				}


				TreeView {
					id: searchResultsView

					anchors.fill: parent
					anchors.margins: defaultMargins
					anchors.rightMargin: searchResultsCloseButton.width+4
					//						clip: true
					model: null

					headerVisible: false
					frameVisible: false
					alternatingRowColors: false
					backgroundVisible: false

					selection: ItemSelectionModel {
						model:searchResultsView.model
					}

					TableViewColumn {
						id: searchResultsColum

						role:"name"
						width: Math.ceil(searchResultsArea.width*0.5 - defaultSpacing*0.5)


						//							Keys.onPressed: {
						//								if(event.key === Qt.Key_Up && searchResultsListView.currentIndex > 0){
						//									currentIndex--
						//									searchResultsSelectedItem = model.get(currentIndex)
						//								}
						//								else if(event.key === Qt.Key_Down && searchResultsListView.currentIndex < model.rowCount() - 1){
						//									currentIndex++
						//									searchResultsSelectedItem = model.get(currentIndex)
						//								}
						//								else if((event.key === Qt.Key_Enter || event.key === Qt.Key_Return) && searchResultsSelectedItem !== null){
						//									editKeyWindow.selectedNode = searchResultsSelectedItem
						//									editKeyWindow.accessFromSearchResults = true
						//									editAction.trigger()
						//								}
						//							}

						//							highlight: Rectangle {
						//								id: highlightBar
						//								color: guiSettings.highlightColor
						//							}

						//							delegate: Text {
						//								color: guiSettings.nodeWithKeyColor
						//								text: "name"

						//								MouseArea {
						//									anchors.fill: parent
						//									acceptedButtons: Qt.LeftButton | Qt.RightButton

						//									onClicked: {
						//										if(searchResultsListView.model.get(0).name !== "NotfoundNode"){
						//											if(mouse.button === Qt.LeftButton){
						//												searchResultsListView.currentIndex = index
						//												searchResultsSelectedItem = searchResultsListView.model.get(searchResultsListView.currentIndex)
						//												forceActiveFocus()
						//											}
						//											else if(mouse.button === Qt.RightButton) {
						//												searchResultsListView.currentIndex = index
						//												searchResultsSelectedItem = searchResultsListView.model.get(searchResultsListView.currentIndex)
						//												forceActiveFocus()
						//												editKeyWindow.accessFromSearchResults = true
						//												searchResultsContextMenu.popup()
						//											}
						//										}
						//									}
						//									onDoubleClicked: {
						//										if(searchResultsListView.model.get(0).name !== "NotfoundNode"){
						//											searchResultsListView.currentIndex = index
						//											forceActiveFocus()
						//											editKeyWindow.accessFromSearchResults = true
						//											editKeyWindow.selectedNode = searchResultsListView.model.get(searchResultsListView.currentIndex)
						//											editAction.trigger()
						//										}
						//									}
						//								}
						//							}
					}
				}
			}
			states:
				State {
				name: "SHOW_SEARCH_RESULTS"

				PropertyChanges {
					target: keyArea
					height: deltaKeyAreaHeight
				}
				PropertyChanges {
					target: metaArea
					height: deltaMetaAreaHeight
				}
				PropertyChanges {
					target: searchResultsArea
					visible: true
				}
			}
		}
	}
}
