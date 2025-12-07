#pragma once

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