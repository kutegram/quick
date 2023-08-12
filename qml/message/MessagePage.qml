import QtQuick 1.0

Rectangle {
    width: ListView.view.width
    height: ListView.view.height

    property string globalState: "NO_SELECT"

    ListView {
        anchors.fill: parent

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
}
