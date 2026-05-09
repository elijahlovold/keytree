import QtQuick
import QtQuick.Window

Window {
    id: root
    width: 520
    height: 520
    x: Screen.width  / 2 - width  / 2
    y: Screen.height / 2 - height / 2
    // Wayland: layer-shell protocol would replace WindowStaysOnTopHint here
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    color: "transparent"

    Item {
        anchors.fill: parent
        focus: true

        Keys.onPressed: (event) => {
            if (event.key === Qt.Key_Escape) {
                keyTree.back()
                event.accepted = true
                return
            }
            if (event.text.length > 0) {
                keyTree.pressKey(event.text)
                event.accepted = true
            }
        }

        RadialView {
            anchors.fill: parent
            items: keyTree.children
        }

        Text {
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottomMargin: 12
            text: keyTree.atRoot ? "" : keyTree.currentLabel
            color: "#DDFFFFFF"
            font.pixelSize: 13
            style: Text.Outline
            styleColor: "#80000000"
        }
    }

    Component.onCompleted: requestActivate()
}
