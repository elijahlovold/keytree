cmake_minimum_required(VERSION 3.24)
project(keytree VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 6.2 REQUIRED COMPONENTS Core Quick)
find_package(X11 REQUIRED)  # linked for future X11-specific code (window type atoms, etc.)

include(FetchContent)
FetchContent_Declare(
    tomlplusplus
    GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
    GIT_TAG        v3.4.0
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(tomlplusplus)

qt_standard_project_setup()
qt_policy(SET QTP0001 NEW)
qt_policy(SET QTP0004 NEW)

qt_add_executable(keytree
    src/main.cpp
    src/tree.cpp
    src/executor.cpp
    src/model.cpp
)

qt_add_qml_module(keytree
    URI KeyTree
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/RadialView.qml
        qml/SearchView.qml
)

target_link_libraries(keytree PRIVATE
    Qt6::Core
    Qt6::Quick
    tomlplusplus::tomlplusplus
    ${X11_LIBRARIES}
)

target_include_directories(keytree PRIVATE
    src
    ${X11_INCLUDE_DIR}
)

install(TARGETS keytree RUNTIME DESTINATION bin)

# ── Tests ─────────────────────────────────────────────────────────────────────
find_package(PkgConfig REQUIRED)
pkg_check_modules(CUNIT REQUIRED cunit)

add_executable(keytree_tests
    tests/tests.cpp
    src/tree.cpp
    src/model.cpp
    src/executor.cpp
)
target_link_libraries(keytree_tests PRIVATE
    Qt6::Core
    tomlplusplus::tomlplusplus
    ${CUNIT_LIBRARIES}
)
target_include_directories(keytree_tests PRIVATE
    src
    ${CUNIT_INCLUDE_DIRS}
)
target_compile_options(keytree_tests PRIVATE ${CUNIT_CFLAGS_OTHER})

enable_testing()
add_test(NAME unit COMMAND keytree_tests)
