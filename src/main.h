#pragma once

#define LOG_ENABLED

enum FileType
{
    Directory,
    RegularFile,
    Symlink,
    Unknown
};

enum SortMode
{
    ByNameAsc,
    ByNameDesc,
    BySize,
    ByDate
};