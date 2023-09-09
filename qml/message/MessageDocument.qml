import QtQuick 1.0
import "../control"

Rectangle {
    width: 240 * kgScaling
    height: 40 * kgScaling

    property alias image: documentImage.source
    property alias title: documentTitle.text
    property alias text: documentText.text
    property bool downloadable: false
    property int rowIndex: -1
    property int mid: -1

    MouseArea {
        anchors.fill: parent
        enabled: downloadable
        onClicked: {
            if (attachButton.state == "NOT_DOWNLOADING") {
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
            attachButton.state = "DOWNLOADED";
            break;
        case 0:
            attachButton.state = "DOWNLOADING";
            break;
        case -1:
            attachButton.state = "NOT_DOWNLOADING";
            break;
        }
    }

    Rectangle {
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: 5
        id: attachButton

        width: 30 * kgScaling
        height: width
        radius: width / 2
        smooth: true
        color: "#54759E"

        state: downloadable ? "NOT_DOWNLOADING" : "DOWNLOADED"

        Image {
            id: downloadImage
            anchors.centerIn: parent
            width: 20 * kgScaling
            height: width
            smooth: true
            source: "../../img/media/download.png"
            asynchronous: true
        }

        Image {
            id: documentImage
            anchors.centerIn: parent
            width: 20 * kgScaling
            height: width
            smooth: true
            asynchronous: true
        }

        Spinner {
            id: uploadSpinner
            anchors.centerIn: parent
            white: true

            Image {
                anchors.centerIn: parent
                width: 20 * kgScaling
                height: width
                smooth: true
                source: "../../img/media/close-circle-outline_inner.png"
                asynchronous: true
            }
        }

        states: [
            State {
                name: "DOWNLOADING"
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
                name: "NOT_DOWNLOADING"
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
                name: "DOWNLOADED"
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
            spacing: 5 * kgScaling
            Text {
                id: documentTitle
                font.bold: true
            }
        }

        Text {
            id: documentText
            color: "#8D8D8D"
        }
    }
}
