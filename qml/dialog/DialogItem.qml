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

        Row {
            spacing: 5
            Text {
                text: "Just Piggy"
                font.family: "Open Sans SemiBold"
                font.pixelSize: 12
            }

            Text {
                anchors.bottom: parent.bottom
                text: "09:35"
                font.family: "Open Sans SemiBold"
                font.pixelSize: 10
                color: "#999999"
            }
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
