cmake_minimum_required(VERSION 3.16)

project(filemanager VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(MY_LOCAL_QT "$ENV{HOME}/Qt/6.8.0/gcc_64")

if(EXISTS "${MY_LOCAL_QT}")
    message(STATUS "Найден локальный Qt 6.8: ${MY_LOCAL_QT}")
    list(PREPEND CMAKE_PREFIX_PATH "${MY_LOCAL_QT}")
endif()

find_package(Qt6 REQUIRED COMPONENTS Widgets Concurrent)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GIO REQUIRED gio-2.0)

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
    src/QtInterface/SettingsDialog.h
    src/QtInterface/SettingsDialog.cpp
    src/LinuxAppManager.h
    src/LinuxAppManager.cpp
    src/QtInterface/OpenWithDialog.h
    src/QtInterface/OpenWithDialog.cpp
)

target_include_directories(filemanager PRIVATE ${GIO_INCLUDE_DIRS})

target_link_libraries(filemanager PRIVATE 
    Qt6::Widgets 
    ${GIO_LIBRARIES}
    Qt6::Concurrent
)