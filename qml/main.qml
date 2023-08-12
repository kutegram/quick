import QtQuick 1.0
import "dialog"
import "message"
import "topbar"

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

        DialogPage {
            id: dialogPage
        }

        MessagePage {
            id: messagePage
        }
    }

    TopBar {
        id: topBar
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
    }
}
