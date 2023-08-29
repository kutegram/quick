import QtQuick 1.0

Item {
    property bool opened: false

    id: root
    width: 240
    height: 320

    state: opened ? "OPENED" : "CLOSED"

    states: [
        State {
            name: "OPENED"
            PropertyChanges {
                target: dimmBackground
                opacity: 0.5
            }
            PropertyChanges {
                target: drawerSlide
                currentIndex: 0
                anchors.leftMargin: 0
            }
        },
        State {
            name: "CLOSED"
            PropertyChanges {
                target: dimmBackground
                opacity: 0
            }
            PropertyChanges {
                target: drawerSlide
                anchors.leftMargin: -root.width
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

    Rectangle {
        id: dimmBackground
        anchors.fill: parent
        color: "#000000"
    }

    ListView {
        id: drawerSlide
        anchors.left: parent.left
        width: root.width
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        boundsBehavior: Flickable.StopAtBounds
        orientation: ListView.Horizontal
        snapMode: ListView.SnapOneItem
        highlightRangeMode: ListView.StrictlyEnforceRange
        highlightFollowsCurrentItem: true
        highlightMoveDuration: 200

        onCurrentItemChanged: {
            if (currentIndex == 1) {
                topBar.currentState = "MENU"
                opened = false;
            }
        }

        model: VisualItemModel {
            Item {
                id: drawerContent
                width: root.width
                height: root.height

                MouseArea {
                    id: dimmMouseArea
                    anchors.fill: parent

                    onClicked: {
                        topBar.currentState = "MENU"
                        opened = false;
                    }
                }

                Rectangle {
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.bottom: parent.bottom
                    width: Math.min(parent.width * 2 / 3, 280)
                    color: "#FFFFFF"

                    ListView {
                        anchors.fill: parent
                        //model: 5
                        delegate: DrawerButton {

                        }
                    }
                }
            }
            Item {
                id: drawerSpace
                width: root.width
                height: root.height
            }
        }
    }
}
