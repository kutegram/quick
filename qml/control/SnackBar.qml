import QtQuick 1.0

Item {
    width: 200
    height: 40

    property alias text: snackText.text

    function close() {
        snackText.state = "EMPTY";
    }

    Rectangle {
        id: snackRect

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 4

        height: 32
        radius: 4
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
            anchors.leftMargin: 8
            anchors.rightMargin: 8
            color: "#FFFFFF"
            text: ""

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
                        anchors.bottomMargin: -40
                    }
                },
                State {
                    name: "NOT_EMPTY"
                    PropertyChanges {
                        target: snackRect
                        anchors.bottomMargin: 4
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
