import QtQuick

Item {
    anchors.fill: parent

    Rectangle {
        id: panel
        anchors.centerIn: parent
        width: 420
        // input row + separator + results rows + vertical padding
        height: 40 + 1 + keyTree.searchResults.length * 34 + 24
        radius: 12
        color: "#E01A1A2E"
        border.color: "#4488DD"
        border.width: 1

        Column {
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                margins: 12
            }
            spacing: 0

            // Input row
            Row {
                width: parent.width
                height: 40
                spacing: 8

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "/"
                    color: "#88CCFF"
                    font.pixelSize: 20
                    font.bold: true
                }

                Row {
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 2
                    clip: true
                    width: parent.width - 32

                    Text {
                        text: keyTree.searchText
                        color: "white"
                        font.pixelSize: 15
                        font.family: "monospace"
                    }

                    Rectangle {
                        width: 2
                        height: 17
                        anchors.verticalCenter: parent.verticalCenter
                        color: "#88CCFF"

                        SequentialAnimation on opacity {
                            loops: Animation.Infinite
                            NumberAnimation { to: 0; duration: 500 }
                            NumberAnimation { to: 1; duration: 100 }
                        }
                    }
                }
            }

            // Divider
            Rectangle {
                width: parent.width
                height: 1
                color: "#4488DD"
                opacity: 0.5
            }

            // Results
            Repeater {
                model: keyTree.searchResults

                delegate: Rectangle {
                    required property string modelData
                    required property int    index

                    width: panel.width - 24
                    height: 34
                    radius: 5
                    color: index === keyTree.searchSelection ? "#774488DD" : "transparent"

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 8
                        text: modelData
                        color: index === keyTree.searchSelection ? "white" : "#CCDDFF"
                        font.pixelSize: 13
                        font.family: "monospace"
                    }
                }
            }
        }
    }
}
