import QtQuick 1.0
import "../control"

Rectangle {
    //TODO: think about it
    width: ListView.view.width
    height: ListView.view.height

    Column {
        anchors.centerIn: parent
        width: Math.min(320, parent.width * 2 / 3)
        spacing: 5

        Text {
            anchors.left: parent.left
            anchors.right: parent.right
            text: "Confirmation Code"
            font.family: "Open Sans SemiBold"
            font.pixelSize: 12
            wrapMode: Text.Wrap
        }

        Text {
            anchors.left: parent.left
            anchors.right: parent.right
            text: "Please, enter your confirmation code."
            font.family: "Open Sans"
            font.pixelSize: 12
            wrapMode: Text.Wrap
        }

        LineEdit {
            anchors.left: parent.left
            anchors.right: parent.right
        }

        Button {
            anchors.left: parent.left
            anchors.right: parent.right
            onClicked: {
                root.state = "MAIN"
            }
        }
    }
}
