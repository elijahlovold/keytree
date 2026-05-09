import QtQuick

Item {
    anchors.fill: parent

    property real fontScale: 1.0

    readonly property real inputH:  Math.round(40 * fontScale)
    readonly property real resultH: Math.round(34 * fontScale)

    Rectangle {
        id: panel
        anchors.centerIn: parent
        width:  Math.round(420 * fontScale)
        height: inputH + 1 + keyTree.searchResults.length * resultH + Math.round(24 * fontScale)
        radius: Math.round(12 * fontScale)
        color: "#E01A1A2E"
        border.color: "#4488DD"
        border.width: 1

        Column {
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                margins: Math.round(12 * fontScale)
            }
            spacing: 0

            // Input row
            Row {
                width: parent.width
                height: inputH
                spacing: Math.round(8 * fontScale)

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "/"
                    color: "#88CCFF"
                    font.pixelSize: Math.round(20 * fontScale)
                    font.bold: true
                }

                Row {
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 2
                    clip: true
                    width: parent.width - Math.round(32 * fontScale)

                    Text {
                        text: keyTree.searchText
                        color: "white"
                        font.pixelSize: Math.round(15 * fontScale)
                        font.family: "monospace"
                    }

                    Rectangle {
                        width: Math.max(2, Math.round(2 * fontScale))
                        height: Math.round(17 * fontScale)
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

                    width:  panel.width - Math.round(24 * fontScale)
                    height: resultH
                    radius: Math.round(5 * fontScale)
                    color: index === keyTree.searchSelection ? "#774488DD" : "transparent"

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: Math.round(8 * fontScale)
                        text: modelData
                        color: index === keyTree.searchSelection ? "white" : "#CCDDFF"
                        font.pixelSize: Math.round(13 * fontScale)
                        font.family: "monospace"
                    }
                }
            }
        }
    }
}
