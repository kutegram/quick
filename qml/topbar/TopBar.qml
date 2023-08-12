import QtQuick 1.0
import "../dialog"

Rectangle {
    property alias currentState: topBarRoot.state

    id: topBarRoot
    height: 40
    width: 240
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    color: "#54759E"
    state: "MENU"

    Item {
        anchors.fill: parent
        anchors.leftMargin: parent.height
        state: topBarRoot.state

        ListView {
            id: folderList
            focus: true
            boundsBehavior: Flickable.StopAtBounds
            anchors.fill: parent
            orientation: ListView.Horizontal
            highlightFollowsCurrentItem: true
            highlightMoveDuration: 200
            highlightResizeDuration: 200

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

        Item {
            id: peerHeader
            anchors.fill: parent

            Rectangle {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: (parent.height - width) / 2
                id: avatarRect

                width: 30
                height: 30
                radius: 15
                smooth: true
                color: "#FFA3A3"

                Text {
                    anchors.fill: parent
                    text: "JP"
                    color: "#FFFFFF"
                    font.family: "Open Sans SemiBold"
                    font.pixelSize: 12
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Column {
                anchors.left: avatarRect.right
                anchors.verticalCenter: avatarRect.verticalCenter
                anchors.leftMargin: avatarRect.anchors.leftMargin

                Row {
                    spacing: 5
                    Text {
                        text: "Just Piggy"
                        font.family: "Open Sans SemiBold"
                        font.pixelSize: 12
                        color: "#FFFFFF"
                    }
                }

                Text {
                    text: "last seen at 09:35"
                    font.family: "Open Sans"
                    color: "#FFFFFF"
                    font.pixelSize: 12
                }
            }
        }

        states: [
            State {
                name: "MENU"
                PropertyChanges {
                    target: folderList
                    opacity: 1
                }
                PropertyChanges {
                    target: peerHeader
                    opacity: 0
                }
            },
            State {
                name: "BACK"
                PropertyChanges {
                    target: folderList
                    opacity: 0
                }
                PropertyChanges {
                    target: peerHeader
                    opacity: 1
                }
            }
        ]

        transitions: [
            Transition {
                to: "*"
                NumberAnimation {
                    properties: "opacity"
                    easing.type: Easing.InOutQuad
                    duration: 200
                }
            }
        ]
    }

    Rectangle {
        id: menuButton
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        width: parent.height
        height: parent.height
        color: parent.color
        state: topBarRoot.state

        Image {
            id: menuImage
            source: "../../img/menu.svg"
            x: 10
            y: 10
            width: 20
            height: 20
            smooth: true
        }

        Image {
            id: backImage
            source: "../../img/arrow-left.svg"
            x: 10
            y: 10
            width: 20
            height: 20
            smooth: true
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (stack.currentIndex == 1)
                    stack.currentIndex = 0;
            }
        }

        states: [
            State {
                name: "MENU"
                PropertyChanges {
                    target: menuImage
                    opacity: 1
                    x: 10
                }
                PropertyChanges {
                    target: backImage
                    opacity: 0
                    rotation: 180
                }
            },
            State {
                name: "BACK"
                PropertyChanges {
                    target: menuImage
                    opacity: 0
                    x: 0
                }
                PropertyChanges {
                    target: backImage
                    opacity: 1
                    rotation: 0
                }
            }
        ]

        transitions: [
            Transition {
                to: "*"
                NumberAnimation {
                    properties: "opacity,rotation,x"
                    easing.type: Easing.InOutQuad
                    duration: 200
                }
            }
        ]
    }
}
