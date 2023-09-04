import QtQuick 1.0

Item {
    property alias currentState: messageRoot.state
    property int messageIndex: index

    id: messageRoot
    width: ListView.view.width
    height: textContainer.height
    state: "NO_SELECT"

    states: [
        State {
            name: "NO_SELECT"
            PropertyChanges {
                target: checkbox
                opacity: 0
                x: -15
            }
        },
        State {
            name: "SHOW_SELECT"
            PropertyChanges {
                target: checkbox
                opacity: 1
                x: 5
            }
        }
    ]

    transitions: [
        Transition {
            NumberAnimation {
                properties: "x,opacity"
                easing.type: Easing.InOutQuad
                duration: 200
            }
        }
    ]

//    MouseArea {
//        anchors.fill: parent
//        onClicked: {
//            if (ListView.view.parent.globalState == "SHOW_SELECT")
//                ListView.view.parent.globalState = "NO_SELECT"
//            else ListView.view.parent.globalState = "SHOW_SELECT"
//        }
//    }

    Image {
        id: checkbox
        source: "../../img/checkbox-blank-circle-outline.png"
        smooth: true
        height: 20
        width: 20
        x: 5
        y: 5
        asynchronous: true
    }

    Rectangle {
        id: messageAvatar
        visible: !mergeMessage && !isChannel && (!avatarLoaded || avatarImage.status != Image.Ready)

        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.left: checkbox.right
        anchors.leftMargin: isChannel ? -35 : 5

        width: 30
        height: 30
        smooth: true

        color: thumbnailColor

        Text {
            anchors.fill: parent
            text: thumbnailText
            color: "#FFFFFF"
            font.bold: true
            font.pixelSize: 12
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    Image {
        id: avatarImage
        visible: !mergeMessage && !isChannel && avatarLoaded

        anchors.top: parent.top
        anchors.topMargin: 5
        anchors.left: checkbox.right
        anchors.leftMargin: isChannel ? -35 : 5

        width: 30
        height: 30
        smooth: true

        asynchronous: true
        source: avatarLoaded ? avatar : ""
    }

    Column {
        id: textContainer
        anchors.top: parent.top
        anchors.left: messageAvatar.right
        anchors.right: parent.right
        anchors.topMargin: 5
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        spacing: 2

        Row {
            anchors.left: parent.left
            anchors.right: parent.right
            spacing: 4

            Text {
                text: senderName
                font.bold: true
                font.pixelSize: 12
                visible: !mergeMessage
            }

            Text {
                anchors.bottom: parent.bottom
                text: messageTime
                font.pixelSize: 10
                color: "#999999"
                visible: !mergeMessage
            }
        }

        Row {
            anchors.left: parent.left
            anchors.right: parent.right
            spacing: 4
            visible: forwardedFrom.length != 0

            Image {
                source: "../../img/share_forwarded.png"
                smooth: true
                width: 20
                height: 20
                asynchronous: true
            }

            Text {
                text: forwardedFrom
                font.bold: true
                font.pixelSize: 12
                color: "#8D8D8D"
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        Text {
            anchors.left: parent.left
            anchors.right: parent.right

            wrapMode: Text.Wrap
            text: messageText
            visible: messageText.length != 0
            color: "#000000"
            font.pixelSize: 12

            onLinkActivated: {
                messagesModel.linkActivated(link, index);
            }
        }

//        ListView {
//            anchors.left: parent.left
//            anchors.right: parent.right
//            height: 100
//            //TODO: think about this value
//            cacheBuffer: 20000
//            spacing: 5
//            orientation: ListView.Horizontal
//            snapMode: ListView.SnapOneItem
//            highlightRangeMode: ListView.StrictlyEnforceRange
//            highlightFollowsCurrentItem: true
//            highlightMoveDuration: 200
//            clip: true

//            model: ListModel {
////                ListElement {
////                    image: "../../img/test/1.jpg"
////                }
////                ListElement {
////                    image: "../../img/test/2.jpg"
////                }
////                ListElement {
////                    image: "../../img/test/3.jpg"
////                }
////                ListElement {
////                    image: "../../img/test/4.jpg"
////                }
////                ListElement {
////                    image: "../../img/test/5.jpg"
////                }
//            }
//            delegate: MessageImage {
//                state: currentState
//            }
//        }

        Repeater {
            model: hasMedia
            MessageDocument {
                image: mediaImage
                title: mediaTitle
                text: mediaText
                rowIndex: messageIndex
                mid: messageId
                downloadable: mediaDownloadable
                anchors.left: parent.left
                anchors.right: parent.right
                state: currentState
            }
        }
    }
}
