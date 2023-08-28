import QtQuick 1.0

Rectangle {
    width: ListView.view.width
    height: 40

    MouseArea {
        anchors.fill: parent
        onClicked: {
            stack.currentIndex = 1
        }
    }

    Rectangle {
        id: avatarRect
        visible: !avatarLoaded

        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 5

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
        visible: avatarLoaded

        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 5

        width: 30
        height: 30
        smooth: true

        asynchronous: true
        source: avatarLoaded ? avatar : ""
    }

    Column {
        anchors.left: avatarRect.right
        anchors.right: parent.right
        anchors.verticalCenter: avatarRect.verticalCenter
        anchors.leftMargin: avatarRect.anchors.leftMargin
        anchors.rightMargin: anchors.leftMargin

        Row {
            anchors.left: parent.left
            anchors.right: parent.right

            spacing: 4
            Text {
                elide: Text.ElideRight
                text: title
                font.pixelSize: 12
            }

            Text {
                anchors.bottom: parent.bottom
                text: messageTime
                font.pixelSize: 10
                color: "#999999"
            }
        }

        Text {
            text: messageText
            color: "#8D8D8D"
            font.pixelSize: 12
            anchors.left: parent.left
            anchors.right: parent.right
            elide: Text.ElideRight
        }
    }

    Rectangle {
        height: 1
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        color: "#EEEEEE"
    }
}
