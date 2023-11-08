import QtQuick 1.0
import "../control"

Rectangle {
    width: 240 * kgScaling
    height: 40 * kgScaling

    property int rowIndex: -1
    property string filePath: ""

    Component.onCompleted: {
        messagesModel.downloadUpdated.connect(handleDownload);
    }

    function handleDownload(mid, state, path) {
        if (mid != messageId) {
            return;
        }

        filePath = path;

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
        color: globalAccent

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
                font.pixelSize: 12 * kgScaling
            }
        }

        Text {
            text: mediaText
            color: "#8D8D8D"
            font.pixelSize: 12 * kgScaling
        }
    }

    Rectangle {
        id: spoilerRect
        visible: mediaSpoiler
        anchors.fill: parent
        color: "gray"

        Text {
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 12 * kgScaling
            color: "white"
            text: "Media is hidden.\nClick to reveal."
        }
    }

    MouseArea {
        anchors.fill: parent
        enabled: spoilerRect.visible || mediaDownloadable || mediaUrl.length != 0
        onClicked: {
            if (spoilerRect.visible) {
                spoilerRect.visible = false;
                return;
            }

            if (mediaUrl.length != 0) {
                messagesModel.openUrl(mediaUrl);
                return;
            }

            if (attachButton.state == "NOT_DOWNLOADING") {
                messagesModel.downloadFile(rowIndex);
            } else if (attachButton.state == "DOWNLOADED" && filePath.length != 0) {
                messagesModel.openUrl(filePath);
            } else {
                messagesModel.cancelDownload(rowIndex);
            }
        }
    }
}
