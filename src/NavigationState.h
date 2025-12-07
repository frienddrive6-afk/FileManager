#pragma once

#include <iostream>
#include <vector>
#include <filesystem>
#include "FileEntry.h"
#include "main.h"


using namespace std;


class NavigationState
{
private:
    filesystem::path currentPath;
    vector<FileEntry> currentFiles;
    vector<filesystem::path> clipboard;
    bool isCutMode;
    SortMode currentSort;

public:
    //конструктор без параметров устанавливает текущий путь в домашнюю директорию пользователя
    NavigationState();

    //конструктор с параметрами
    NavigationState(const filesystem::path& path,bool cutMode = false,SortMode sortMode = SortMode::ByNameAsc);

    /// @brief устанавливает текущий путь навигации
    /// @param path новый текущий путь
    void SetPath(const filesystem::path& path);

    /// @brief обновляет список текущих файлов в навигации
    /// @param files новый список файлов типа FileEntry
    void Refresh(const vector<FileEntry>& files);

    /// @brief переключает статус выделения файла по индексу в текущем списке файлов
    /// @param index индекс файла в текущем списке файлов
    void ToggleSelection(const int index);

    /// @brief выделяет все файлы в текущем списке файлов
    void SelectAll();

    /// @brief снимает выделение со всех файлов в текущем списке файлов
    void ClearSelection();

    /// @brief добавляет путь к файлу или директории в буфер обмена
    /// @param path путь к файлу или директории для добавления в буфер обмен
    void AddToClipboard(const filesystem::path& path);

    /// @brief добавляет массив путей к файлам или директориям в буфер обмена
    /// @param arrPath массив путей к файлам или директориям для добавления в буфер обмена
    void AddToClipboard(const vector<filesystem::path>& arrPath);

    /// @brief Выбирает режим вырезания или копирования для буфера обмена
    /// @param cutMode true для режима вырезания, false для режима копирования
    void SetCutMode(const bool cutMode);

    void SetSortMode(SortMode mode);

    /// @return текущий путь навигации
    filesystem::path GetCurrentPath() const;

    /// @return текущий режим буфера обмена (вырезание или копирование)
    const vector<FileEntry>& GetCurrentFiles() const;

    /// @brief сортирует текущие файлы в соответствии с текущим режимом сортировки
    void SortFiles();






};