# KeyTree

Fast, deterministic, hierarchical application launcher. Single key presses navigate a tree of apps; designed to be launched directly by your window manager.

## Dependencies

### Runtime
- Qt6 Quick (≥ 6.2)
- X11 (Wayland: works via XWayland or with layer-shell support in Qt6)

### Build
- CMake ≥ 3.24
- C++20 compiler (GCC 11+ or Clang 14+)
- Qt6 development libraries
- X11 development headers
- CUnit (for tests)
- git (FetchContent pulls toml++ at configure time)

### Arch / Artix

```sh
sudo pacman -S cmake qt6-declarative libx11 cunit
```

### Debian / Ubuntu

```sh
sudo apt install cmake qt6-declarative-dev libx11-dev libcunit1-dev
```

## Build

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

Binary: `build/keytree`

## Install

```sh
sudo cmake --install build
```

This installs the `keytree` binary to `/usr/local/bin` and copies `example-config/config.toml` to `~/.config/keytree/config.toml` (respecting `$XDG_CONFIG_HOME`) if no config exists there yet. An existing config is never overwritten.

To install to a different prefix:

```sh
sudo cmake --install build --prefix /usr
```

## Running

Launch directly, or bind to a key in your WM:

```sh
keytree                          # uses ~/.config/keytree/config.toml
keytree /path/to/config.toml     # explicit config
```

## Config

See `example-config/config.toml` for a full annotated example.

### Format

```toml
[commands]
back   = "Escape"   # move up a group (or quit at root)
quit   = "q"        # quit immediately
search = "/"        # enter PATH search mode

[colors]
leaf_bg          = "#E0071520"
leaf_border      = "#4499EE"
group_bg         = "#E01A0C06"
group_border     = "#AA6622"
key_text         = "#88CCFF"
label_text       = "#DDFFFFFF"
connector        = "#334466"
center_dot       = "#5588CC"
search_bg        = "#E0071520"
search_border    = "#4499EE"
search_selection = "#774499EE"
search_text      = "#AACCFF"

[keys.t]
label = "Terminal"
cmd   = "alacritty"
icon  = "  "        # optional — Nerd Font glyph or emoji, max 5 chars

[keys.g]
label = "Games"     # no cmd = group node; pressing g navigates into it
icon  = "  "

[keys.g.s]
label = "Steam"
cmd   = "steam"
icon  = "  "
```

- Leaf nodes (`cmd` set): pressing the key launches the command and exits
- Group nodes (no `cmd`): pressing the key navigates into the subtree
- Commands run via `/bin/sh -c` — PATH and shell features work
- Colors use Qt's `#AARRGGBB` format; all `[colors]` and `[commands]` keys are optional

### Steam games

```sh
steam -applaunch <appid>
```

Find appids for installed games:

```sh
grep -R -E '"appid"|"name"' ~/.steam/steam/steamapps/appmanifest_*.acf
```

## Tests

```sh
cmake --build build --target keytree_tests
./build/keytree_tests
```
