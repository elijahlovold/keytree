#include "model.hpp"
#include "tree.hpp"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <cstdlib>
#include <stdexcept>
#include <X11/Xlib.h>
#include <X11/Xatom.h>

// Sets _NET_WM_WINDOW_TYPE_DIALOG before the window is mapped so i3 sees it
// at MapRequest time and makes the window floating automatically.
static void setX11DialogType(QWindow* w) {
    Display* dpy = XOpenDisplay(nullptr);
    if (!dpy) return;
    Window xwin = static_cast<Window>(w->winId());  // winId() forces native creation
    Atom netWmWindowType      = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE",        False);
    Atom netWmWindowTypeDialog = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_DIALOG", False);
    XChangeProperty(dpy, xwin, netWmWindowType, XA_ATOM, 32,
                    PropModeReplace,
                    reinterpret_cast<unsigned char*>(&netWmWindowTypeDialog), 1);
    XFlush(dpy);
    XCloseDisplay(dpy);
}

static std::string resolveConfigPath(int argc, char* argv[]) {
    if (argc > 1) return argv[1];
    if (const char* xdg = std::getenv("XDG_CONFIG_HOME"))
        return std::string(xdg) + "/keytree/config.toml";
    if (const char* home = std::getenv("HOME"))
        return std::string(home) + "/.config/keytree/config.toml";
    return "config.toml";
}

int main(int argc, char* argv[]) {
    // Must precede QGuiApplication construction for alpha channel to work
    QQuickWindow::setDefaultAlphaBuffer(true);

    // Parse config before Qt engine init to front-load the only real I/O
    std::string cfgPath = resolveConfigPath(argc, argv);
    std::unique_ptr<Node> root;
    ColorScheme colors;
    try {
        root   = loadConfig(cfgPath);
        colors = loadColors(cfgPath);
    } catch (const std::exception& e) {
        qCritical("keytree: failed to load config '%s': %s", cfgPath.c_str(), e.what());
        return 1;
    }

    QGuiApplication app(argc, argv);

    KeyTreeModel model(root.get());
    QObject::connect(&model, &KeyTreeModel::quit, &app, &QCoreApplication::quit);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("keyTree"), &model);

    QVariantMap colorMap;
    colorMap[QStringLiteral("leafBg")]          = QString::fromStdString(colors.leafBg);
    colorMap[QStringLiteral("leafBorder")]      = QString::fromStdString(colors.leafBorder);
    colorMap[QStringLiteral("groupBg")]         = QString::fromStdString(colors.groupBg);
    colorMap[QStringLiteral("groupBorder")]     = QString::fromStdString(colors.groupBorder);
    colorMap[QStringLiteral("keyText")]         = QString::fromStdString(colors.keyText);
    colorMap[QStringLiteral("labelText")]       = QString::fromStdString(colors.labelText);
    colorMap[QStringLiteral("connector")]       = QString::fromStdString(colors.connector);
    colorMap[QStringLiteral("centerDot")]       = QString::fromStdString(colors.centerDot);
    colorMap[QStringLiteral("searchBg")]        = QString::fromStdString(colors.searchBg);
    colorMap[QStringLiteral("searchBorder")]    = QString::fromStdString(colors.searchBorder);
    colorMap[QStringLiteral("searchSelection")] = QString::fromStdString(colors.searchSelection);
    colorMap[QStringLiteral("searchText")]      = QString::fromStdString(colors.searchText);
    engine.rootContext()->setContextProperty(QStringLiteral("colorScheme"), colorMap);

    // Wayland: qt_add_qml_module embeds QML as resources; loading is identical
    // on X11 and Wayland — platform-specific divergence is in window flags only.
    engine.loadFromModule("KeyTree", "Main");
    if (engine.rootObjects().isEmpty())
        return 1;

    auto* window = qobject_cast<QQuickWindow*>(engine.rootObjects().first());
    if (window) {
        window->setFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        setX11DialogType(window);  // must be before show() so i3 sees it at MapRequest
        window->show();
    }

    return app.exec();
}
