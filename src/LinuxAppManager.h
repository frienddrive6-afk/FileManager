#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include "Types.h"

using namespace std;

class LinuxAppManager
{
public:

    /// @brief Сканирует систему и возвращает список ВСЕХ GUI приложений
    static vector<AppInfo> GetAllApps();

    /// @brief Возвращает список приложений, отсортированный по релевантности к файлу
    /// @param filePath Путь к файлу, который хотим открыть
    static vector<AppInfo> GetAppsForFile(const filesystem::path& filePath);


private:

    /// @brief Парсит один .desktop файл
    static AppInfo ParseDesktopFile(const filesystem::path& path);

    /// @brief Помощник: разбивает строку по разделителю (для MimeType)
    static vector<string> SplitString(const string& str, char delimiter);

    /// @brief Попытка угадать MimeType файла по расширению (простая реализация)
    static string GetMimeType(const filesystem::path& path);

};