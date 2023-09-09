import QtQuick 1.0

Image {
    height: ListView.view.height
    width: height
    source: image
    clip: true
    asynchronous: true
    fillMode: Image.PreserveAspectCrop

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
