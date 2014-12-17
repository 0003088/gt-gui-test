import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Window 2.1
import QtQuick.Controls.Styles 1.1
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.1

RowLayout {

	anchors.bottom: parent.bottom
	anchors.right: parent.right
	anchors.margins: defaultMargins

	property alias nextButton: nextButton
	property alias finishButton: finishButton
	property alias cancelButton: cancelButton

	Button {
		id:nextButton
		action: Action {
			text: qsTr("&Next")
			iconSource: "icons/go-next.png"
		}
	}
	Button {
		id:finishButton
		action: Action {
			text: qsTr("&Finish")
			iconSource: "icons/dialog-ok.png"
		}
	}
	Button {
		id:cancelButton
		action: Action {
			text: qsTr("&Cancel")
			iconSource: "icons/dialog-cancel.png"
		}
	}
}
