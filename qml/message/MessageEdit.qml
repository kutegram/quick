import QtQuick 1.0
import Kutegram 1.0
import "../control"

Rectangle {
    height: 40
    width: 240
    color: "#FFFFFF"
    id: editRoot

    property alias peer: messageEditor.peer
    property alias messageText: innerEdit.text

    MessageEditor {
        id: messageEditor
        client: telegramClient

        onDraftChanged: {
            messageText = draft;
        }

        onUploadingProgress: {
            if (progress != -1 && progress != 100) {
                attachButton.state = "UPLOADING";
                uploadBar.width = progress * editRoot.width / 100;
            } else {
                attachButton.state = progress != 100 ? "NOT_UPLOADING" : "UPLOADED";
                uploadBar.width = 0;
            }
        }
    }

    MouseArea {
        anchors.fill: parent
    }

    Item {
        id: attachButton
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        width: height
        state: "NOT_UPLOADING"

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (attachButton.state == "NOT_UPLOADING") {
                    messageEditor.uploadFile();
                } else {
                    messageEditor.cancelUpload();
                }
            }
        }

        Image {
            id: attachmentImage
            anchors.centerIn: parent
            width: 20
            height: 20
            smooth: true
            source: "../../img/attachment.png"
            rotation: 135
            asynchronous: true
        }

        Image {
            id: deleteImage
            anchors.centerIn: parent
            width: 20
            height: 20
            smooth: true
            source: "../../img/delete.png"
            asynchronous: true
        }

        Spinner {
            id: uploadSpinner
            anchors.centerIn: parent

            Image {
                anchors.centerIn: parent
                width: 20
                height: 20
                smooth: true
                source: "../../img/close-circle-outline_inner.png"
                asynchronous: true
            }
        }

        states: [
            State {
                name: "UPLOADING"
                PropertyChanges {
                    target: attachmentImage
                    opacity: 0
                    scale: 0
                }
                PropertyChanges {
                    target: deleteImage
                    opacity: 0
                    scale: 0
                }
                PropertyChanges {
                    target: uploadSpinner
                    opacity: 1
                    scale: 1
                }
            },
            State {
                name: "NOT_UPLOADING"
                PropertyChanges {
                    target: attachmentImage
                    opacity: 1
                    scale: 1
                }
                PropertyChanges {
                    target: deleteImage
                    opacity: 0
                    scale: 0
                }
                PropertyChanges {
                    target: uploadSpinner
                    opacity: 0
                    scale: 0
                }
            },
            State {
                name: "UPLOADED"
                PropertyChanges {
                    target: attachmentImage
                    opacity: 0
                    scale: 0
                }
                PropertyChanges {
                    target: deleteImage
                    opacity: 1
                    scale: 1
                }
                PropertyChanges {
                    target: uploadSpinner
                    opacity: 0
                    scale: 0
                }
            }
        ]

        transitions: [
            Transition {
                NumberAnimation {
                    properties: "opacity,scale"
                    easing.type: Easing.InOutQuad
                    duration: 200
                }
            }
        ]
    }

    Item {
        id: sendButton
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: height
        state: messageText.length == 0 && attachButton.state != "UPLOADED" ? "EMPTY" : "NOT_EMPTY"

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (sendButton.state == "NOT_EMPTY") {
                    messageEditor.sendMessage(messageText);
                    messageText = "";
                }
            }
        }

        Image {
            id: sendImage
            anchors.centerIn: parent
            width: 20
            height: 20
            smooth: true
            source: "../../img/send_accent.png"
            asynchronous: true
        }

        Image {
            id: micImage
            anchors.centerIn: parent
            width: 20
            height: 20
            smooth: true
            //source: "../../img/microphone.png"
            source: "../../img/send.png"
            asynchronous: true
        }

        states: [
            State {
                name: "EMPTY"
                PropertyChanges {
                    target: sendImage
                    opacity: 0
                    scale: 0
                }
                PropertyChanges {
                    target: micImage
                    opacity: 1
                    scale: 1
                }
            },
            State {
                name: "NOT_EMPTY"
                PropertyChanges {
                    target: sendImage
                    opacity: 1
                    scale: 1
                }
                PropertyChanges {
                    target: micImage
                    opacity: 0
                    scale: 0
                }
            }
        ]

        transitions: [
            Transition {
                NumberAnimation {
                    properties: "opacity,scale"
                    easing.type: Easing.InOutQuad
                    duration: 200
                }
            }
        ]
    }

    Flickable {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: attachButton.right
        anchors.right: sendButton.left
        anchors.margins: 2
        clip: true
        id: innerFlick

        function ensureVisible(r)
        {
            if (contentX >= r.x)
                contentX = r.x;
            else if (contentX+width <= r.x+r.width)
                contentX = r.x+r.width-width;
            if (contentY >= r.y)
                contentY = r.y;
            else if (contentY+height <= r.y+r.height)
                contentY = r.y+r.height-height;
        }

        TextEdit {
            id: innerEdit
            anchors.fill: parent
            font.pixelSize: 12
            focus: true
            wrapMode: TextEdit.Wrap
            anchors.topMargin: Math.max((parent.height - paintedHeight) / 2, 0)
            onCursorRectangleChanged: innerFlick.ensureVisible(cursorRectangle)
        }

        Text {
            id: messageTip
            anchors.top: innerEdit.top
            anchors.left: parent.left
            font.pixelSize: 12
            color: "#8D8D8D"
            text: "Write a message..."
            state: messageText.length == 0 ? "EMPTY" : "NOT_EMPTY"

            states: [
                State {
                    name: "EMPTY"
                    PropertyChanges {
                        target: messageTip
                        opacity: 1
                        anchors.leftMargin: 0
                    }
                },
                State {
                    name: "NOT_EMPTY"
                    PropertyChanges {
                        target: messageTip
                        opacity: 0
                        anchors.leftMargin: -10
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
        }
    }

    Rectangle {
        id: uploadBar
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        height: 2
        color: "#54759E"
        width: 0

        Behavior on width {
            NumberAnimation {
                easing.type: Easing.InOutQuad
            }
        }
    }
}
