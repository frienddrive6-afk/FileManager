#pragma once

#include <vector>
#include <filesystem>
#include "FileEntry.h"

using namespace std;

/// @brief класс для управления файловой системой
class FileSystemManager
{
public:
    /// @brief сканирует директорию и возвращает список файлов и папок в ней типа FileEntry
    /// @param path путь к директории для сканирования
    static vector<FileEntry> LoadDirectory(const filesystem::path& path);

    /// @brief получает родительский путь для указанного пути
    /// @param path путь для получения родительского пути
    static filesystem::path GetParentPath(const filesystem::path& path);

    /// @brief проверяет есть ли доступ к указанной директории
    /// @param path путь к директории для проверки доступа
    /// @return true если доступ есть иначе false
    static bool CheckAccess(const filesystem::path& path);

    /// @brief создает новую директорию с указанным именем в указанном пути
    /// @param path путь к родительской директории где будет создана новая папка
    /// @param name имя новой директории
    static void CreateDirectory(const filesystem::path& path,const string& name);

    /// @brief рекурсивно удаляет файл или директорию по указанному пути
    /// @param path путь к файлу или директории для удаления
    static void DeletePath(const filesystem::path& path);

    /// @brief переименовывает файл или директорию по указанному пути
    /// @param oldPath путь к файлу или директории для переименования
    /// @param name новое имя файла или директории
    static void Rename(const filesystem::path& oldPath,const string& name);

    /// @brief копирует файл или директорию в новое место
    /// @param soursePath путь к файлу или директории для копирования
    /// @param newPath путь к новому месту назначения
    static void Copy(const filesystem::path& soursePath,const filesystem::path& newPath);

    /// @brief перемещает файл или директорию в новое место
    /// @param soursPath путь к файлу или директории для перемещения
    /// @param newPath путь к новому месту назначения
    static void Move(const filesystem::path& soursPath,const filesystem::path& newPath);

    /// @brief создает пустой текстовый файл (.txt) в указанной директории с указанным именем
    /// @param path путь к директории где будет создан файл
    /// @param name имя создаваемого текстового файла
    static void CreateEmtyTXTFile(const filesystem::path& path,const string& name);


    /// @brief получает информацию о директории
    /// @param path путь к директории
    /// @return информация о директории
    static DirectoryInfo GetDirectoryInfo(const filesystem::path& path);

    /// @brief форматирует время
    /// @param ftime время
    /// @return форматированное время
    static string FormatTime(filesystem::file_time_type ftime);

    /// @brief форматирует размер
    /// @param size размер
    /// @return форматированный размер
    static string FormatSize(uintmax_t size);
};