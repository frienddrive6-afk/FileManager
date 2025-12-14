#pragma once

#define LOG_ENABLED
#define LOG_APP_CORE

#include "FileEntry.h"



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