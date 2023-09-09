import QtQuick 1.0

Item {
    height: ListView.view.height
    width: tabContent.width+10

    Row {
        id: tabContent
        anchors.centerIn: parent
        spacing: 5

        //TODO: only icon, only text, text + icon
//        Image {
//            source: icon
//            smooth: true
//            width: 20
//            height: 20
//            asynchronous: true
//        }

        Text {
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
