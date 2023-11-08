import QtQuick 1.0

Item {
    property alias currentState: messageRoot.state
    property int messageIndex: index

    id: messageRoot
    width: ListView.view.width
    height: Math.max(textContainer.height, avatarImage.height) + 6 * kgScaling
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
        y: 3 * kgScaling
        asynchronous: true
    }

    Rectangle {
        id: messageAvatar
        visible: !mergeMessage && !isChannel && (avatar.length == 0 || avatarImage.status != Image.Ready)

        anchors.top: parent.top
        anchors.topMargin: 3 * kgScaling
        anchors.left: checkbox.right
        anchors.leftMargin: (isChannel ? -35 : 5) * kgScaling

        width: 30 * kgScaling
        height: visible ? width : 0
        smooth: true

        color: thumbnailColor
        radius: width / 2

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
        visible: !mergeMessage && !isChannel && avatar.length != 0

        anchors.top: parent.top
        anchors.topMargin: 3 * kgScaling
        anchors.left: checkbox.right
        anchors.leftMargin: (isChannel ? -35 : 5) * kgScaling

        width: 30 * kgScaling
        height: visible ? width : 0
        smooth: true

        asynchronous: true
        source: avatar
    }

    Column {
        id: textContainer
        anchors.top: parent.top
        anchors.left: messageAvatar.right
        anchors.right: parent.right
        anchors.topMargin: 3 * kgScaling
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

        Repeater {
            model: hasPhoto
            MessageImage {
                state: currentState
                anchors.left: parent.left
                width: Math.min(280, parent.width)
            }
        }

        Repeater {
            model: hasMedia
            MessageDocument {
                rowIndex: messageIndex
                anchors.left: parent.left
                anchors.right: parent.right
                state: currentState
            }
        }
    }

//    Rectangle {
//        id: debugRectangle
//        anchors.fill: parent
//        color: Qt.hsla((messageId % 37) / 36, 0.5, 0.5, 0.8)
//    }
}
