import QtQuick 1.0

Rectangle {
    Column {
        anchors.centerIn: parent
        width: Math.min(320, parent.width * 2 / 3)
        spacing: 5

        Text {
            anchors.left: parent.left
            anchors.right: parent.right
            text: "Kutegram"
            font.bold: true
            font.pixelSize: 12
            wrapMode: Text.Wrap
        }

        Text {
            anchors.left: parent.left
            anchors.right: parent.right
            text: "Just another unofficial Telegram client."
            font.pixelSize: 12
            wrapMode: Text.Wrap
        }

        Button {
            anchors.left: parent.left
            anchors.right: parent.right
            enabled: !root.authProgress
            onClicked: {
                telegramClient.resetSession();
                root.setAuthProgress(true);
                telegramClient.start();
            }
        }
    }
}
