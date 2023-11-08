import QtQuick 1.0

Item {
    width: ListView.view.width
    height: 40 * kgScaling

    Item {
        id: actionIcon
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        width: height

        Image {
            anchors.centerIn: parent
            asynchronous: true
            source: icon
            smooth: true
            width: 20 * kgScaling
            height: width
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            switch (index) {
            case 0:
                telegramClient.resetSession();
                break;
            case 1:
                Qt.quit();
                break;
            }
        }
    }

    Text {
        anchors.left: actionIcon.right
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        text: name
        font.pixelSize: 12 * kgScaling
    }
}
