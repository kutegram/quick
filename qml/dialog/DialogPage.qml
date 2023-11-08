import QtQuick 1.0
import Kutegram 1.0

Rectangle {
    id: pageRoot
    signal refresh()

    ListView {
        id: folderSlide
        focus: true
        anchors.fill: parent
        clip: true

        model: foldersModel
        cacheBuffer: width * 5

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
            cacheBuffer: pageRoot.height / 6
            focus: true

            boundsBehavior: Flickable.StopAtBounds

            onMovementEnded: {
                if (atYEnd && dialogsModel.canFetchMoreDownwards()) {
                    dialogsModel.canFetchMoreDownwards();
                }
            }

            model: dialogsModel

            delegate: Repeater {
                id: dialogRepeater

                model: dialogsModel.inFolder(index, folderIndex)
                height: count != 0 ? 40 * kgScaling : 0

                DialogItem {
                    y: dialogRepeater.y
                    width: dialogsView.width
                }
            }
        }
    }
}
