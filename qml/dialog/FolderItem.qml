import QtQuick 1.0

Item {
    height: ListView.view.height
    width: tabContent.width + 10 * kgScaling

    Row {
        id: tabContent
        anchors.centerIn: parent
        spacing: 5 * kgScaling

        //TODO: only icon, only text, text + icon
        Image {
            source: icon
            smooth: true
            width: folderText.font.pixelSize * 1.5
            height: width
            asynchronous: true
            visible: icon.length != 0
        }

        Text {
            id: folderText
            text: title
            visible: title.length != 0
            color: "#FFFFFF"
            font.pixelSize: 12 * kgScaling
        }
    }

    MouseArea {
        anchors.fill: parent

        onClicked: {
            currentFolderIndex = index
        }
    }
}
