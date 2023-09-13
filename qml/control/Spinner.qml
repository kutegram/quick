import QtQuick 1.0

Item {
    width: 40 * kgScaling
    height: width

    property bool white: false

    Image {
        asynchronous: true
        anchors.centerIn: parent
        source: white ? "../../img/loading_white.png" : "../../img/loading.png"
        width: 20 * kgScaling
        height: width
        smooth: true

        PropertyAnimation on rotation {
            loops: Animation.Infinite
            from: 0
            to: 360
            easing.type: Easing.InOutQuad
            duration: 1400
        }
    }
}
