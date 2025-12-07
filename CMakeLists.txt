cmake_minimum_required(VERSION 3.10)

project(FileManager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(src)

add_executable(FileManager 
    src/main.cpp 
    src/FileEntry.cpp
    src/FileSystemManager.cpp
)