import QtQuick 1.0

Item {
    id: buttonRoot
    signal clicked()

    property bool enabled: true

    width: 160
    height: 40

    Rectangle {
        anchors.fill: parent
        radius: 5
        color: "#54759E"
    }

    Text {
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
        radius: 5
        color: "#000000"
        opacity: 0.1
        visible: activeFocus
    }
}
