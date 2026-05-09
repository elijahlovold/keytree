import QtQuick

Item {
    id: root

    property var  items: []
    property real fontScale: 1.0

    readonly property real cx:    width  / 2
    readonly property real cy:    height / 2
    readonly property real r:     Math.min(width, height) * 0.38
    readonly property real nodeW: Math.round(88 * fontScale)
    readonly property real nodeH: Math.round(64 * fontScale)

    // Center dot — indicates active subtree depth
    Rectangle {
        x: root.cx - Math.round(5 * fontScale)
        y: root.cy - Math.round(5 * fontScale)
        width:  Math.round(10 * fontScale)
        height: Math.round(10 * fontScale)
        radius: Math.round(5  * fontScale)
        color: "#5588CC"
        opacity: 0.85
    }

    Repeater {
        model: root.items

        delegate: Item {
            id: node

            required property var modelData
            required property int index

            readonly property real angle: root.items.length === 1
                ? -Math.PI / 2
                : -Math.PI / 2 + (index / root.items.length) * 2 * Math.PI

            readonly property real px: root.cx + root.r * Math.cos(angle)
            readonly property real py: root.cy + root.r * Math.sin(angle)

            x: px - root.nodeW / 2
            y: py - root.nodeH / 2
            width:  root.nodeW
            height: root.nodeH

            Rectangle {
                anchors.fill: parent
                radius: Math.round(10 * fontScale)
                color: modelData.isLeaf ? "#E00D1B2E" : "#E01A1A2E"
                border.color: modelData.isLeaf ? "#4488DD" : "#335577"
                border.width: 1
            }

            Column {
                anchors.centerIn: parent
                spacing: Math.round(3 * fontScale)

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: modelData.key
                    color: "#88CCFF"
                    font.pixelSize: Math.round(22 * fontScale)
                    font.bold: true
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: modelData.label
                    color: "#DDFFFFFF"
                    font.pixelSize: Math.round(11 * fontScale)
                    width: Math.round(82 * fontScale)
                    horizontalAlignment: Text.AlignHCenter
                    elide: Text.ElideRight
                }
            }
        }
    }
}
