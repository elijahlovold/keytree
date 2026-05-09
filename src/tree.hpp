#pragma once
#include <memory>
#include <string>
#include <unordered_map>

struct Node {
    char key = 0;
    std::string label;
    std::string cmd;   // empty if non-leaf
    std::string icon;  // optional display string, max 5 Unicode chars (enforced at model layer)
    std::unordered_map<char, std::unique_ptr<Node>> children;

    bool isLeaf() const { return !cmd.empty(); }
};

std::unique_ptr<Node> loadConfig(const std::string& path);
