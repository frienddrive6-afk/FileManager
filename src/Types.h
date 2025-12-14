#pragma once

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