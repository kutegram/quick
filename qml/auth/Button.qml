import QtQuick 1.0

Item {
    id: buttonRoot
    signal clicked()

    property bool enabled: true

    width: 160
    height: 40

    Text {
        anchors.centerIn: parent
        text: "Next"
        font.family: "Open Sans SemiBold"
        font.pixelSize: 12
        //color: "#FFFFFF"
    }

    Rectangle {
        anchors.fill: parent
        radius: 5
        color: "#000000"
        opacity: innerArea.activeFocus ? 0.2 : 0
    }

    MouseArea {
        id: innerArea
        anchors.fill: parent
        enabled: buttonRoot.enabled
    }

    Component.onCompleted: {
        innerArea.clicked.connect(buttonRoot.clicked);
    }
}
