import QtQuick 1.0

Rectangle {
    //TODO: think about it
    width: ListView.view.width
    height: ListView.view.height

    property string globalState: "NO_SELECT"

    ListView {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: messageEdit.top
        spacing: 5

        model: ListModel {
            ListElement {
                senderId: 0
                previousSenderId: -1
            }

            ListElement {
                senderId: 0
                previousSenderId: 0
            }

            ListElement {
                senderId: 1
                previousSenderId: 0
            }

            ListElement {
                senderId: 2
                previousSenderId: 1
            }
        }

        delegate: MessageItem {
            state: globalState
        }
    }

    MessageEdit {
        id: messageEdit
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }
}
