cmake_minimum_required(VERSION 3.16)

project(filemanager VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

include_directories(src)
include_directories(src/QtInterface)

add_executable(filemanager
    src/main.cpp
    
    src/resources.qrc

    src/AppCore.cpp
    src/FileSystemManager.cpp
    src/NavigationState.cpp
    src/FileEntry.cpp
    src/ConsoleRender.cpp

    src/QtInterface/MainWindow.h
    src/QtInterface/MainWindow.cpp
    src/QtInterface/FileListModel.h
    src/QtInterface/FileListModel.cpp
    src/QtInterface/MainWindow_UI.cpp
    src/QtInterface/MainWindow_Slots.cpp
    src/DirectoryWatcher.cpp
    src/DirectoryWatcher.h
)

target_link_libraries(filemanager PRIVATE Qt6::Widgets)