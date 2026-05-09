import QtQuick
import QtQuick.Window

Window {
    id: root

    // Physical DPI: pixelDensity is logical px/mm; multiply by devicePixelRatio to
    // get physical px/mm, then by 25.4 to get physical DPI. Works on both X11
    // (unscaled 4K: devicePixelRatio=1, high pixelDensity) and Wayland
    // (compositor-scaled: devicePixelRatio>1, lower reported pixelDensity).
    readonly property real physicalDpi: Screen.pixelDensity * Screen.devicePixelRatio * 25.4
    readonly property real dpiScale:    Math.max(1.0, physicalDpi / 96.0)

    // Window pre-sized by DPI so it occupies a consistent physical footprint.
    // fontScale is derived from actual window size rather than DPI alone so that
    // any manual window resize or WM override also flows through to fonts.
    width:  Math.round(520 * dpiScale)
    height: Math.round(520 * dpiScale)
    readonly property real fontScale: Math.min(width, height) / 520.0

    x: Screen.width  / 2 - width  / 2
    y: Screen.height / 2 - height / 2
    // Wayland: layer-shell protocol would replace WindowStaysOnTopHint here
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    color: "transparent"

    Item {
        anchors.fill: parent
        focus: true

        // Maps a config binding string to a key event match.
        // Named keys: "Escape"/"Esc", "Return"/"Enter", "Backspace", "Tab".
        // Everything else is matched as a single character against event.text.
        function keyMatches(event, binding) {
            switch (binding) {
            case "Escape": case "Esc":
                return event.key === Qt.Key_Escape
            case "Return": case "Enter":
                return event.key === Qt.Key_Return || event.key === Qt.Key_Enter
            case "Backspace":
                return event.key === Qt.Key_Backspace
            case "Tab":
                return event.key === Qt.Key_Tab
            default:
                return binding.length === 1 && event.text === binding
            }
        }

        Keys.onPressed: (event) => {
            if (keyTree.searchMode) {
                // back exits search; quit is intentionally inactive while typing
                if (keyMatches(event, keyBindings.back)) {
                    keyTree.exitSearch()
                } else if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
                    keyTree.confirmSearch()
                } else if (event.key === Qt.Key_Backspace) {
                    keyTree.backspaceSearch()
                } else if (event.key === Qt.Key_Up) {
                    keyTree.navigateSearch(-1)
                } else if (event.key === Qt.Key_Down) {
                    keyTree.navigateSearch(1)
                } else if (event.text.length > 0) {
                    keyTree.appendSearch(event.text)
                }
                event.accepted = true
                return
            }

            // Normal tree mode
            if (keyMatches(event, keyBindings.quit)) {
                Qt.quit()
                event.accepted = true
                return
            }
            if (keyMatches(event, keyBindings.back)) {
                keyTree.back()
                event.accepted = true
                return
            }
            if (keyMatches(event, keyBindings.search)) {
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
            items:       keyTree.children
            fontScale:   root.fontScale
            centerLabel: keyTree.currentLabel
            atRoot:      keyTree.atRoot
            visible: !keyTree.searchMode
        }

        SearchView {
            fontScale: root.fontScale
            visible: keyTree.searchMode
        }
    }

    Component.onCompleted: requestActivate()
}
