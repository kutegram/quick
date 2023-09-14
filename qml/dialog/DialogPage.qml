import QtQuick 1.0
import Kutegram 1.0

Rectangle {
    id: pageRoot
    property alias foldersModel: foldersModel
    signal refresh()

    FoldersModel {
        id: foldersModel
        client: telegramClient
        Component.onCompleted: pageRoot.refresh.connect(refresh)
    }

    DialogsModel {
        id: dialogsModel
        elideLength: globalScreenLength
        folders: foldersModel
        client: telegramClient
        avatarDownloader: globalAvatarDownloader
        Component.onCompleted: pageRoot.refresh.connect(refresh)
    }

    ListView {
        id: folderSlide
        focus: true
        anchors.fill: parent

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

//            highlight: Rectangle {
//                width: folderSlide.width
//                height: 40 * kgScaling
//                opacity: 0.1
//                color: "#000000"
//            }

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
