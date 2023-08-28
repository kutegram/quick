import QtQuick 1.0
import Kutegram 1.0

Rectangle {
    ListView {
        id: folderSlide
        focus: true
        anchors.fill: parent
        model: 3
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
            model: DialogsModel {
                id: dialogsModel
                client: telegramClient
            }

            delegate: DialogItem {

            }
        }
    }
}
