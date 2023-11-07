import QtQuick 1.0

Image {
    height: width
    //height: sourceSize.height * width / sourceSize.width
    source: photoFile.length == 0 ? "" : photoFile + ".thumbnail.jpg"
    clip: true
    asynchronous: true
    smooth: true
    fillMode: Image.PreserveAspectFit

    //TODO loading spinner

    Image {
        id: checkbox
        //TODO: adaptive color
        source: "../../img/checkbox-blank-circle-outline.png"
        smooth: true
        width: 20 * kgScaling
        height: width
        x: 5 * kgScaling
        y: 5 * kgScaling
        asynchronous: true
    }

    Rectangle {
        id: spoilerRect
        visible: photoSpoiler
        anchors.fill: parent
        color: "gray"
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (spoilerRect.visible) {
                spoilerRect.visible = false;
                return;
            }

            imageViewer.imageSource = photoFile;
            imageViewer.state = "OPENED";
        }
    }

    transitions: [
        Transition {
            NumberAnimation {
                properties: "x,opacity"
                easing.type: Easing.InOutQuad
                duration: 200
            }
        }
    ]

    states: [
        State {
            name: "NO_SELECT"
            PropertyChanges {
                target: checkbox
                opacity: 0
                x: -15 * kgScaling
            }
        },
        State {
            name: "SHOW_SELECT"
            PropertyChanges {
                target: checkbox
                opacity: 1
                x: 5 * kgScaling
            }
        }
    ]
}
