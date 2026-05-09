# keytree

Hierarchical application launcher. Single key presses navigate a tree of apps; designed to be launched directly by your window manager.

## Dependencies

### Runtime
- Qt6 Quick (≥ 6.2)
- X11 (Wayland: works via XWayland or with layer-shell support in Qt6)

### Build
- CMake ≥ 3.21
- C++20 compiler (GCC 11+ or Clang 14+)
- Qt6 development libraries
- X11 development headers
- git (for FetchContent to pull toml++)

### Arch / Artix

```sh
sudo pacman -S cmake qt6-declarative libx11
```

### Debian / Ubuntu

```sh
sudo apt install cmake qt6-declarative-dev libx11-dev
```

## Build

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j$(nproc)
```

Binary: `build/keytree`

## Install

```sh
sudo cmake --install build  # installs to /usr/local/bin/keytree
```

## Config

Place config at `~/.config/keytree/config.toml` (respects `$XDG_CONFIG_HOME`).

Or pass a path explicitly: `keytree /path/to/config.toml`

See `config/config.toml` for a full example.

### Format

```toml
[keys.t]
label = "Terminal"
cmd = "alacritty"

[keys.g]
label = "Games"

[keys.g.s]
label = "Steam"
cmd = "steam"
```

- Nodes without `cmd` are subtrees — pressing their key navigates into them
- Nodes with `cmd` are leaves — pressing their key launches the command and exits
- Commands run via `/bin/sh -c` so PATH and shell features work
- ESC navigates back; ESC at root exits

## WM Integration

### i3 / Sway

```
bindsym $mod+space exec keytree
```

### bspwm (sxhkd)

```
super + space
    keytree
```

### Hyprland

```
bind = SUPER, space, exec, keytree
```

## Wayland Notes

The overlay uses standard Qt window flags (`WindowStaysOnTopHint`). On Wayland compositors without XWayland, proper overlay positioning requires the `wlr-layer-shell` protocol — this is the intended future integration point. See the comment in `qml/Main.qml` for where the flag divergence lives.
