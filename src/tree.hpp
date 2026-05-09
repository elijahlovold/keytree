#pragma once
#include <memory>
#include <string>
#include <unordered_map>

struct ColorScheme {
    // Node cards — defaults match the built-in fallback palette
    std::string leafBg          = "#E00D1B2E";
    std::string leafBorder      = "#4488DD";
    std::string groupBg         = "#E01A1A2E";
    std::string groupBorder     = "#335577";
    // Text
    std::string keyText         = "#88CCFF";
    std::string labelText       = "#DDFFFFFF";
    // Radial connectors
    std::string connector       = "#334466";
    std::string centerDot       = "#5588CC";
    // Search panel
    std::string searchBg        = "#E01A1A2E";
    std::string searchBorder    = "#4488DD";
    std::string searchSelection = "#774488DD";
    std::string searchText      = "#CCDDFF";
};

struct Node {
    char key = 0;
    std::string label;
    std::string cmd;   // empty if non-leaf
    std::string icon;  // optional display string, max 5 Unicode chars (enforced at model layer)
    std::unordered_map<char, std::unique_ptr<Node>> children;

    bool isLeaf() const { return !cmd.empty(); }
};

struct KeyBindings {
    std::string back   = "Escape";
    std::string quit   = "q";
    std::string search = "/";
};

std::unique_ptr<Node> loadConfig(const std::string& path);
ColorScheme           loadColors(const std::string& path);
KeyBindings           loadKeyBindings(const std::string& path);
