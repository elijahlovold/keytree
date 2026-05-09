#include "model.hpp"
#include "executor.hpp"
#include <QVariantMap>

KeyTreeModel::KeyTreeModel(Node* root, QObject* parent)
    : QObject(parent), m_root(root), m_current(root) {}

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
