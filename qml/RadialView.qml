import QtQuick

Item {
    id: root

    property var items: []

    readonly property real cx: width  / 2
    readonly property real cy: height / 2
    readonly property real r:  Math.min(width, height) * 0.38

    // Center dot — indicates active subtree depth
    Rectangle {
        x: root.cx - 5; y: root.cy - 5
        width: 10; height: 10; radius: 5
        color: "#5588CC"
        opacity: 0.85
    }

    Repeater {
        model: root.items

        delegate: Item {
            id: node

            required property var    modelData
            required property int    index

            // Distribute children evenly around the circle, top-anchored
            readonly property real angle: root.items.length === 1
                ? -Math.PI / 2
                : -Math.PI / 2 + (index / root.items.length) * 2 * Math.PI

            readonly property real px: root.cx + root.r * Math.cos(angle)
            readonly property real py: root.cy + root.r * Math.sin(angle)

            x: px - 44; y: py - 32
            width: 88; height: 64

            Rectangle {
                anchors.fill: parent
                radius: 10
                color: modelData.isLeaf ? "#E00D1B2E" : "#E01A1A2E"
                border.color: modelData.isLeaf ? "#4488DD" : "#335577"
                border.width: 1
            }

            Column {
                anchors.centerIn: parent
                spacing: 3

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: modelData.key
                    color: "#88CCFF"
                    font.pixelSize: 22
                    font.bold: true
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: modelData.label
                    color: "#DDFFFFFF"
                    font.pixelSize: 11
                    width: 82
                    horizontalAlignment: Text.AlignHCenter
                    elide: Text.ElideRight
                }
            }
        }
    }
}
