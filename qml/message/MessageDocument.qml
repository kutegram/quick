import QtQuick 1.0

Rectangle {
    width: 240
    height: 40

    property alias image: documentImage.source
    property alias title: documentTitle.text
    property alias text: documentText.text

    MouseArea {
        anchors.fill: parent
        onClicked: {

        }
    }

    Rectangle {
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 5
        id: circleRect

        width: 30
        height: 30
        radius: 15
        smooth: true
        color: "#54759E"

        Image {
            id: documentImage
            anchors.centerIn: parent
            smooth: true
            height: 20
            width: 20
        }
    }

    Column {
        anchors.left: circleRect.right
        anchors.verticalCenter: circleRect.verticalCenter
        anchors.leftMargin: circleRect.anchors.leftMargin

        Row {
            spacing: 5
            Text {
                id: documentTitle
                font.bold: true
                font.pixelSize: 12
            }
        }

        Text {
            id: documentText
            color: "#8D8D8D"
            font.pixelSize: 12
        }
    }
}
