#pragma once
#include "tree.hpp"
#include <QObject>
#include <QStringList>
#include <QVariantList>
#include <future>
#include <vector>

class KeyTreeModel : public QObject {
    Q_OBJECT
    // Tree navigation
    Q_PROPERTY(QVariantList children    READ children     NOTIFY stateChanged)
    Q_PROPERTY(QString currentLabel     READ currentLabel NOTIFY stateChanged)
    Q_PROPERTY(bool atRoot              READ atRoot       NOTIFY stateChanged)
    // Search
    Q_PROPERTY(bool searchMode          READ searchMode   NOTIFY searchStateChanged)
    Q_PROPERTY(QString searchText       READ searchText   NOTIFY searchStateChanged)
    Q_PROPERTY(QStringList searchResults READ searchResults NOTIFY searchStateChanged)
    Q_PROPERTY(int searchSelection      READ searchSelection NOTIFY searchStateChanged)

public:
    explicit KeyTreeModel(Node* root, QObject* parent = nullptr);

    QVariantList children() const;
    QString currentLabel() const;
    bool atRoot() const;

    bool searchMode() const       { return m_searchMode; }
    QString searchText() const    { return m_searchText; }
    QStringList searchResults() const { return m_searchResults; }
    int searchSelection() const   { return m_searchSelection; }

    Q_INVOKABLE void pressKey(const QString& key);
    Q_INVOKABLE void back();

    Q_INVOKABLE void enterSearch();
    Q_INVOKABLE void exitSearch();
    Q_INVOKABLE void appendSearch(const QString& ch);
    Q_INVOKABLE void backspaceSearch();
    Q_INVOKABLE void navigateSearch(int delta);
    Q_INVOKABLE void confirmSearch();

signals:
    void stateChanged();
    void searchStateChanged();
    void quit();

private:
    Node* m_root;
    Node* m_current;
    std::vector<Node*> m_stack;

    bool m_searchMode = false;
    QString m_searchText;
    int m_searchSelection = 0;
    QStringList m_searchResults;

    std::vector<std::string> m_allBins;
    bool m_binsReady = false;
    std::future<std::vector<std::string>> m_binsFuture;

    void updateSearchResults();
    static std::vector<std::string> scanPath();
};
