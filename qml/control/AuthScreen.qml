import QtQuick 1.0
import "../dialog"
import "../message"
import "../control"
import "../auth"

Rectangle {
    id: authScreen
    property int currentIndex: 0
    state: "INTRO"

    onCurrentIndexChanged: {
        if (currentIndex == 0)
            state = "INTRO";
        if (currentIndex == 1)
            state = "PHONE";
        if (currentIndex == 2)
            state = "CODE";
    }

    property alias phonePage: phonePage

    states: [
        State {
            name: "INTRO"
            PropertyChanges {
                target: introPage
                anchors.leftMargin: 0
                opacity: 1
            }
            PropertyChanges {
                target: phonePage
                anchors.leftMargin: width
                opacity: 0
            }
            PropertyChanges {
                target: codePage
                anchors.leftMargin: width
                opacity: 0
            }
        },
        State {
            name: "PHONE"
            PropertyChanges {
                target: introPage
                anchors.leftMargin: -width
                opacity: 0
            }
            PropertyChanges {
                target: phonePage
                anchors.leftMargin: 0
                opacity: 1
            }
            PropertyChanges {
                target: codePage
                anchors.leftMargin: width
                opacity: 0
            }
        },
        State {
            name: "CODE"
            PropertyChanges {
                target: introPage
                anchors.leftMargin: -width
                opacity: 0
            }
            PropertyChanges {
                target: phonePage
                anchors.leftMargin: -width
                opacity: 0
            }
            PropertyChanges {
                target: codePage
                anchors.leftMargin: 0
                opacity: 1
            }
        }
    ]

    transitions: [
        Transition {
            NumberAnimation {
                properties: "anchors.leftMargin,opacity"
                easing.type: Easing.InOutQuad
                duration: 200
            }
        }
    ]

    IntroPage {
        id: introPage
        anchors.left: parent.left
        width: parent.width
        height: parent.height
    }

    PhonePage {
        id: phonePage
        anchors.left: parent.left
        width: parent.width
        height: parent.height
    }

    CodePage {
        id: codePage
        anchors.left: parent.left
        width: parent.width
        height: parent.height
    }

    Item {
        anchors.top: parent.top
        anchors.left: parent.left
        width: 40 * kgScaling
        height: width
        state: currentIndex == 0 ? "NO_BACK" : "BACK"
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
            source: "../../img/arrow-left_black.png"
            width: 20 * kgScaling
            height: width
            smooth: true
            asynchronous: true
        }

        MouseArea {
            anchors.fill: parent
            onClicked: {
                setAuthProgress(false);
                authScreen.currentIndex = Math.max(0, authScreen.currentIndex - 1)
            }
        }
    }

    Item {
        anchors.top: parent.top
        anchors.right: parent.right
        width: settingsText.width + 20 * kgScaling
        height: 40 * kgScaling
        visible: false

        Text {
            id: settingsText
            anchors.centerIn: parent
            font.bold: true
            text: "SETTINGS"
        }
    }

    Spinner {
        id: authSpinner
        visible: root.authProgress
        anchors.top: parent.top
        anchors.right: parent.right
    }
}
