import QtQuick 1.0
import "dialog"
import "message"
import "control"
import "auth"
import Kutegram 1.0

Rectangle {
    width: 320
    height: 240

    TgClient {
        id: telegramClient
    }

    Component.onCompleted: {
        telegramClient.start();
    }

    id: root
    state: "AUTH"

    states: [
        State {
            name: "AUTH"
            PropertyChanges {
                target: mainRect
                anchors.leftMargin: root.width
                opacity: 0
            }
        },
        State {
            name: "MAIN"
            PropertyChanges {
                target: mainRect
                anchors.leftMargin: 0
                opacity: 1
            }
        }
    ]

    transitions: [
        Transition {
            NumberAnimation {
                properties: "opacity,anchors.leftMargin"
                easing.type: Easing.InOutQuad
                duration: 200
            }
        }
    ]

    property int currentFolderIndex: 0

    Rectangle {
        id: authRect
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: parent.width

        Stack {
            flickableDirection: Flickable.VerticalFlick
            id: authStack
            anchors.fill: parent

            PhonePage {

            }

            CodePage {

            }
        }

        Item {
            anchors.top: parent.top
            anchors.left: parent.left
            height: 40
            width: 40
            state: authStack.currentIndex == 0 ? "NO_BACK" : "BACK"
            id: authBackRect

            states: [
                State {
                    name: "NO_BACK"
                    PropertyChanges {
                        target: authBackImage
                        opacity: 0
                        rotation: 180
                    }
                },
                State {
                    name: "BACK"
                    PropertyChanges {
                        target: authBackImage
                        opacity: 1
                        rotation: 0
                    }
                }
            ]

            transitions: [
                Transition {
                    NumberAnimation {
                        properties: "opacity,rotation"
                        easing.type: Easing.InOutQuad
                        duration: 200
                    }
                }
            ]

            Image {
                id: authBackImage
                anchors.centerIn: parent
                source: "../img/arrow-left_black.svg"
                width: 20
                height: 20
                smooth: true
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    authStack.currentIndex = Math.max(0, authStack.currentIndex - 1)
                }
            }
        }

        Item {
            anchors.top: parent.top
            anchors.right: parent.right
            width: settingsText.width + 20
            height: 40
            visible: false

            Text {
                id: settingsText
                anchors.centerIn: parent
                font.family: "Open Sans SemiBold"
                font.pixelSize: 12
                text: "SETTINGS"
            }
        }

        Spinner {
            anchors.top: parent.top
            anchors.right: parent.right
        }
    }

    Rectangle {
        id: mainRect
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: parent.width

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
}
