import QtQuick 1.0
import "dialog"

Rectangle {
    width: 240
    height: 320

    property int currentFolderIndex: 0

    ListView {
        id: folderSlide
        focus: true
        boundsBehavior: Flickable.StopAtBounds
        anchors.top: topbar.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        model: 3
        orientation: ListView.Horizontal
        snapMode: ListView.SnapOneItem
        highlightRangeMode: ListView.StrictlyEnforceRange
        highlightFollowsCurrentItem: true
        highlightMoveDuration: 300

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

    Rectangle {
        id: topbar
        height: 40
        width: 240
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        color: "#54759E"

        ListView {
            id: folderList
            focus: true
            boundsBehavior: Flickable.StopAtBounds
            anchors.fill: parent
            anchors.leftMargin: parent.height
            orientation: ListView.Horizontal
            highlightFollowsCurrentItem: true
            highlightMoveDuration: 300
            highlightResizeDuration: 300

            currentIndex: currentFolderIndex
            onCurrentItemChanged: {
                currentFolderIndex = currentIndex
            }

            model: ListModel {
                ListElement {
                    icon: "../../img/forum.svg"
                    label: "All Chats"
                }

                ListElement {
                    icon: "../../img/message-badge.svg"
                    label: "Unread"
                }

                ListElement {
                    icon: "../../img/bullhorn.svg"
                    label: "Channels"
                }
            }

            delegate: FolderItem {

            }

            highlight: Item {
                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    height: 2
                    color: "#FFFFFF"

                }

                Behavior on x {
                    NumberAnimation {
                        easing.type: Easing.InOutQuad
                    }
                }
                Behavior on width {
                    NumberAnimation {
                        easing.type: Easing.InOutQuad
                    }
                }
            }
        }

        Rectangle {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            width: parent.height
            height: parent.height
            color: parent.color

            Image {
                anchors.centerIn: parent
                source: "../img/menu.svg"
                width: 20
                height: 20
            }
        }
    }

}
