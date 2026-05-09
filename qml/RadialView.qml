import QtQuick

Item {
    id: root

    property var    items: []
    property real   fontScale:   1.0
    property string centerLabel: ""
    property bool   atRoot:      true

    readonly property real cx:      width  / 2
    readonly property real cy:      height / 2
    readonly property real r:       Math.min(width, height) * 0.38
    readonly property real nodeW:   Math.round(88 * fontScale)
    readonly property real nodeH:   Math.round(64 * fontScale)
    readonly property real centerW: Math.round(80 * fontScale)
    readonly property real centerH: Math.round(46 * fontScale)

    // Connector lines — drawn first so node cards render on top
    Canvas {
        anchors.fill: parent

        // Repaint whenever items or dimensions change
        property var watchItems: root.items
        onWatchItemsChanged: requestPaint()
        onWidthChanged:      requestPaint()
        onHeightChanged:     requestPaint()

        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)
            var n = root.items.length
            if (n === 0) return

            ctx.strokeStyle = colorScheme.connector
            ctx.lineWidth   = Math.max(1, Math.round(fontScale))
            ctx.globalAlpha = 0.45
            ctx.lineCap     = "round"

            for (var i = 0; i < n; i++) {
                var angle = n === 1
                    ? -Math.PI / 2
                    : -Math.PI / 2 + (i / n) * 2 * Math.PI
                ctx.beginPath()
                ctx.moveTo(root.cx, root.cy)
                ctx.lineTo(root.cx + root.r * Math.cos(angle),
                           root.cy + root.r * Math.sin(angle))
                ctx.stroke()
            }
        }
    }

    // Center element — dot at root, labeled group card in subtrees
    Item {
        x: root.cx - width  / 2
        y: root.cy - height / 2
        width:  atRoot ? Math.round(10 * fontScale) : root.centerW
        height: atRoot ? Math.round(10 * fontScale) : root.centerH

        // Dot
        Rectangle {
            visible: atRoot
            anchors.fill: parent
            radius: width / 2
            color:   colorScheme.centerDot
            opacity: 0.85
        }

        // Group card
        Rectangle {
            visible: !atRoot
            anchors.fill: parent
            radius:       Math.round(8 * fontScale)
            color:        colorScheme.groupBg
            border.color: colorScheme.centerDot
            border.width: 1

            Text {
                anchors.centerIn: parent
                text:  centerLabel
                color: colorScheme.labelText
                font.pixelSize: Math.round(12 * fontScale)
                font.bold: true
                width: parent.width - Math.round(8 * fontScale)
                horizontalAlignment: Text.AlignHCenter
                elide: Text.ElideRight
            }
        }
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

            // Leaf: rounder + thicker bright border. Group: angular + thin muted border.
            Rectangle {
                anchors.fill: parent
                radius:       modelData.isLeaf ? Math.round(12 * fontScale)
                                               : Math.round(5  * fontScale)
                color:        modelData.isLeaf ? colorScheme.leafBg    : colorScheme.groupBg
                border.color: modelData.isLeaf ? colorScheme.leafBorder : colorScheme.groupBorder
                border.width: modelData.isLeaf ? 2 : 1
            }

            Column {
                anchors.centerIn: parent
                spacing: Math.round(3 * fontScale)

                Row {
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: Math.round(3 * fontScale)

                    Text {
                        visible: modelData.icon.length > 0
                        text: modelData.icon
                        color: colorScheme.keyText
                        font.pixelSize: Math.round(18 * fontScale)
                        font.bold: true
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    Text {
                        text: modelData.key
                        color: colorScheme.keyText
                        font.pixelSize: Math.round(22 * fontScale)
                        font.bold: true
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: modelData.label
                    color: colorScheme.labelText
                    font.pixelSize: Math.round(11 * fontScale)
                    width: Math.round(82 * fontScale)
                    horizontalAlignment: Text.AlignHCenter
                    elide: Text.ElideRight
                }
            }
        }
    }
}
