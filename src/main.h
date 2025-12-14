#pragma once

#define LOG_ENABLED

enum FileType
{
    Directory,
    RegularFile,
    Symlink,
    Unknown
};


enum class ClipboardMode
{
    None,   // Буфер пуст или неактивен 
    Copy,   // Режим копирования 
    Cut     // Режим вырезания 
};




//для NavigationState
using FileComparator = bool(*)(const FileEntry&, const FileEntry&);

inline bool CompareByNameAsc(const FileEntry& a, const FileEntry& b) {
    return a.GetName() < b.GetName();
}

inline bool CompareByNameDesc(const FileEntry& a, const FileEntry& b) {
    return a.GetName() > b.GetName();
}

inline bool CompareBySizeAsc(const FileEntry& a, const FileEntry& b) {
    return a.GetSize() < b.GetSize();
}

inline bool CompareBySizeDesc(const FileEntry& a, const FileEntry& b) {
    return a.GetSize() > b.GetSize();
}

inline bool CompareByDate(const FileEntry& a, const FileEntry& b) {
    return a.GetLastWriteTime() > b.GetLastWriteTime();
}