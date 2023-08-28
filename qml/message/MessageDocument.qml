import QtQuick 1.0

Rectangle {
    width: 240
    height: 40

    MouseArea {
        anchors.fill: parent
        onClicked: {

        }
    }

    Rectangle {
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 5
        id: avatarRect

        width: 30
        height: 30
        radius: 15
        smooth: true
        color: "#54759E"

        Image {
            anchors.centerIn: parent
            source: "../../img/file.svg"
            smooth: true
            height: 20
            width: 20
        }
    }

    Column {
        anchors.left: avatarRect.right
        anchors.verticalCenter: avatarRect.verticalCenter
        anchors.leftMargin: avatarRect.anchors.leftMargin

        Row {
            spacing: 5
            Text {
                text: "JbakTaskManTimeless.zip"
                font.bold: true
                font.pixelSize: 12
            }
        }

        Text {
            text: "1.0 MB"
            color: "#8D8D8D"
            font.pixelSize: 12
        }
    }
}
