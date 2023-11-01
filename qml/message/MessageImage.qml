import QtQuick 1.0

Image {
    property url image

    //TODO use photo size
    height: 100 * kgScaling
    width: 100 * kgScaling
    source: image
    clip: true
    asynchronous: true
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

    MouseArea {
        anchors.fill: parent
        onClicked: {
            imageViewer.imageSource = image;
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
