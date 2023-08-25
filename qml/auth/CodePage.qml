import QtQuick 1.0
import "../control"

Rectangle {
    property alias error: errorText.text

    Column {
        anchors.centerIn: parent
        width: Math.min(320, parent.width * 2 / 3)
        spacing: 5

        Text {
            anchors.left: parent.left
            anchors.right: parent.right
            text: "Confirmation Code"
            font.family: "Open Sans SemiBold"
            font.pixelSize: 12
            wrapMode: Text.Wrap
        }

        Text {
            anchors.left: parent.left
            anchors.right: parent.right
            text: "Please, enter your confirmation code."
            font.family: "Open Sans"
            font.pixelSize: 12
            wrapMode: Text.Wrap
        }

        Text {
            id: errorText
            anchors.left: parent.left
            anchors.right: parent.right
            text: ""
            font.family: "Open Sans"
            font.pixelSize: 12
            wrapMode: Text.Wrap
            state: "EMPTY"
            onTextChanged: {
                state = text.length == 0 ? "EMPTY" : "NOT_EMPTY"
            }

            states: [
                State {
                    name: "EMPTY"
                    PropertyChanges {
                        target: errorText
                        opacity: 0
                    }
                },
                State {
                    name: "NOT_EMPTY"
                    PropertyChanges {
                        target: errorText
                        opacity: 1
                    }
                }
            ]
            transitions: [
                Transition {
                    NumberAnimation {
                        properties: "opacity,height"
                        easing.type: Easing.InOutQuad
                        duration: 200
                    }
                }
            ]

            Timer {
                id: hideTimer
                interval: 10000
                running: errorText.state == "NOT_EMPTY"
                onTriggered: {
                    errorText.state = "EMPTY";
                }
            }
        }

        LineEdit {
            id: codeEdit
            anchors.left: parent.left
            anchors.right: parent.right
        }

        Button {
            anchors.left: parent.left
            anchors.right: parent.right
            enabled: !root.authProgress
            onClicked: {
                if (codeEdit.text.length == 0) {
                    errorText.text = "You have entered an invalid code.";
                    return;
                }

                errorText.state = "EMPTY";

                setAuthProgress(true);
                telegramClient.authSignIn(phonePage.phoneNumber, phonePage.phoneCodeHash, codeEdit.text);
            }
        }
    }
}
