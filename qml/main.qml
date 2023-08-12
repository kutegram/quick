import QtQuick 1.0
import "dialog"
import "message"
import "control"

Rectangle {
    width: 320
    height: 240

    property int currentFolderIndex: 0

    Stack {
        id: stack
        anchors.top: topBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        onCurrentItemChanged: {
            topBar.currentState = currentIndex == 1 ? "CHAT" : "MENU";
            messagePage.globalState = "NO_SELECT";
        }

        DialogPage {
            id: dialogPage
        }

        MessagePage {
            id: messagePage
        }
    }

    Drawer {
        id: drawer
        anchors.top: topBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }

    TopBar {
        id: topBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
    }
}
