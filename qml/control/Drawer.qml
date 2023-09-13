import QtQuick 1.0
import Kutegram 1.0

Item {
    property bool opened: false

    CurrentUserInfo {
        id: currentUserInfo
        client: telegramClient
        avatarDownloader: globalAvatarDownloader

        onUserAvatarDownloaded: {
            peerAvatar = avatar;
        }

        onUserInfoChanged: {
            peerThumbnailText = thumbnailText;
            peerThumbnailColor = thumbnailColor;
            peerName = name;
            peerUsername = username;
        }
    }

    property string peerAvatar: ""
    property string peerThumbnailText: ""
    property color peerThumbnailColor: "#00000000"
    property string peerName: ""
    property string peerUsername: ""

    id: drawerRoot
    width: 240
    height: 320

    state: opened ? "OPENED" : "CLOSED"

    states: [
        State {
            name: "OPENED"
            PropertyChanges {
                target: dimmBackground
                opacity: 0.5
            }
            PropertyChanges {
                target: drawerSlide
                currentIndex: 0
                anchors.leftMargin: 0
            }
        },
        State {
            name: "CLOSED"
            PropertyChanges {
                target: dimmBackground
                opacity: 0
            }
            PropertyChanges {
                target: drawerSlide
                anchors.leftMargin: -Math.max(240, drawerRoot.width)
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

    Rectangle {
        id: dimmBackground
        anchors.fill: parent
        color: "#000000"
    }

    ListView {
        id: drawerSlide
        anchors.left: parent.left
        width: drawerRoot.width
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        boundsBehavior: Flickable.StopAtBounds
        orientation: ListView.Horizontal
        snapMode: ListView.SnapOneItem
        highlightRangeMode: ListView.StrictlyEnforceRange
        highlightFollowsCurrentItem: true
        highlightMoveDuration: 200

        onCurrentItemChanged: {
            if (currentIndex == 1) {
                topBar.currentState = "MENU"
                opened = false;
            }
        }

        model: VisualItemModel {
            Item {
                id: drawerContent
                width: drawerRoot.width
                height: drawerRoot.height

                MouseArea {
                    id: dimmMouseArea
                    anchors.fill: parent

                    onClicked: {
                        topBar.currentState = "MENU"
                        opened = false;
                    }
                }

                Rectangle {
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.bottom: parent.bottom
                    width: Math.min(Math.max(240, parent.width * 2 / 3), 280 * kgScaling)
                    color: "#FFFFFF"

                    Rectangle {
                        id: currentUserRect
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: 90 * kgScaling
                        color: globalAccent

                        MouseArea {
                            anchors.fill: parent
                        }

                        Rectangle {
                            id: avatarRect
                            visible: peerAvatar.length == 0 || avatarImage.status != Image.Ready

                            anchors.left: parent.left
                            anchors.top: parent.top
                            anchors.topMargin: 8 * kgScaling
                            anchors.leftMargin: anchors.topMargin

                            width: 40 * kgScaling
                            height: width
                            smooth: true

                            color: peerThumbnailColor

                            Text {
                                anchors.fill: parent
                                text: peerThumbnailText
                                color: "#FFFFFF"
                                font.bold: true
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                        }

                        Image {
                            id: avatarImage
                            visible: peerAvatar.length != 0

                            anchors.left: parent.left
                            anchors.top: parent.top
                            anchors.topMargin: avatarRect.anchors.topMargin
                            anchors.leftMargin: avatarRect.anchors.topMargin

                            width: 40 * kgScaling
                            height: width
                            smooth: true

                            asynchronous: true
                            source: peerAvatar
                        }

                        Text {
                            id: avatarName
                            text: peerName
                            color: "#FFFFFF"
                            font.bold: true

                            anchors.left: parent.left
                            anchors.top: avatarImage.bottom
                            anchors.topMargin: avatarRect.anchors.topMargin
                            anchors.leftMargin: avatarRect.anchors.topMargin
                            anchors.right: parent.right
                            anchors.rightMargin: avatarRect.anchors.topMargin
                        }

                        Text {
                            id: avatarUsername
                            text: peerUsername.length != 0 ? "@" + peerUsername : "no username"
                            color: "#FFFFFF"

                            anchors.left: parent.left
                            anchors.top: avatarName.bottom
                            anchors.topMargin: 0
                            anchors.leftMargin: avatarRect.anchors.topMargin
                            anchors.right: parent.right
                            anchors.rightMargin: avatarRect.anchors.topMargin
                        }
                    }

                    ListView {
                        id: drawerListView
                        anchors.top: currentUserRect.bottom
                        anchors.bottom: drawerBottom.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        boundsBehavior: Flickable.StopAtBounds

//                        highlight: Rectangle {
//                            width: drawerListView.width
//                            height: 40 * kgScaling
//                            opacity: 0.1
//                            color: "#000000"
//                        }

                        model: ListModel {
                            ListElement {
                                icon: "../../img/refresh.png"
                                name: "Refresh dialogs"
                            }
                            ListElement {
                                icon: "../../img/exit-to-app.png"
                                name: "Log out"
                            }
                            ListElement {
                                icon: "../../img/close.png"
                                name: "Close"
                            }
                        }

                        delegate: DrawerButton {

                        }
                    }

                    Rectangle {
                        id: drawerBottom
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        color: "#FFFFFF"
                        height: versionRow.height + 16 * kgScaling

                        MouseArea {
                            anchors.fill: parent
                        }

                        Text {
                            id: versionRow
                            anchors.bottom: parent.bottom
                            anchors.left: parent.left
                            anchors.leftMargin: 12 * kgScaling
                            anchors.right: parent.right
                            anchors.rightMargin: 12 * kgScaling
                            anchors.bottomMargin: 8 * kgScaling

                            color: "#999999"
                            text: "Version " + kutegramVersion + " for " + kutegramPlatform
                            elide: Text.ElideRight
                            font.bold: true
                        }
                    }
                }
            }
            Item {
                id: drawerSpace
                width: drawerRoot.width
                height: drawerRoot.height
            }
        }
    }
}
