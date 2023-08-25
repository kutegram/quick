import QtQuick 1.0

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
            width: folderSlide.width
            height: folderSlide.height
            focus: true
            model: 10
            delegate: DialogItem {

            }
        }
    }
}
