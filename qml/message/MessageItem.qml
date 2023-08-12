import QtQuick 1.0

Item {
    property alias currentState: messageRoot.state

    id: messageRoot
    width: ListView.view.width
    height: textContainer.height
    state: "NO_SELECT"

    states: [
        State {
            name: "NO_SELECT"
            PropertyChanges {
                target: checkbox
                opacity: 0
                x: -15
            }
        },
        State {
            name: "SHOW_SELECT"
            PropertyChanges {
                target: checkbox
                opacity: 1
                x: 5
            }
        }
    ]

    transitions: [
        Transition {
            to: "*"
            NumberAnimation {
                properties: "x,opacity"
                easing.type: Easing.InOutQuad
                duration: 200
            }
        }
    ]

    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (ListView.view.parent.globalState == "SHOW_SELECT")
                ListView.view.parent.globalState = "NO_SELECT"
            else ListView.view.parent.globalState = "SHOW_SELECT"
        }
    }

    //TODO: use peer object
    property double _senderId: senderId;
    property double _previousSenderId: previousSenderId;

    Image {
        id: checkbox
        source: "../../img/checkbox-blank-circle-outline.svg"
        smooth: true
        height: 20
        width: 20
        x: 5
        y: 5
    }

    Rectangle {
        id: messageAvatar
        anchors.top: parent.top
        anchors.left: checkbox.right
        anchors.leftMargin: 5
        anchors.topMargin: 5
        visible: _previousSenderId != _senderId
        width: 30
        height: 30
        radius: 15
        smooth: true
        color: "#FFA3A3"

        Text {
            anchors.fill: parent
            text: "JP"
            color: "#FFFFFF"
            font.family: "Open Sans SemiBold"
            font.pixelSize: 12
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }

    Column {
        id: textContainer
        anchors.top: parent.top
        anchors.left: messageAvatar.right
        anchors.right: parent.right
        anchors.topMargin: 5
        anchors.leftMargin: 5
        anchors.rightMargin: 5

        Row {
            spacing: 5

            Text {
                id: messageSender
                text: "Just Piggy"
                font.family: "Open Sans SemiBold"
                font.pixelSize: 12
                visible: _previousSenderId != _senderId
            }

            Text {
                id: messageTimestamp
                anchors.bottom: parent.bottom
                text: "09:35"
                font.family: "Open Sans SemiBold"
                font.pixelSize: 10
                color: "#999999"
                visible: _previousSenderId != _senderId
            }
        }

        Text {
            anchors.left: parent.left
            anchors.right: parent.right

            id: messageText
            wrapMode: Text.Wrap
            text: "Consectetur et doloribus beatae eius maxime. Error qui placeat et architecto consequatur. Dolorem qui amet necessitatibus. Hic non et sapiente minima nihil veritatis. Repellendus quia id ducimus. Architecto vero temporibus et dolorum."
            font.family: "Open Sans"
            font.pixelSize: 12
        }
    }
}
