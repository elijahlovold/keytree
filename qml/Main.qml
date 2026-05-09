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
            if (keyTree.searchMode) {
                switch (event.key) {
                case Qt.Key_Escape:
                    keyTree.exitSearch(); break
                case Qt.Key_Return:
                case Qt.Key_Enter:
                    keyTree.confirmSearch(); break
                case Qt.Key_Backspace:
                    keyTree.backspaceSearch(); break
                case Qt.Key_Up:
                    keyTree.navigateSearch(-1); break
                case Qt.Key_Down:
                    keyTree.navigateSearch(1); break
                default:
                    if (event.text.length > 0)
                        keyTree.appendSearch(event.text)
                }
                event.accepted = true
                return
            }

            // Normal tree mode
            if (event.key === Qt.Key_Escape) {
                keyTree.back()
                event.accepted = true
                return
            }
            if (event.text === "/") {
                keyTree.enterSearch()
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
            visible: !keyTree.searchMode
        }

        SearchView {
            visible: keyTree.searchMode
        }

        Text {
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottomMargin: 12
            visible: !keyTree.searchMode && !keyTree.atRoot
            text: keyTree.currentLabel
            color: "#DDFFFFFF"
            font.pixelSize: 13
            style: Text.Outline
            styleColor: "#80000000"
        }
    }

    Component.onCompleted: requestActivate()
}
