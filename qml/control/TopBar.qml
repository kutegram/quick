import QtQuick 1.0
import "../dialog"

Rectangle {
    property string currentState: "MENU"

    id: topBarRoot
    height: 40
    width: 240
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    color: "#54759E"
    state: currentState == "CHAT" ? "BACK" : currentState

    Item {
        anchors.fill: parent
        anchors.leftMargin: parent.height
        state: currentState

        ListView {
            id: folderList
            focus: true
            clip: true
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

            MouseArea {
                anchors.fill: parent
            }

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
                anchors.right: actionsButton.left

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

            Item {
                id: actionsButton
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                width: height

                Image {
                    id: actionsImage
                    anchors.centerIn: parent
                    source: "../../img/dots-vertical.svg"
                    width: 20
                    height: 20
                    smooth: true
                }
            }
        }

        Text {
            id: appNameText
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            text: "Kutegram"
            font.family: "Open Sans SemiBold"
            font.pixelSize: 12
            color: "#FFFFFF"

            MouseArea {
                anchors.fill: parent
            }
        }

        states: [
            State {
                name: "CHAT"
                PropertyChanges {
                    target: folderList
                    opacity: 0
                    anchors.leftMargin: -20
                }
                PropertyChanges {
                    target: peerHeader
                    opacity: 1
                    anchors.leftMargin: 0
                }
                PropertyChanges {
                    target: appNameText
                    opacity: 0
                    anchors.leftMargin: -20
                }
            },
            State {
                name: "BACK"
                PropertyChanges {
                    target: folderList
                    opacity: 0
                    anchors.leftMargin: -20
                }
                PropertyChanges {
                    target: peerHeader
                    opacity: 0
                    anchors.leftMargin: -20
                }
                PropertyChanges {
                    target: appNameText
                    opacity: 1
                    anchors.leftMargin: 0
                }
            },
            State {
                name: "MENU"
                PropertyChanges {
                    target: folderList
                    opacity: 1
                    anchors.leftMargin: 0
                }
                PropertyChanges {
                    target: peerHeader
                    opacity: 0
                    anchors.leftMargin: -20
                }
                PropertyChanges {
                    target: appNameText
                    opacity: 0
                    anchors.leftMargin: -20
                }
            }

        ]

        transitions: [
            Transition {
                NumberAnimation {
                    properties: "opacity,anchors.leftMargin"
                    easing.type: Easing.InOutQuad
                    duration: 200
                }
            }
        ]
    }

    Item {
        id: menuButton
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        width: height
        state: currentState == "CHAT" ? "BACK" : currentState

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
                if (currentState == "MENU") {
                    currentState = "BACK"
                    drawer.opened = true;
                } else {
                    currentState = "MENU"
                    drawer.opened = false;
                    stack.currentIndex = 0;
                }
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
                NumberAnimation {
                    properties: "opacity,rotation,x"
                    easing.type: Easing.InOutQuad
                    duration: 200
                }
            }
        ]
    }
}
