import QtQuick 1.0
import Kutegram 1.0

Rectangle {
    property string globalState: "NO_SELECT"
    property alias messagesModel: messagesModel
    property alias messageEdit: messageEdit

    ListView {
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: messageEdit.top
        spacing: 2

        onMovementEnded: {
            if (atYBeginning && messagesModel.canFetchMoreUpwards()) {
                messagesModel.fetchMoreUpwards();
            }
        }

        model: MessagesModel {
            id: messagesModel
            client: telegramClient
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
