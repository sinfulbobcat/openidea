cmake_minimum_required(VERSION 3.16)

project(openidea_qt_rewrite VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if (TARGET Qt6::Widgets)
    find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
    set(QT_LIBS Qt6::Core Qt6::Gui Qt6::Widgets)
else()
    find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)
    set(QT_LIBS Qt5::Core Qt5::Gui Qt5::Widgets)
endif()

add_executable(openidea_qt_rewrite
    src/main.cpp
    src/AppTheme.hpp
    src/AppTheme.cpp
    src/AppSettings.hpp
    src/AppSettings.cpp
    src/WorkspaceModel.hpp
    src/WorkspaceModel.cpp
    src/ProjectExplorer.hpp
    src/ProjectExplorer.cpp
    src/EditorPanel.hpp
    src/EditorPanel.cpp
    src/BuildRunner.hpp
    src/BuildRunner.cpp
    src/TerminalPane.hpp
    src/TerminalPane.cpp
    src/StatusConsole.hpp
    src/StatusConsole.cpp
    src/MainWindow.hpp
    src/MainWindow.cpp
)

target_link_libraries(openidea_qt_rewrite PRIVATE ${QT_LIBS})

target_include_directories(openidea_qt_rewrite PRIVATE src)

if (POLICY CMP0071)
    cmake_policy(SET CMP0071 NEW)
endif()
