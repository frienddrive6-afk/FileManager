#pragma once

#include <filesystem>
#include <string>


#include "Types.h"

using namespace std;


/// @brief класс представляющий файл или директорию в файловой системе
/// - @param path: Полный путь к файлу (std::filesystem::path)
/// - @param name: Имя файла для отображения (string)
/// - @param size: Размер в байтах (uintmax_t)
/// - @param type: Тип файла (Directory, RegularFile, Symlink, Unknown)
/// - @param lastWriteTime: Время последнего изменения
/// - @param isSelected: Флаг, выделен ли файл в интерфейсе
class FileEntry
{
private:
    filesystem::path path;                                  
    string name;                                                                               
    uintmax_t size;                                         
    FileType type;                                          
    filesystem::file_time_type lastWriteTime;               
    bool isSelected;                                          

public:

    FileEntry();
    FileEntry(filesystem::path p,string n,uintmax_t s,FileType t,filesystem::file_time_type lwt,bool sel);

    /// @brief эта функция проверяет является ли файл директорией
    /// @return true если файл является директорией иначе false
    bool IsDirectory() const;

    /// @brief нужна для получения размера файла в удобночитаемом формате
    /// @return строку с размером файла в формате B, KB, MB, GB пример "1.23 MB"
    string GetFormattedSize() const;

    
    /// @return имя файла обьекта FileEntry
    string GetName() const;

    /// @return путь к файлу 
    filesystem::path GetPath() const;


    /// @return размер файла в байтах
    uintmax_t GetSize() const;

    /// @return тип файла типа FileType (Directory, RegularFile, Symlink, Unknown)
    FileType GetType() const;


    /// @return выбран ли файл в интерфейсе
    bool IsSelected() const;

    /// @brief установить статус выделения файла выделенным или нет
    /// @param value true если файл должен быть выделен иначе false
    void SetSelected(bool value);

    /// @return время последнего изменения файла
    filesystem::file_time_type GetLastWriteTime() const;
    


};