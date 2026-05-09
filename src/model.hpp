#pragma once
#include "tree.hpp"
#include <QObject>
#include <QVariantList>
#include <vector>

class KeyTreeModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList children READ children NOTIFY stateChanged)
    Q_PROPERTY(QString currentLabel READ currentLabel NOTIFY stateChanged)
    Q_PROPERTY(bool atRoot READ atRoot NOTIFY stateChanged)

public:
    explicit KeyTreeModel(Node* root, QObject* parent = nullptr);

    QVariantList children() const;
    QString currentLabel() const;
    bool atRoot() const;

    Q_INVOKABLE void pressKey(const QString& key);
    Q_INVOKABLE void back();

signals:
    void stateChanged();
    void quit();

private:
    Node* m_root;
    Node* m_current;
    std::vector<Node*> m_stack;
};
