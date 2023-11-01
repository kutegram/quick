import QtQuick 1.0

Item {
    id: imageViewer
    property int scaling: Math.min(100, Math.min(Math.floor(imageViewer.width / innerImage.sourceSize.width * 100), Math.floor(imageViewer.height / innerImage.sourceSize.height * 100)))

    visible: opacity != 0

    //TODO loading spinner

    state: "CLOSED"
    states: [
        State {
            name: "OPENED"
            PropertyChanges {
                target: imageViewer
                opacity: 1
            }
        },
        State {
            name: "CLOSED"
            PropertyChanges {
                target: imageViewer
                opacity: 0
            }
        }
    ]

    Behavior on opacity {
        NumberAnimation {
            easing.type: Easing.InOutQuad
        }
    }

    property url imageSource

    onWidthChanged: {
        changeTimer.restart();
    }

    Timer {
        id: changeTimer
        interval: 200
        repeat: false
        onTriggered: {
            scaling = Math.min(100, Math.min(Math.floor(imageViewer.width / innerImage.sourceSize.width * 100), Math.floor(imageViewer.height / innerImage.sourceSize.height * 100)));
            flickable.contentX = 0;
            flickable.contentY = 0;
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "black"
        opacity: 0.8
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            imageViewer.state = "CLOSED"
        }
    }

    Flickable {
        id: flickable
        anchors.centerIn: parent

        width: Math.min(parent.width, innerImage.width)
        height: Math.min(parent.height, innerImage.height)

        contentWidth: innerImage.width
        contentHeight: innerImage.height

        //TODO think about image center drift on first zoom
        //TODO protect contentX and contentY from spam zooming

        Behavior on contentX {
            enabled: !flickable.flicking
            NumberAnimation {
                easing.type: Easing.InOutQuad
            }
        }

        Behavior on contentY {
            enabled: !flickable.flicking
            NumberAnimation {
                easing.type: Easing.InOutQuad
            }
        }

        Image {
            id: innerImage
            source: imageSource
            smooth: true

            width: sourceSize.width * scaling / 100
            height: sourceSize.height * scaling / 100

            Behavior on width {
                NumberAnimation {
                    easing.type: Easing.InOutQuad
                }
            }

            Behavior on height {
                NumberAnimation {
                    easing.type: Easing.InOutQuad
                }
            }
        }
    }

    Rectangle {
        id: topLeftRect
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.leftMargin: 10 * kgScaling
        anchors.topMargin: 10 * kgScaling
        height: 40 * kgScaling
        width: 40 * kgScaling
        color: "black"
        opacity: 0.5
        radius: 10 * kgScaling
        smooth: true

        MouseArea {
            anchors.fill: parent
            onClicked: {
                imageViewer.state = "CLOSED"
            }
        }
    }

    Image {
        anchors.centerIn: topLeftRect
        width: 20 * kgScaling
        height: width
        smooth: true
        source: "../../img/arrow-left.png"
        asynchronous: true
    }

    Rectangle {
        id: topRightRect
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: 10 * kgScaling
        anchors.topMargin: 10 * kgScaling
        height: 40 * kgScaling
        width: 40 * kgScaling
        color: "black"
        opacity: 0.5
        radius: 10 * kgScaling
        smooth: true

        MouseArea {
            anchors.fill: parent
            onClicked: {
                scaling = Math.min(100, Math.min(Math.floor(imageViewer.width / innerImage.sourceSize.width * 100), Math.floor(imageViewer.height / innerImage.sourceSize.height * 100)));
                flickable.contentX = 0;
                flickable.contentY = 0;
            }
        }
    }

    Image {
        anchors.centerIn: topRightRect
        width: 20 * kgScaling
        height: width
        smooth: true
        source: "../../img/fullscreen.png"
        asynchronous: true
    }

    Rectangle {
        id: bottomLeftRect
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.leftMargin: 10 * kgScaling
        anchors.bottomMargin: 10 * kgScaling
        height: 40 * kgScaling
        width: 40 * kgScaling
        color: "black"
        opacity: 0.5
        radius: 10 * kgScaling
        smooth: true

        MouseArea {
            anchors.fill: parent
            onClicked: {
                var newScaling = Math.ceil(scaling / 25) * 25;

                if (scaling > 300)
                    newScaling -= 100;
                else if (scaling > 100)
                    newScaling -= 50;
                else if (scaling > 25)
                    newScaling -= 25;

                flickable.contentX = (flickable.contentX + imageViewer.width / 2) * newScaling / scaling
                        - imageViewer.width / 2;
                flickable.contentY = (flickable.contentY + imageViewer.height / 2) * newScaling / scaling
                        - imageViewer.height / 2;


                scaling = newScaling;
            }
        }
    }

    Image {
        anchors.centerIn: bottomLeftRect
        width: 20 * kgScaling
        height: width
        smooth: true
        source: "../../img/magnify-minus-outline.png"
        asynchronous: true
    }

    Rectangle {
        id: bottomRightRect
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: 10 * kgScaling
        anchors.bottomMargin: 10 * kgScaling
        height: 40 * kgScaling
        width: 40 * kgScaling
        color: "black"
        opacity: 0.5
        radius: 10 * kgScaling
        smooth: true

        MouseArea {
            anchors.fill: parent
            onClicked: {
                var newScaling = Math.ceil(scaling / 25) * 25;

                if (scaling < 100)
                    newScaling += 25;
                else if (scaling < 300)
                    newScaling += 50;
                else if (scaling < 800)
                    newScaling += 100;

                flickable.contentX = (flickable.contentX + imageViewer.width / 2) * newScaling / scaling
                        - imageViewer.width / 2;
                flickable.contentY = (flickable.contentY + imageViewer.height / 2) * newScaling / scaling
                        - imageViewer.height / 2;

                scaling = newScaling;
            }
        }
    }

    Image {
        anchors.centerIn: bottomRightRect
        width: 20 * kgScaling
        height: width
        smooth: true
        source: "../../img/magnify-plus-outline.png"
        asynchronous: true
    }

    Rectangle {
        id: bottomCenterRect
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: (60 * kgScaling - height) / 2
        height: 16 * kgScaling + innerText.height
        width: 16 * kgScaling + innerText.width
        color: "black"
        opacity: 0.5
        radius: 8 * kgScaling
        smooth: true
    }

    Text {
        property int scalingAnimated: scaling

        Behavior on scalingAnimated {
            NumberAnimation {
                easing.type: Easing.InOutQuad
            }
        }

        id: innerText
        anchors.centerIn: bottomCenterRect
        text: scalingAnimated + "%"
        color: "white"
        font.pixelSize: 14 * kgScaling
    }
}

