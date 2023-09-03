import QtQuick 1.0
import Kutegram 1.0

Rectangle {
    id: pageRoot
    property alias foldersModel: foldersModel
    signal refresh()

    ListView {
        id: folderSlide
        focus: true
        anchors.fill: parent

        model: ListModel {
            id: foldersModel
            ListElement {
                icon: "../../img/forum.png"
                label: "All Chats"
            }
        }

        boundsBehavior: Flickable.StopAtBounds
        orientation: ListView.Horizontal
        snapMode: ListView.SnapOneItem
        highlightRangeMode: ListView.StrictlyEnforceRange
        highlightFollowsCurrentItem: true
        highlightMoveDuration: 200

        currentIndex: currentFolderIndex
        onCurrentItemChanged: {
            currentFolderIndex = currentIndex
        }

        delegate: ListView {
            id: dialogsView
            width: folderSlide.width
            height: folderSlide.height
            focus: true

            highlight: Rectangle {
                width: folderSlide.width
                height: 40
                opacity: 0.1
                color: "#000000"
            }

            model: DialogsModel {
                client: telegramClient
                avatarDownloader: globalAvatarDownloader
                Component.onCompleted: pageRoot.refresh.connect(refresh)
            }

            delegate: DialogItem {

            }
        }
    }
}
