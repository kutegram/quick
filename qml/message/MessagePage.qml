import QtQuick 1.0
import Kutegram 1.0

Rectangle {
    property string globalState: "NO_SELECT"
    property alias messageEdit: messageEdit
    property alias messagesView: messagesView

    ListView {
        id: messagesView
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: messageEdit.top
        clip: true

        boundsBehavior: Flickable.StopAtBounds

        anchors.topMargin: Math.max(0, parent.height - messageEdit.height - childrenRect.height)

        cacheBuffer: parent.height / 6

        onMovementEnded: {
            if (atYBeginning && messagesModel.canFetchMoreUpwards()) {
                messagesModel.fetchMoreUpwards();
            }
            if (atYEnd && messagesModel.canFetchMoreDownwards()) {
                messagesModel.canFetchMoreDownwards();
            }
        }

        model: messagesModel

        delegate: MessageItem {
            state: globalState
        }
    }

    //TODO Hide MessageEdit when user is restricted
    MessageEdit {
        id: messageEdit
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }
}
