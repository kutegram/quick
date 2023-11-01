import QtQuick 1.0

Item {
    default property alias content: visualModel.children
    property alias model: visualModel
    property alias currentIndex: stackView.currentIndex
    property alias currentItem: stackView.currentItem
    property alias flickableDirection: stackView.flickableDirection

    ListView {
        id: stackView
        anchors.fill: parent
        boundsBehavior: Flickable.StopAtBounds
        orientation: ListView.Horizontal
        snapMode: ListView.SnapOneItem
        highlightRangeMode: ListView.StrictlyEnforceRange
        highlightFollowsCurrentItem: true
        highlightMoveDuration: 200
        cacheBuffer: parent.width * 3
        model: VisualItemModel {
            id: visualModel
        }
    }
}
