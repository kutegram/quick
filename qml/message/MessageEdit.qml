import QtQuick 1.0

Rectangle {
    height: 40
    width: 240
    color: "#FFFFFF"

    MouseArea {
        anchors.fill: parent
    }

    Item {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        width: height
        id: attachButton

        MouseArea {
            anchors.fill: parent
        }

        Image {
            anchors.centerIn: parent
            width: 20
            height: 20
            smooth: true
            source: "../../img/attachment.svg"
            rotation: 135
        }
    }

    Item {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        width: height
        id: sendButton
        state: innerEdit.text.length == 0 ? "EMPTY" : "NOT_EMPTY"

        MouseArea {
            anchors.fill: parent
        }

        Image {
            id: sendImage
            anchors.centerIn: parent
            width: 20
            height: 20
            smooth: true
            source: "../../img/send.svg"
        }

        Image {
            id: micImage
            anchors.centerIn: parent
            width: 20
            height: 20
            smooth: true
            source: "../../img/microphone.svg"
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
            font.family: "Open Sans"
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
            font.family: "Open Sans"
            font.pixelSize: 12
            color: "#8D8D8D"
            text: "Write a message..."
            state: innerEdit.text.length == 0 ? "EMPTY" : "NOT_EMPTY"

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
}
