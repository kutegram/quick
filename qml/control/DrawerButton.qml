import QtQuick 1.0

Item {
    width: ListView.view.width
    height: 40

    Item {
        id: actionIcon
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        width: height

        Image {
            anchors.centerIn: parent
            source: icon
            smooth: true
            height: 20
            width: 20
        }
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            switch (index) {
            case 0:
                dialogPage.refresh();
                break;
            case 1:
                telegramClient.resetSession();
                break;
            case 2:
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
        text: name
        font.pixelSize: 12
    }
}
