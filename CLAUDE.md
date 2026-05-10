# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

First build fetches `toml++` via CMake FetchContent (requires network). Subsequent builds are offline.

```sh
./build/keytree                          # run with default config (~/.config/keytree/config.toml)
./build/keytree /path/to/config.toml     # explicit config path
```

## Tests

```sh
./build/keytree_tests       # verbose CUnit output
ctest --test-dir build -V   # via CTest
```

Tests recompile `tree.cpp`, `model.cpp`, `executor.cpp` directly (no shared library). There is no mock for `executor` — tests that reach leaf nodes or `confirmSearch` will spawn real child processes (`/bin/true`).

## Architecture

keytree is a **single binary** launched on demand by the WM (no daemon). The critical startup path is deliberately ordered to minimize time-to-visible:

1. `QQuickWindow::setDefaultAlphaBuffer(true)` — must precede `QGuiApplication`
2. `loadConfig()` — TOML parse happens **before** Qt engine init to front-load I/O
3. `KeyTreeModel` constructor kicks off `std::async` PATH scan for search mode
4. QML engine loads; `KeyTreeModel` is exposed as `keyTree` context property
5. `setX11DialogType()` sets `_NET_WM_WINDOW_TYPE_DIALOG` via Xlib **before** `window->show()` so i3 sees it at MapRequest and floats the window automatically
6. `window->show()`

### Data flow

```
config.toml → loadConfig() → Node tree (tree.hpp)
                                  ↓
                           KeyTreeModel (model.hpp)
                           ├── tree navigation: pressKey / back
                           └── search mode: scanPath (async) + filter
                                  ↓ Q_PROPERTY / Q_INVOKABLE
                           QML (keyTree context property)
                           ├── Main.qml — key routing, mode switching
                           ├── RadialView.qml — polar layout of current node's children
                           └── SearchView.qml — PATH search input + result list
```

### Key design points

**Node tree** (`tree.hpp`): `Node` holds `key`, `label`, `cmd`, `icon`, and `children`. Leaf nodes have `cmd` set; intermediate nodes do not. The tree is owned by `unique_ptr<Node> root` in `main()` and outlives everything.

**KeyTreeModel** has two modes:
- *Tree mode*: `pressKey(char)` traverses `m_current` through `m_stack`; ESC calls `back()`, which emits `quit()` at root
- *Search mode*: entered by `/`, driven by `appendSearch`/`backspaceSearch`/`navigateSearch`/`confirmSearch`; PATH scan result is cached in `m_allBins` after first `enterSearch()`

**QML ↔ C++**: `keyTree` is the sole context property. QML reads `children` (a `QVariantList` of maps with keys `key`, `label`, `cmd`, `isLeaf`, `icon`) and calls invokables directly. No signals flow from QML to C++.

**Font/size scaling**: `Main.qml` computes `physicalDpi = Screen.pixelDensity * Screen.devicePixelRatio * 25.4` and derives `dpiScale` relative to 96 DPI. Window is pre-sized at `520 * dpiScale` logical px. `fontScale = min(width, height) / 520` is passed as a property to `RadialView` and `SearchView` so all geometry and font sizes stay proportional.

**Icon field**: optional `icon` string in TOML (e.g. Nerd Font glyphs or emoji). Stored raw in `Node::icon`, clamped to 5 QChars in `KeyTreeModel::children()` before exposure to QML.

**Wayland**: the single platform-divergence point is in `Main.qml` (`WindowStaysOnTopHint` comment) and `setX11DialogType` in `main.cpp`. Everything else is platform-neutral Qt.

## Config format

Example config lives at `example-config/config.toml`. Default lookup: `~/.config/keytree/config.toml` (respects `$XDG_CONFIG_HOME`).

```toml
[colors]
leaf_bg          = "#E0071520"   # executable nodes
leaf_border      = "#4499EE"
group_bg         = "#E01A0C06"   # subtree nodes
group_border     = "#AA6622"
key_text         = "#88CCFF"
label_text       = "#DDFFFFFF"
connector        = "#334466"     # radial lines (0.45 opacity applied in renderer)
center_dot       = "#5588CC"
search_bg        = "#E0071520"
search_border    = "#4499EE"
search_selection = "#774499EE"
search_text      = "#AACCFF"

[keys.t]
label = "Terminal"
cmd = "alacritty"
icon = "  "       # optional Nerd Font glyph or emoji, max 5 Unicode chars

[keys.g]
label = "Games"   # no cmd = subtree/group node

[keys.g.s]
label = "Steam"
cmd = "steam"
```

All `[colors]` keys are optional — omitted keys fall back to the defaults in `ColorScheme` in `tree.hpp`. Colors use Qt's `#AARRGGBB` format. Commands run via `/bin/sh -c`. `/` from any tree level enters PATH search mode.
