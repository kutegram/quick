import QtQuick 1.0

Item {
    id: buttonRoot
    signal clicked()

    property bool enabled: true

    width: buttonText.width + 40 * kgScaling
    height: 40 * kgScaling

    Rectangle {
        anchors.fill: parent
        radius: 5
        color: globalAccent
    }

    Text {
        id: buttonText
        anchors.centerIn: parent
        text: "Next"
        font.bold: true
        color: "#FFFFFF"
    }

    MouseArea {
        id: innerArea
        anchors.fill: parent
        enabled: buttonRoot.enabled
    }

    Component.onCompleted: {
        innerArea.clicked.connect(buttonRoot.clicked);
    }

    Rectangle {
        anchors.fill: parent
        radius: 5 * kgScaling
        color: "#000000"
        opacity: 0.1
        visible: activeFocus
    }
}
