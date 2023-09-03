import QtQuick 1.0
import "../control"

Rectangle {
    width: 240
    height: 40

    property alias image: documentImage.source
    property alias title: documentTitle.text
    property alias text: documentText.text
    property bool downloadable: false
    property int rowIndex: -1
    property int mid: -1

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (attachButton.state == "NOT_UPLOADING") {
                messagesModel.downloadFile(rowIndex);
            } else {
                messagesModel.cancelDownload(rowIndex);
            }
        }
    }

    Component.onCompleted: {
        messagesModel.downloadUpdated.connect(handleDownload);
    }

    function handleDownload(messageId, state) {
        if (messageId != mid) {
            return;
        }

        switch (state) {
        case 1:
            attachButton.state = "UPLOADED";
            break;
        case 0:
            attachButton.state = "UPLOADING";
            break;
        case -1:
            attachButton.state = "NOT_UPLOADING";
            break;
        }
    }

    Rectangle {
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 5
        id: attachButton

        width: 30
        height: 30
        radius: 15
        smooth: true
        color: "#54759E"

        state: downloadable ? "NOT_UPLOADING" : "UPLOADED"

        Image {
            id: downloadImage
            anchors.centerIn: parent
            width: 20
            height: 20
            smooth: true
            source: "../../img/media/download.png"
            asynchronous: true
        }

        Image {
            id: documentImage
            anchors.centerIn: parent
            width: 20
            height: 20
            smooth: true
            asynchronous: true
        }

        Spinner {
            id: uploadSpinner
            anchors.centerIn: parent
            white: true

            Image {
                anchors.centerIn: parent
                width: 20
                height: 20
                smooth: true
                source: "../../img/media/close-circle-outline_inner.png"
                asynchronous: true
            }
        }

        states: [
            State {
                name: "UPLOADING"
                PropertyChanges {
                    target: downloadImage
                    opacity: 0
                    scale: 0
                }
                PropertyChanges {
                    target: documentImage
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
                    target: downloadImage
                    opacity: 1
                    scale: 1
                }
                PropertyChanges {
                    target: documentImage
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
                    target: downloadImage
                    opacity: 0
                    scale: 0
                }
                PropertyChanges {
                    target: documentImage
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

    Column {
        anchors.left: attachButton.right
        anchors.verticalCenter: attachButton.verticalCenter
        anchors.leftMargin: attachButton.anchors.leftMargin

        Row {
            spacing: 5
            Text {
                id: documentTitle
                font.bold: true
                font.pixelSize: 12
            }
        }

        Text {
            id: documentText
            color: "#8D8D8D"
            font.pixelSize: 12
        }
    }
}
