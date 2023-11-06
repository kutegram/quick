import QtQuick 1.0

Rectangle {
    id: introPageRect

    Column {
        anchors.centerIn: parent
        width: parent.width * 2 / 3
        spacing: 5

        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            width: Math.min(introPageRect.height / 2, parent.width * 2 / 3)
            height: width
            color: globalAccent
            radius: width / 4

            Image {
                anchors.fill: parent
                asynchronous: true
                smooth: true
                source: "../../kutegramquick_big.png"
            }
        }

        Text {
            anchors.left: parent.left
            anchors.right: parent.right
            text: "Kutegram"
            font.bold: true
            wrapMode: Text.Wrap
            horizontalAlignment: Text.AlignHCenter
        }

        Text {
            anchors.left: parent.left
            anchors.right: parent.right
            text: "Just another unofficial Telegram client."
            wrapMode: Text.Wrap
            horizontalAlignment: Text.AlignHCenter
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
