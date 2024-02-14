import QtQuick 1.0
import "../dialog"

Item {
    property string peerTitle: ""
    property color peerThumbnailColor: "#00000000"
    property string peerThumbnailText: ""
    property string peerAvatar: ""
    property string peerTooltip: ""

    id: topBarRoot
    height: 40 * kgScaling
    width: 240 * kgScaling
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    state: currentState == "CHAT" ? "BACK" : currentState

    Component.onCompleted: {
        platformUtils.windowsExtendFrameIntoClientArea(0, height, 0, 0);
    }

    Rectangle {
        anchors.fill: parent
        color: globalAccent
        //opacity: platformUtils.windowsIsCompositionEnabled() ? 0 : 1
    }

    Rectangle {
        height: 1 * kgScaling
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: Qt.darker(globalAccent)
    }

    Item {
        anchors.fill: parent
        anchors.leftMargin: parent.height
        state: currentState

        ListView {
            id: folderList
            clip: true
            boundsBehavior: Flickable.StopAtBounds
            anchors.left: parent.left
            width: parent.width
            height: parent.height
            orientation: ListView.Horizontal
            highlightFollowsCurrentItem: true
            highlightMoveDuration: 200
            highlightResizeDuration: 200

            currentIndex: currentFolderIndex
            onCurrentItemChanged: {
                currentFolderIndex = currentIndex
            }

            model: foldersModel

            delegate: FolderItem {

            }

            highlight: Item {
                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 1 * kgScaling
                    height: 2 * kgScaling
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
            anchors.left: parent.left
            width: parent.width
            height: parent.height

            MouseArea {
                anchors.fill: parent
            }

            Rectangle {
                id: avatarRect
                visible: peerAvatar.length == 0 || avatarImage.status != Image.Ready

                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: (parent.height - width) / 2

                width: 30 * kgScaling
                height: width
                smooth: true

                color: peerThumbnailColor
                radius: width / 2

                Text {
                    anchors.fill: parent
                    text: peerThumbnailText
                    color: "#FFFFFF"
                    font.bold: true
                    font.pixelSize: 12 * kgScaling
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }

            Image {
                id: avatarImage
                visible: peerAvatar.length != 0

                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: avatarRect.anchors.leftMargin

                width: 30 * kgScaling
                height: width
                smooth: true

                asynchronous: true
                source: peerAvatar
            }

            Column {
                anchors.left: avatarRect.right
                anchors.verticalCenter: avatarRect.verticalCenter
                anchors.leftMargin: avatarRect.anchors.leftMargin
                anchors.right: actionsButton.left

                Row {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    spacing: 4 * kgScaling

                    Text {
                        elide: Text.ElideRight
                        text: peerTitle
                        font.bold: true
                        font.pixelSize: 12 * kgScaling
                        color: "#FFFFFF"
                    }
                }

                Text {
                    text: peerTooltip
                    color: "#FFFFFF"
                    anchors.left: parent.left
                    anchors.right: parent.right
                    elide: Text.ElideRight
                    font.pixelSize: 12 * kgScaling
                }
            }

            Item {
                id: actionsButton
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                width: height

                visible: false

                Image {
                    id: actionsImage
                    anchors.centerIn: parent
                    source: "../../img/dots-vertical.png"
                    width: 20 * kgScaling
                    height: height
                    smooth: true
                    asynchronous: true
                }
            }
        }

        Text {
            id: appNameText
            anchors.left: parent.left
            width: parent.width
            height: parent.height
            verticalAlignment: Text.AlignVCenter
            text: "Kutegram"
            font.bold: true
            font.pixelSize: 12 * kgScaling
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
                    anchors.leftMargin: -folderList.width
                }
                PropertyChanges {
                    target: peerHeader
                    opacity: 1
                    anchors.leftMargin: 0
                }
                PropertyChanges {
                    target: appNameText
                    opacity: 0
                    anchors.leftMargin: -appNameText.width
                }
            },
            State {
                name: "BACK"
                PropertyChanges {
                    target: folderList
                    opacity: 0
                    anchors.leftMargin: -folderList.width
                }
                PropertyChanges {
                    target: peerHeader
                    opacity: 0
                    anchors.leftMargin: -peerHeader.width
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
                    anchors.leftMargin: -peerHeader.width
                }
                PropertyChanges {
                    target: appNameText
                    opacity: 0
                    anchors.leftMargin: -appNameText.width
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

    property string currentState: "MENU"

    function menuButtonClicked() {
        if (currentState == "MENU") {
            currentState = "BACK";
            drawer.opened = true;
        } else {
            currentState = "MENU";
            drawer.opened = false;
            mainScreen.state = "MENU";
        }
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
            source: "../../img/menu.png"
            x: 10 * kgScaling
            y: 10 * kgScaling
            width: 20 * kgScaling
            height: width
            smooth: true
            asynchronous: true
        }

        Image {
            id: backImage
            source: "../../img/arrow-left.png"
            x: 10 * kgScaling
            y: 10 * kgScaling
            width: 20 * kgScaling
            height: width
            smooth: true
            asynchronous: true
        }

        MouseArea {
            id: menuButtonArea
            anchors.fill: parent
            onClicked: {
                menuButtonClicked();
            }
        }

        states: [
            State {
                name: "MENU"
                PropertyChanges {
                    target: menuImage
                    opacity: 1
                    x: 10 * kgScaling
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
