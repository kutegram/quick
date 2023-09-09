import QtQuick 1.0
import "../control"

Rectangle {

    Column {
        anchors.centerIn: parent
        width: Math.min(320, parent.width * 2 / 3)
        spacing: 5

        Text {
            anchors.left: parent.left
            anchors.right: parent.right
            text: "Confirmation Code"
            font.bold: true
            wrapMode: Text.Wrap
        }

        Text {
            anchors.left: parent.left
            anchors.right: parent.right
            text: "Please, enter your confirmation code."
            wrapMode: Text.Wrap
        }

        LineEdit {
            id: codeEdit
            anchors.left: parent.left
            anchors.right: parent.right
        }

        Button {
            anchors.left: parent.left
            anchors.right: parent.right
            enabled: !root.authProgress
            onClicked: {
                if (codeEdit.text.length == 0) {
                    snackBar.text = "You have entered an invalid code.";
                    return;
                }

                snackBar.close();

                setAuthProgress(true);
                telegramClient.authSignIn(phonePage.phoneNumber, phonePage.phoneCodeHash, codeEdit.text);
            }
        }
    }
}
