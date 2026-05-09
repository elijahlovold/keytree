#include "model.hpp"
#include "executor.hpp"
#include <QVariantMap>
#include <dirent.h>
#include <set>
#include <sstream>
#include <unistd.h>

static constexpr int kMaxResults = 10;

KeyTreeModel::KeyTreeModel(Node* root, QObject* parent)
    : QObject(parent), m_root(root), m_current(root),
      // Scan PATH in background so it's hidden behind Qt/QML startup time
      m_binsFuture(std::async(std::launch::async, &KeyTreeModel::scanPath))
{}

// ── Tree navigation ───────────────────────────────────────────────────────────

QVariantList KeyTreeModel::children() const {
    QVariantList list;
    for (auto& [k, node] : m_current->children) {
        QVariantMap item;
        item[QStringLiteral("key")]    = QString(QChar(node->key));
        item[QStringLiteral("label")]  = QString::fromStdString(node->label);
        item[QStringLiteral("isLeaf")] = node->isLeaf();
        list.append(item);
    }
    return list;
}

QString KeyTreeModel::currentLabel() const {
    return QString::fromStdString(m_current->label);
}

bool KeyTreeModel::atRoot() const {
    return m_current == m_root;
}

void KeyTreeModel::pressKey(const QString& key) {
    if (key.isEmpty()) return;
    char k = key[0].toLower().toLatin1();
    auto it = m_current->children.find(k);
    if (it == m_current->children.end()) return;

    Node* next = it->second.get();
    if (next->isLeaf()) {
        execute(next->cmd);
        emit quit();
    } else {
        m_stack.push_back(m_current);
        m_current = next;
        emit stateChanged();
    }
}

void KeyTreeModel::back() {
    if (!m_stack.empty()) {
        m_current = m_stack.back();
        m_stack.pop_back();
        emit stateChanged();
    } else {
        emit quit();
    }
}

// ── Search ────────────────────────────────────────────────────────────────────

std::vector<std::string> KeyTreeModel::scanPath() {
    std::vector<std::string> bins;
    const char* pathEnv = std::getenv("PATH");
    if (!pathEnv) return bins;

    std::set<std::string> seen;
    std::stringstream ss(pathEnv);
    std::string dir;

    while (std::getline(ss, dir, ':')) {
        DIR* d = opendir(dir.c_str());
        if (!d) continue;
        struct dirent* entry;
        while ((entry = readdir(d)) != nullptr) {
            if (entry->d_name[0] == '.') continue;
            if (entry->d_type == DT_DIR) continue;
            std::string fullPath = dir + "/" + entry->d_name;
            if (access(fullPath.c_str(), X_OK) == 0)
                seen.emplace(entry->d_name);
        }
        closedir(d);
    }

    bins.assign(seen.begin(), seen.end());  // set is already sorted
    return bins;
}

void KeyTreeModel::updateSearchResults() {
    m_searchResults.clear();
    m_searchSelection = 0;

    if (m_searchText.isEmpty()) {
        for (int i = 0; i < (int)m_allBins.size() && m_searchResults.size() < kMaxResults; ++i)
            m_searchResults << QString::fromStdString(m_allBins[i]);
        emit searchStateChanged();
        return;
    }

    std::string query = m_searchText.toLower().toStdString();

    // Prefix matches first, then substring — mirrors dmenu/rofi ordering
    for (auto& bin : m_allBins) {
        if ((int)m_searchResults.size() >= kMaxResults) break;
        if (bin.starts_with(query))
            m_searchResults << QString::fromStdString(bin);
    }
    for (auto& bin : m_allBins) {
        if ((int)m_searchResults.size() >= kMaxResults) break;
        if (!bin.starts_with(query) && bin.find(query) != std::string::npos)
            m_searchResults << QString::fromStdString(bin);
    }

    emit searchStateChanged();
}

void KeyTreeModel::enterSearch() {
    if (!m_binsReady) {
        m_allBins = m_binsFuture.get();
        m_binsReady = true;
    }
    m_searchMode = true;
    updateSearchResults();
}

void KeyTreeModel::exitSearch() {
    m_searchMode = false;
    m_searchText.clear();
    m_searchResults.clear();
    m_searchSelection = 0;
    emit searchStateChanged();
}

void KeyTreeModel::appendSearch(const QString& ch) {
    m_searchText += ch;
    updateSearchResults();
}

void KeyTreeModel::backspaceSearch() {
    if (m_searchText.isEmpty()) {
        exitSearch();
        return;
    }
    m_searchText.chop(1);
    updateSearchResults();
}

void KeyTreeModel::navigateSearch(int delta) {
    int n = m_searchResults.size();
    if (n == 0) return;
    m_searchSelection = ((m_searchSelection + delta) % n + n) % n;
    emit searchStateChanged();
}

void KeyTreeModel::confirmSearch() {
    if (m_searchSelection < m_searchResults.size()) {
        execute(m_searchResults[m_searchSelection].toStdString());
        emit quit();
    }
}
