import QtQuick 1.0

Item {
    width: 160
    height: 40

    state: innerInput.activeFocus ? "FOCUSED" : "NOT_FOCUSED"

    states: [
        State {
            name: "NOT_FOCUSED"
            PropertyChanges {
                target: focusedRect
                width: 0
            }
        },
        State {
            name: "FOCUSED"
            PropertyChanges {
                target: focusedRect
                width: parent.width - 10
            }
        }
    ]

    transitions: [
        Transition {
            NumberAnimation {
                properties: "width"
                easing.type: Easing.InOutQuad
                duration: 200
            }
        }
    ]

    MouseArea {
        anchors.fill: parent
        onClicked: {
            innerInput.forceActiveFocus();
        }
    }

    TextInput {
        id: innerInput
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 5
        anchors.rightMargin: 5
        font.family: "Open Sans"
        font.pixelSize: 12
    }

    Rectangle {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        width: parent.width - 10
        height: 2
        color: "#EEEEEE"
    }

    Rectangle {
        id: focusedRect
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        width: parent.width - 10
        height: 2
        color: "#54759E"
    }
}
