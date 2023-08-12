import QtQuick 1.0

Item {
    default property alias content: visualModel.children
    property alias currentIndex: stackView.currentIndex
    property alias currentItem: stackView.currentItem

    ListView {
        id: stackView
        anchors.fill: parent
        boundsBehavior: Flickable.StopAtBounds
        orientation: ListView.Horizontal
        snapMode: ListView.SnapOneItem
        highlightRangeMode: ListView.StrictlyEnforceRange
        highlightFollowsCurrentItem: true
        highlightMoveDuration: 200
        model: VisualItemModel {
            id: visualModel
        }
    }
}
