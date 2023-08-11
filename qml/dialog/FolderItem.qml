import QtQuick 1.0

Item {
    height: 40
    width: 80

    Row {
        anchors.centerIn: parent
        spacing: 4

        Image {
            source: icon
            width: 16
            height: 16
        }

        Text {
            font.family: "Open Sans SemiBold"
            font.pixelSize: 12
            text: label
            color: "#FFFFFF"
        }
    }

    MouseArea {
        anchors.fill: parent

        onClicked: {
            currentFolderIndex = index
        }
    }
}
