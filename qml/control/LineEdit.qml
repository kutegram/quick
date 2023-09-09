import QtQuick 1.0

Item {
    width: 160 * kgScaling
    height: 40 * kgScaling

    property alias text: innerInput.text

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
                width: parent.width - 10 * kgScaling
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
        anchors.leftMargin: 5 * kgScaling
        anchors.rightMargin: 5 * kgScaling
    }

    Rectangle {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        width: parent.width - 10 * kgScaling
        height: 2 * kgScaling
        color: "#999999"
    }

    Rectangle {
        id: focusedRect
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        width: parent.width - 10 * kgScaling
        height: 2 * kgScaling
        color: "#54759E"
    }
}
