#include <CUnit/Basic.h>
#include <CUnit/CUnit.h>
#include <QCoreApplication>
#include <unistd.h>
#include "model.hpp"
#include "tree.hpp"

static std::string writeTempToml(const std::string& content) {
    char tmpl[] = "/tmp/keytree_test_XXXXXX";
    int fd = mkstemp(tmpl);
    ::write(fd, content.c_str(), content.size());
    ::close(fd);
    return tmpl;
}

// ── Tree ──────────────────────────────────────────────────────────────────────

static void test_leaf_parses() {
    auto path = writeTempToml(R"(
[keys.t]
label = "Terminal"
cmd = "alacritty"
)");
    auto root = loadConfig(path);
    ::unlink(path.c_str());

    auto& node = root->children.at('t');
    CU_ASSERT_STRING_EQUAL(node->label.c_str(), "Terminal");
    CU_ASSERT_STRING_EQUAL(node->cmd.c_str(), "alacritty");
    CU_ASSERT_TRUE(node->isLeaf());
    CU_ASSERT_TRUE(node->children.empty());
}

static void test_subtree_parses() {
    auto path = writeTempToml(R"(
[keys.g]
label = "Games"
[keys.g.s]
label = "Steam"
cmd = "steam"
)");
    auto root = loadConfig(path);
    ::unlink(path.c_str());

    auto& g = root->children.at('g');
    CU_ASSERT_FALSE(g->isLeaf());
    CU_ASSERT_EQUAL((int)g->children.size(), 1);
    CU_ASSERT_STRING_EQUAL(g->children.at('s')->cmd.c_str(), "steam");
}

static void test_multiple_root_keys() {
    auto path = writeTempToml(R"(
[keys.a]
label = "A"
cmd = "a"
[keys.b]
label = "B"
cmd = "b"
[keys.c]
label = "C"
cmd = "c"
)");
    auto root = loadConfig(path);
    ::unlink(path.c_str());
    CU_ASSERT_EQUAL((int)root->children.size(), 3);
}

static void test_bad_path_throws() {
    bool threw = false;
    try { loadConfig("/no/such/file.toml"); }
    catch (...) { threw = true; }
    CU_ASSERT_TRUE(threw);
}

// ── Navigation ────────────────────────────────────────────────────────────────

static std::unique_ptr<Node> nav_root;
static KeyTreeModel* nav_model = nullptr;

static int nav_init() {
    auto path = writeTempToml(R"(
[keys.g]
label = "Games"
[keys.g.s]
label = "Steam"
cmd = "/bin/true"
[keys.t]
label = "Terminal"
cmd = "/bin/true"
)");
    try { nav_root = loadConfig(path); } catch (...) { ::unlink(path.c_str()); return 1; }
    ::unlink(path.c_str());
    nav_model = new KeyTreeModel(nav_root.get());
    return 0;
}

static int nav_cleanup() {
    delete nav_model; nav_model = nullptr;
    nav_root.reset();
    return 0;
}

static void test_initial_at_root() {
    CU_ASSERT_TRUE(nav_model->atRoot());
    CU_ASSERT_EQUAL((int)nav_model->children().size(), 2); // g, t
    CU_ASSERT_FALSE(nav_model->searchMode());
}

static void test_unknown_key_ignored() {
    nav_model->pressKey("z");
    CU_ASSERT_TRUE(nav_model->atRoot());
}

static void test_navigate_into_subtree() {
    nav_model->pressKey("g");
    CU_ASSERT_FALSE(nav_model->atRoot());
    CU_ASSERT_STRING_EQUAL(nav_model->currentLabel().toStdString().c_str(), "Games");
    CU_ASSERT_EQUAL((int)nav_model->children().size(), 1); // s only
    nav_model->back();
}

static void test_back_restores_parent() {
    nav_model->pressKey("g");
    nav_model->back();
    CU_ASSERT_TRUE(nav_model->atRoot());
    CU_ASSERT_EQUAL((int)nav_model->children().size(), 2);
}

static void test_key_case_insensitive() {
    nav_model->pressKey("G"); // uppercase
    CU_ASSERT_FALSE(nav_model->atRoot());
    nav_model->back();
    CU_ASSERT_TRUE(nav_model->atRoot());
}

static void test_back_at_root_emits_quit() {
    bool fired = false;
    auto conn = QObject::connect(nav_model, &KeyTreeModel::quit, [&] { fired = true; });
    nav_model->back();
    QObject::disconnect(conn);
    CU_ASSERT_TRUE(fired);
}

static void test_leaf_emits_quit() {
    bool fired = false;
    auto conn = QObject::connect(nav_model, &KeyTreeModel::quit, [&] { fired = true; });
    nav_model->pressKey("t");
    QObject::disconnect(conn);
    CU_ASSERT_TRUE(fired);
    CU_ASSERT_TRUE(nav_model->atRoot()); // leaf press must not move m_current
}

// ── Search ────────────────────────────────────────────────────────────────────

static std::unique_ptr<Node> search_root;
static KeyTreeModel* search_model = nullptr;

