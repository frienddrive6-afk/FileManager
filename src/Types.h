#pragma once

//Описывает типы файлов (директория, регулярный файл, символьная ссылка, неизвестно)
enum FileType
{
    Directory,
    RegularFile,
    Symlink,
    Unknown
};

//Показывает режим работы буфера обмена (копирование, вырезание, ничего)
enum class ClipboardMode
{
    None,   // Буфер пуст или неактивен 
    Copy,   // Режим копирования 
    Cut     // Режим вырезания 
};

//Нужен для возврата нужной информации о Папке
struct DirectoryInfo {
    uintmax_t totalSize; // Общий размер в байтах
    size_t fileCount;    // Количество файлов
    size_t folderCount;  // Количество подпапок
};


//Нужен для возврата нужной информации о Файле
struct FileProperties {
    std::string name;            // Имя 
    std::string parentPath;      // Где лежит path
    std::string fullPath;        // Полный путь
    
    uintmax_t size;         // Размер в байтах
    size_t fileCount;       // 0 если это файл
    size_t folderCount;     // 0 если это файл
    
    std::string dateModified;    // отформатированная дата
    bool isDirectory;       // является ли директорией
    
    FileProperties() : size(0), fileCount(0), folderCount(0), isDirectory(false) {}
};


//Структура описывающая ассоциацию расширений с командами
struct FileAssociation
{
    std::string extension;
    std::string command;
    bool runInTerminal;
};