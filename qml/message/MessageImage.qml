import QtQuick 1.0

Image {
    width: 120
    height: ListView.view.height
    source: image
    clip: true
    asynchronous: true
    fillMode: Image.PreserveAspectCrop

    Image {
        id: checkbox
        //TODO: adaptive color
        source: "../../img/checkbox-blank-circle-outline.svg"
        smooth: true
        height: 20
        width: 20
        x: 5
        y: 5
    }

    transitions: [
        Transition {
            to: "*"
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
                x: -15
            }
        },
        State {
            name: "SHOW_SELECT"
            PropertyChanges {
                target: checkbox
                opacity: 1
                x: 5
            }
        }
    ]
}
