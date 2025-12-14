#pragma once

#include <iostream>
#include <vector>
#include <filesystem>
#include "FileEntry.h"
#include "main.h"


using namespace std;

/// @brief класс представляющий состояние навигации в файловой системе
/// - @param currentPath: текущий путь навигации (std::filesystem::path)
/// - @param currentFiles: текущий список файлов (vector<FileEntry>)
/// - @param clipboard: буфер обмена (vector<filesystem::path>)
/// - @param clipboardMode: текущий режим буфера обмена (None, Copy, Cut)
///- @param currentSort: текущий режим сортировки
class NavigationState
{
private:
    filesystem::path currentPath;
    vector<FileEntry> currentFiles;
    vector<filesystem::path> clipboard;
    ClipboardMode clipboardMode;                            
    FileComparator currentSortAlgo;

public:
    //конструктор без параметров устанавливает текущий путь в домашнюю директорию пользователя
    NavigationState();

    //конструктор с параметрами
    NavigationState(const filesystem::path& path,ClipboardMode clipboardMode = ClipboardMode::None,FileComparator sortAlgo = CompareByNameAsc);

    /// @brief устанавливает текущий путь навигации
    /// @param path новый текущий путь
    void SetPath(const filesystem::path& path);
   
    /// @brief устанавливает текущий список файлов в навигации
    void SetCurrentFiles(const vector<FileEntry>& files);

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

    /// @brief устанавливает режим буфера обмена
    void SetClipboardMode(ClipboardMode mode);

    /// @return текущий режим буфера обмена
    ClipboardMode GetClipboardMode() const;

    /// @brief очищает буфер обмена
    void ClearClipboard();

    /// @return буфер обмена
    const vector<filesystem::path>& GetClipboard() const;

    /// @brief устанавливает текущий режим сортировки
    void SetSortAlgo(FileComparator algo);

    /// @return текущий путь навигации
    filesystem::path GetCurrentPath() const;

    /// @return список текущих файлов
    const vector<FileEntry>& GetCurrentFiles() const;

    /// @brief сортирует текущие файлы в соответствии с текущим режимом сортировки
    void SortFiles();






};