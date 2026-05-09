#include "tree.hpp"
#include <toml++/toml.hpp>

static void buildSubtree(Node* parent, const toml::table& tbl) {
    for (auto& [k, v] : tbl) {
        if (!v.is_table()) continue;

        auto child = std::make_unique<Node>();
        child->key = static_cast<char>(std::string_view(k)[0]);

        const auto& child_tbl = *v.as_table();
        if (auto label = child_tbl["label"].value<std::string>())
            child->label = *label;
        if (auto cmd = child_tbl["cmd"].value<std::string>())
            child->cmd = *cmd;
        if (auto icon = child_tbl["icon"].value<std::string>())
            child->icon = *icon;

        buildSubtree(child.get(), child_tbl);
        parent->children[child->key] = std::move(child);
    }
}

std::unique_ptr<Node> loadConfig(const std::string& path) {
    auto cfg = toml::parse_file(path);
    auto root = std::make_unique<Node>();
    root->label = "root";

    if (const auto* keys = cfg["keys"].as_table())
        buildSubtree(root.get(), *keys);

    return root;
}

KeyBindings loadKeyBindings(const std::string& path) {
    KeyBindings kb;
    auto cfg = toml::parse_file(path);
    const auto* tbl = cfg["commands"].as_table();
    if (!tbl) return kb;

    auto get = [&](const char* key, std::string& field) {
        if (auto v = (*tbl)[key].value<std::string>()) field = *v;
    };

    get("back",   kb.back);
    get("quit",   kb.quit);
    get("search", kb.search);

    return kb;
}

ColorScheme loadColors(const std::string& path) {
    ColorScheme cs;
    auto cfg = toml::parse_file(path);
    const auto* tbl = cfg["colors"].as_table();
    if (!tbl) return cs;

    auto get = [&](const char* key, std::string& field) {
        if (auto v = (*tbl)[key].value<std::string>()) field = *v;
    };

    get("leaf_bg",          cs.leafBg);
    get("leaf_border",      cs.leafBorder);
    get("group_bg",         cs.groupBg);
    get("group_border",     cs.groupBorder);
    get("key_text",         cs.keyText);
    get("label_text",       cs.labelText);
    get("connector",        cs.connector);
    get("center_dot",       cs.centerDot);
    get("search_bg",        cs.searchBg);
    get("search_border",    cs.searchBorder);
    get("search_selection", cs.searchSelection);
    get("search_text",      cs.searchText);

    return cs;
}