static int search_init() {
    auto path = writeTempToml(R"(
[keys.t]
label = "Terminal"
cmd = "/bin/true"
)");
    try { search_root = loadConfig(path); } catch (...) { ::unlink(path.c_str()); return 1; }
    ::unlink(path.c_str());
    search_model = new KeyTreeModel(search_root.get());
    return 0;
}

static int search_cleanup() {
    delete search_model; search_model = nullptr;
    search_root.reset();
    return 0;
}

static void test_enter_exit_search() {
    search_model->enterSearch();
    CU_ASSERT_TRUE(search_model->searchMode());
    CU_ASSERT_TRUE(search_model->searchText().isEmpty());
    search_model->exitSearch();
    CU_ASSERT_FALSE(search_model->searchMode());
    CU_ASSERT_TRUE(search_model->searchText().isEmpty());
}

static void test_initial_results_nonempty() {
    search_model->enterSearch();
    CU_ASSERT_TRUE(search_model->searchResults().size() > 0);
    search_model->exitSearch();
}

static void test_append_narrows_results() {
    search_model->enterSearch();
    int all = search_model->searchResults().size();
    search_model->appendSearch("zzzzzzzzz");
    CU_ASSERT_TRUE(search_model->searchResults().size() <= all);
    search_model->exitSearch();
}

static void test_backspace_removes_char() {
    search_model->enterSearch();
    search_model->appendSearch("l");
    search_model->appendSearch("s");
    CU_ASSERT_STRING_EQUAL(search_model->searchText().toStdString().c_str(), "ls");
    search_model->backspaceSearch();
    CU_ASSERT_STRING_EQUAL(search_model->searchText().toStdString().c_str(), "l");
    search_model->exitSearch();
}

static void test_backspace_on_empty_exits() {
    search_model->enterSearch();
    search_model->backspaceSearch(); // empty text → exit search
    CU_ASSERT_FALSE(search_model->searchMode());
}

static void test_navigate_wraps_down() {
    search_model->enterSearch();
    search_model->appendSearch("s");
    int n = search_model->searchResults().size();
    if (n > 1) {
        for (int i = 0; i < n; ++i) search_model->navigateSearch(1);
        CU_ASSERT_EQUAL(search_model->searchSelection(), 0);
    }
    search_model->exitSearch();
}

static void test_navigate_wraps_up() {
    search_model->enterSearch();
    search_model->appendSearch("s");
    int n = search_model->searchResults().size();
    if (n > 0) {
        search_model->navigateSearch(-1); // from 0 → wraps to last
        CU_ASSERT_EQUAL(search_model->searchSelection(), n - 1);
    }
    search_model->exitSearch();
}

static void test_confirm_emits_quit() {
    // "true" is universally in PATH and exits immediately — safe to spawn in tests
    search_model->enterSearch();
    search_model->appendSearch("true");
    bool fired = false;
    auto conn = QObject::connect(search_model, &KeyTreeModel::quit, [&] { fired = true; });
    if (!search_model->searchResults().isEmpty())
        search_model->confirmSearch();
    QObject::disconnect(conn);
    if (search_model->searchMode()) search_model->exitSearch();
    CU_ASSERT_TRUE(fired);
}

// ── main ──────────────────────────────────────────────────────────────────────

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    if (CU_initialize_registry() != CUE_SUCCESS)
        return CU_get_error();

    CU_pSuite ts = CU_add_suite("tree", nullptr, nullptr);
    CU_add_test(ts, "leaf node parses",     test_leaf_parses);
    CU_add_test(ts, "subtree node parses",  test_subtree_parses);
    CU_add_test(ts, "multiple root keys",   test_multiple_root_keys);
    CU_add_test(ts, "bad path throws",      test_bad_path_throws);

    CU_pSuite ns = CU_add_suite("model_navigation", nav_init, nav_cleanup);
    CU_add_test(ns, "initial state at root",       test_initial_at_root);
    CU_add_test(ns, "unknown key ignored",         test_unknown_key_ignored);
    CU_add_test(ns, "navigate into subtree",       test_navigate_into_subtree);
    CU_add_test(ns, "back restores parent",        test_back_restores_parent);
    CU_add_test(ns, "key is case insensitive",     test_key_case_insensitive);
    CU_add_test(ns, "back at root emits quit",     test_back_at_root_emits_quit);
    CU_add_test(ns, "leaf press emits quit",       test_leaf_emits_quit);

    CU_pSuite ss = CU_add_suite("model_search", search_init, search_cleanup);
    CU_add_test(ss, "enter and exit search mode",     test_enter_exit_search);
    CU_add_test(ss, "initial results non-empty",      test_initial_results_nonempty);
    CU_add_test(ss, "append text narrows results",    test_append_narrows_results);
    CU_add_test(ss, "backspace removes character",    test_backspace_removes_char);
    CU_add_test(ss, "backspace on empty exits search",test_backspace_on_empty_exits);
    CU_add_test(ss, "navigate wraps downward",        test_navigate_wraps_down);
    CU_add_test(ss, "navigate wraps upward",          test_navigate_wraps_up);
    CU_add_test(ss, "confirm selection emits quit",   test_confirm_emits_quit);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    int failures = (int)CU_get_number_of_failures();
    CU_cleanup_registry();
    return failures;
}
