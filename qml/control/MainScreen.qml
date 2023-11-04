import QtQuick 1.0
import "../dialog"
import "../message"
import "../control"
import "../auth"

Rectangle {
    id: mainScreen
    state: "MENU"

    onStateChanged: {
        topBar.currentState = state;
        messagePage.globalState = "NO_SELECT";
    }

    DialogPage {
        id: dialogPage
        width: parent.width
        height: parent.height
    }

    MessagePage {
        id: messagePage
        width: parent.width
        height: parent.height

        x: parent.state == "MENU" ? width : 0
        Behavior on x {
            NumberAnimation {
                easing.type: Easing.InOutQuad
            }
        }
    }

    TopBar {
        id: topBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
    }

    Drawer {
        id: drawer
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }

    ImageViewer {
        id: imageViewer
        anchors.fill: parent
    }
}
