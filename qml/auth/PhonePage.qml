import QtQuick 1.0
import "../control"

Rectangle {
    property string phoneNumber;
    property string phoneCodeHash;

    Column {
        anchors.centerIn: parent
        width: parent.width * 2 / 3
        spacing: 5

        Text {
            anchors.left: parent.left
            anchors.right: parent.right
            text: "Your Phone Number"
            font.bold: true
            wrapMode: Text.Wrap
        }

        Text {
            anchors.left: parent.left
            anchors.right: parent.right
            text: "Please, enter your phone number."
            wrapMode: Text.Wrap
        }

        LineEdit {
            id: phoneEdit
            anchors.left: parent.left
            anchors.right: parent.right
        }

        Button {
            anchors.left: parent.left
            anchors.right: parent.right
            enabled: !root.authProgress
            onClicked: {
                if (phoneEdit.text.length == 0) {
                    snackBar.text = "Invalid phone number. Please try again.";
                    return;
                }

                snackBar.close();

                phoneNumber = phoneEdit.text;
                phoneNumber.replace(' ', "");
                phoneNumber.replace('-', "");

                phoneCodeHash = "";

                setAuthProgress(true);
                telegramClient.authSendCode(phoneNumber);
            }
        }
    }
}
