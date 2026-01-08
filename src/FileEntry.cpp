#include <iostream>
// #include <filesystem>
// #include <cstdlib> // Для getenv 
// #include <fstream>
// #include<string>
// #include <cstring>
// #include<cstdarg> //для безконечных аргументов функции
// #include <vector>
// #include <algorithm>
// #include <unistd.h> //задежка
// #include<cmath>
// #include<math.h>
// #include <iomanip>
// #include <sstream>
// #include <ncurses.h>
// #include <chrono> 
// #include <random>

#include "main.h"
#include "FileEntry.h"
#include "FileSystemManager.h"

using namespace std;



FileEntry::FileEntry() :
    path{},
    name{},
    size{0},
    type{FileType::Unknown},
    lastWriteTime{},
    isSelected{false}
{}

FileEntry::FileEntry(filesystem::path p,string n,uintmax_t s,FileType t,filesystem::file_time_type lwt,bool sel) :
    path{p},
    name{n},
    size{s},
    type{t},
    lastWriteTime{lwt},
    isSelected{sel}
{}

bool FileEntry::IsDirectory() const
{
    return type == FileType::Directory;
}


string FileEntry::GetFormattedSize() const
{
    if (IsDirectory()) return "<DIR>"; 

    return FileSystemManager::FormatSize(this->size);
}

string FileEntry::GetName() const
{
    return this->name;
}

filesystem::path FileEntry::GetPath() const
{
    return this->path;
}

uintmax_t FileEntry::GetSize() const
{
    return this->size;
}

FileType FileEntry::GetType() const
{
    return this->type;
}

bool FileEntry::IsSelected() const
{
    return this->isSelected;
}

void FileEntry::SetSelected(bool value)
{
    this->isSelected = value;
}


filesystem::file_time_type FileEntry::GetLastWriteTime() const
{
    return this->lastWriteTime;
}