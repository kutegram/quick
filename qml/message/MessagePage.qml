import QtQuick 1.0
import Kutegram 1.0

Rectangle {
    property string globalState: "NO_SELECT"
    property alias messagesModel: messagesModel
    property alias messageEdit: messageEdit

    ListView {
        id: messagesView
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: messageEdit.top

        boundsBehavior: Flickable.StopAtBounds

        anchors.topMargin: Math.max(0, parent.height - messageEdit.height - childrenRect.height)

        cacheBuffer: parent.height / 6

        onMovementEnded: {
            if (atYBeginning && messagesModel.canFetchMoreUpwards()) {
                messagesModel.fetchMoreUpwards();
            }
        }

        model: MessagesModel {
            id: messagesModel
            client: telegramClient
            avatarDownloader: globalAvatarDownloader

            onScrollTo: {
                messagesView.positionViewAtIndex(index, ListView.Beginning);
            }

            onScrollForNew: {
                if (messagesView.atYEnd) {
                    messagesView.positionViewAtIndex(messagesView.count - 1, ListView.Contain);
                }
            }

            onDraftChanged: {
                messageEdit.messageText = draft;
            }

            onUploadingProgress: {
                messageEdit.uploadingProgress(progress);
            }
        }

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
