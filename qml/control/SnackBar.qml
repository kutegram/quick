import QtQuick 1.0

Item {
    id: snackRoot
    width: 200 * kgScaling
    height: snackText.height + 24 * kgScaling

    property alias text: snackText.text

    function close() {
        snackText.state = "EMPTY";
    }

    Rectangle {
        id: snackRect

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 4 * kgScaling

        height: snackText.height + 16 * kgScaling
        radius: 4 * kgScaling
        color: "#323232"

        MouseArea {
            anchors.fill: parent
            //TODO swipe close gesture
            onClicked: {
                close();
            }
        }

        Text {
            id: snackText
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 8 * kgScaling
            anchors.rightMargin: 8 * kgScaling
            color: "#FFFFFF"
            text: ""
            font.pixelSize: 12 * kgScaling

            wrapMode: Text.Wrap
            state: "EMPTY"
            onTextChanged: {
                state = snackText.text.length == 0 ? "EMPTY" : "NOT_EMPTY"
            }

            states: [
                State {
                    name: "EMPTY"
                    PropertyChanges {
                        target: snackRect
                        anchors.bottomMargin: -snackRoot.height
                    }
                },
                State {
                    name: "NOT_EMPTY"
                    PropertyChanges {
                        target: snackRect
                        anchors.bottomMargin: 4 * kgScaling
                    }
                }
            ]
            transitions: [
                Transition {
                    NumberAnimation {
                        properties: "anchors.bottomMargin"
                        easing.type: Easing.InOutQuad
                        duration: 200
                    }
                },
                Transition {
                    to: "EMPTY"
                    SequentialAnimation {
                        NumberAnimation {
                            properties: "anchors.bottomMargin"
                            easing.type: Easing.InOutQuad
                            duration: 200
                        }
                        PropertyAction {
                            target: snackText
                            property: "text"
                            value: ""
                        }
                    }
                }
            ]

            Timer {
                id: hideTimer
                interval: 5000
                running: snackText.state == "NOT_EMPTY"
                onTriggered: {
                    close();
                }
            }
        }
    }
}
