import QtQuick 1.0
import "../dialog"
import "../message"
import "../control"
import "../auth"

Rectangle {
    id: mainScreen
    state: "MENU"

    states: [
        State {
            name: "MENU"
            PropertyChanges {
                target: messagePage
                x: mainScreen.width
            }
        },
        State {
            name: "CHAT"
            PropertyChanges {
                target: messagePage
                x: mainScreen.width < 600 * kgScaling ? 0 : dialogPage.width
            }
        }
    ]

    transitions: [
        Transition {
            NumberAnimation {
                properties: "x"
                easing.type: Easing.InOutQuad
                duration: 200
            }
        }
    ]

    onStateChanged: {
        topBar.currentState = state;
        messagePage.globalState = "NO_SELECT";
    }

    DialogPage {
        id: dialogPage
        width: parent.width < 600 * kgScaling ? parent.width : 300 * kgScaling
        height: parent.height
    }

    MessageIntroPage {
        id: messageIntroPage
        width: messagePage.width
        height: parent.height
        x: dialogPage.width
    }

    MessagePage {
        id: messagePage
        width: parent.width < 600 * kgScaling ? parent.width : parent.width - 300 * kgScaling
        height: parent.height
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
