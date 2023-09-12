import QtQuick 1.0

Item {
    width: 240
    height: 40 * kgScaling

    MouseArea {
        anchors.fill: parent
        onClicked: {
            messagePage.messagesModel.peer = peerBytes;
            messagePage.messageEdit.peer = peerBytes;
            topBar.peerTitle = title;
            topBar.peerThumbnailColor = thumbnailColor;
            topBar.peerThumbnailText = thumbnailText;
            topBar.peerAvatarLoaded = avatarLoaded;
            topBar.peerAvatar = avatar;
            topBar.peerTooltip = tooltip;
            stack.currentIndex = 1;
        }
    }

    Rectangle {
        id: avatarRect
        visible: !avatarLoaded || avatarImage.status != Image.Ready

        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 5 * kgScaling

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
        visible: avatarLoaded

        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: avatarRect.anchors.leftMargin

        width: 30 * kgScaling
        height: width
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
            spacing: 4 * kgScaling

            Text {
                elide: Text.ElideRight
                text: title
            }

            Text {
                anchors.bottom: parent.bottom
                text: messageTime
                color: "#999999"
            }
        }

        Text {
            text: messageText
            color: "#8D8D8D"
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
