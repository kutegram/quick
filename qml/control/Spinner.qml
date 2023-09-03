import QtQuick 1.0

Item {
    width: 40
    height: width

    property bool white: false

    Image {
        anchors.centerIn: parent
        source: white ? "../../img/loading_white.png" : "../../img/loading.png"
        width: 20
        height: 20
        smooth: true

        SequentialAnimation on rotation {
            loops: Animation.Infinite
            PropertyAnimation {
                from: 0
                to: 360
                easing.type: Easing.InOutQuad
                duration: 1400
            }
        }
    }
}
