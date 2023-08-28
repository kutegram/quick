import QtQuick 1.0

Item {
    width: ListView.view.width
    height: 40

    Item {
        id: actionIcon
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        width: height

        Image {
            anchors.centerIn: parent
            source: "../../img/checkbox-marked-circle.svg"
            smooth: true
            height: 20
            width: 20
        }
    }

    Text {
        anchors.left: actionIcon.right
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        verticalAlignment: Text.AlignVCenter
        text: "Action Button"
        font.pixelSize: 12
    }
}
