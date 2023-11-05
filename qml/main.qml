import QtQuick 1.0
import "dialog"
import "message"
import "control"
import "auth"
import Kutegram 1.0

Rectangle {
    //TODO: remove dynamically unused pages / components from memory
    //TODO: keypad navigation
    id: root

    width: 320
    height: 240

    property color globalAccent: "#54759E"

    state: "AUTH"

    states: [
        State {
            name: "AUTH"
            PropertyChanges {
                target: mainScreen
                anchors.leftMargin: root.width
                opacity: 0
            }
        },
        State {
            name: "MAIN"
            PropertyChanges {
                target: mainScreen
                anchors.leftMargin: 0
                opacity: 1
            }
        }
    ]

    transitions: [
        Transition {
            NumberAnimation {
                properties: "opacity,anchors.leftMargin"
                easing.type: Easing.InOutQuad
                duration: 200
            }
        }
    ]

    property int currentFolderIndex: 0
    property bool authProgress: false

    function setAuthProgress(visible) {
        authProgress = visible;
    }

    Component.onCompleted: {
        if (telegramClient.hasSession()) {
            setAuthProgress(true);
            telegramClient.start();
        }
    }

    TgClient {
        id: telegramClient

        onInitialized: {
            if (hasUserId) {
                return;
            }

            setAuthProgress(false);
            authScreen.currentIndex = 1;

            helpGetCountriesList();
        }

        onDisconnected: {
            setAuthProgress(false);

            if (!hasUserId) {
                root.state = "AUTH";
                authScreen.currentIndex = 0;
            } else {
                // TODO: show reconnecting
                // TODO: timer!
                telegramClient.start();
            }
        }

        onAuthSentCodeResponse: {
            setAuthProgress(false);

            authScreen.phonePage.phoneCodeHash = data["phone_code_hash"];
            switch (data["type"]["_"]) {
                //TODO messages
//            case TLType::AuthSentCodeTypeApp:
//                codeNumberDescriptionLabel->setText("A code was sent via Telegram to your other\ndevices, if you have any connected.");
//                break;
//            case TLType::AuthSentCodeTypeSms:
//                codeNumberDescriptionLabel->setText("We've sent an activation code to your phone.\nPlease enter it below.");
//                break;
//            case TLType::AuthSentCodeTypeCall:
//                break;
//            case TLType::AuthSentCodeTypeFlashCall:
//                break;
//            case TLType::AuthSentCodeTypeMissedCall:
//                break;
//            case TLType::AuthSentCodeTypeEmailCode:
//                break;
//            case TLType::AuthSentCodeTypeSetUpEmailRequired:
//                //TODO: show error or implement it lol
//                break;
//            case TLType::AuthSentCodeTypeFragmentSms:
//                break;
//            case TLType::AuthSentCodeTypeFirebaseSms:
//                break;
            }

            authScreen.currentIndex = 2;
        }

        onAuthAuthorizationResponse: {
            setAuthProgress(false);

            if (data["_"] == 0x44747e9a) {
                //TODO sign up / registration support
                snackBar.text = "Sign up isn't supported now. Please, use official app for signing up.";
            }
        }

        onAuthorized: {
            setAuthProgress(false);
            //TODO hide reconnecting

            root.state = "MAIN";
        }

        onRpcError: {
            setAuthProgress(false);

            //TODO think how to improve it
            if (errorMessage == "PHONE_NUMBER_INVALID") {
                snackBar.text = "Invalid phone number. Please try again.";
            }
            else if (errorMessage == "PHONE_NUMBER_FLOOD") {
                snackBar.text = "Phone is used too many times recently.";
            }
            else if (errorMessage == "PHONE_CODE_INVALID") {
                snackBar.text = "You have entered an invalid code.";
            }
            else {
                snackBar.text = "RPC error occured: " + errorMessage + " (" + errorCode + ")"
            }
        }

        onSocketError: {
            if (state == "AUTH") {
                setAuthProgress(false);
                snackBar.text = "Socket error occured: " + errorMessage + " (" + errorCode + ")"
            } else {
                snackBar.text = "Reconnecting...";
            }
        }

        onTfaRequired: {
            setAuthProgress(false);

            //TODO 2fa support
            snackBar.text =  "2FA isn't supported now. You can disable 2FA, log in and enable it afterwards.";
        }

        onHelpCountriesListResponse: {
            //TODO country selector
        }

        //Debug only
//        onFileDownloadCanceled: {
//            console.log("[INFO] File " + fileId + " download canceled");
//        }

//        onFileDownloaded: {
//            console.log("[INFO] File " + fileId + " have been downloaded");
//        }

//        onFileDownloading: {
//            console.log("[INFO] File " + fileId + " download progress: " + processedLength + " / " + totalLength + " " + progressPercentage + " %");
//        }

//        onFileUploadCanceled: {
//            console.log("[INFO] File " + fileId + " upload canceled");
//        }

//        onFileUploaded: {
//            console.log("[INFO] File " + fileId + " have been uploaded");
//        }

//        onFileUploading: {
//            console.log("[INFO] File " + fileId + " upload progress: " + processedLength + " / " + totalLength + " " + progressPercentage + " %");
//        }
    }

    AvatarDownloader {
        id: globalAvatarDownloader
        client: telegramClient
    }

    AuthScreen {
        id: authScreen
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: parent.width
    }

    MainScreen {
        id: mainScreen
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: parent.width
    }

    SnackBar {
        id: snackBar
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }

    Keys.onPressed: {
        if (event.key == Qt.Key_Context1 || event.key == Qt.Key_Escape) {
            topBar.menuButtonClicked();
        }
    }
}
