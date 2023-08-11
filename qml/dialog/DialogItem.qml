import QtQuick 1.0

Rectangle {
    width: 240
    height: 40

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

        Text {
            text: "Just Piggy"
            font.family: "Open Sans SemiBold"
            font.pixelSize: 12
        }

        Text {
            text: "Hello, world!"
            font.family: "Open Sans"
            color: "#8D8D8D"
            font.pixelSize: 12
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
