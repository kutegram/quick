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
                x: -15 * kgScaling
            }
        },
        State {
            name: "SHOW_SELECT"
            PropertyChanges {
                target: checkbox
                opacity: 1
                x: 5 * kgScaling
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
        width: 20 * kgScaling
        height: width
        x: 5 * kgScaling
        y: 5 * kgScaling
        asynchronous: true
    }

    Rectangle {
        id: messageAvatar
        visible: !mergeMessage && !isChannel && (!avatarLoaded || avatarImage.status != Image.Ready)

        anchors.top: parent.top
        anchors.topMargin: 5 * kgScaling
        anchors.left: checkbox.right
        anchors.leftMargin: (isChannel ? -35 : 5) * kgScaling

        width: 30 * kgScaling
        height: width
        smooth: true

        color: thumbnailColor

        Text {
            anchors.fill: parent
            text: thumbnailText
            color: "#FFFFFF"
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    Image {
        id: avatarImage
        visible: !mergeMessage && !isChannel && avatarLoaded

        anchors.top: parent.top
        anchors.topMargin: 5 * kgScaling
        anchors.left: checkbox.right
        anchors.leftMargin: (isChannel ? -35 : 5) * kgScaling

        width: 30 * kgScaling
        height: width
        smooth: true

        asynchronous: true
        source: avatarLoaded ? avatar : ""
    }

    Column {
        id: textContainer
        anchors.top: parent.top
        anchors.left: messageAvatar.right
        anchors.right: parent.right
        anchors.topMargin: 5 * kgScaling
        anchors.leftMargin: 5 * kgScaling
        anchors.rightMargin: 5 * kgScaling
        spacing: 2 * kgScaling

        Row {
            anchors.left: parent.left
            anchors.right: parent.right
            spacing: 4 * kgScaling

            Text {
                text: senderName
                font.bold: true
                visible: !mergeMessage
            }

            Text {
                anchors.bottom: parent.bottom
                text: messageTime
                color: "#999999"
                visible: !mergeMessage
            }
        }

        Row {
            anchors.left: parent.left
            anchors.right: parent.right
            spacing: 4 * kgScaling
            visible: forwardedFrom.length != 0

            Image {
                source: "../../img/share_forwarded.png"
                smooth: true
                width: 20 * kgScaling
                height: width
                asynchronous: true
            }

            Text {
                text: forwardedFrom
                font.bold: true
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

            onLinkActivated: {
                messagesModel.linkActivated(link, index);
            }
        }

//        ListView {
//            anchors.left: parent.left
//            anchors.right: parent.right
//            height: 100 * kgScaling
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
