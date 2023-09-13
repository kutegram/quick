import QtQuick 1.0
import "../control"

Rectangle {
    width: 240 * kgScaling
    height: 40 * kgScaling

    property int rowIndex: -1

    MouseArea {
        anchors.fill: parent
        enabled: mediaDownloadable || mediaUrl.length != 0
        onClicked: {
            if (mediaUrl.length != 0) {
                messagesModel.openUrl(mediaUrl);
                return;
            }

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

    function handleDownload(mid, state) {
        if (mid != messageId) {
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
        anchors.leftMargin: 5 * kgScaling
        id: attachButton

        width: 30 * kgScaling
        height: width
        radius: width / 2
        smooth: true
        color: "#54759E"

        state: mediaDownloadable ? "NOT_DOWNLOADING" : "DOWNLOADED"

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
            source: mediaImage
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
                text: mediaTitle
                font.bold: true
            }
        }

        Text {
            text: mediaText
            color: "#8D8D8D"
        }
    }
}
