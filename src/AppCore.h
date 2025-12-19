#pragma once

#include "NavigationState.h"
#include "FileSystemManager.h"
#include "IRenderer.h"


/// @brief класс основной логики программы
///@param state состояние навигации в файловой системе
///@param render интерфейс рендеринга
class AppCore
{
private:
    NavigationState state;
    IRenderer* render;

public:
    AppCore();

    


    void Init();

    /// @brief переходит в указанную директорию
    /// @param path путь к директории
    void Navigate(const filesystem::path& path);

    /// @brief переходит в папку по указаному индексу в  state vector<FileEntry> currentFiles 
    /// @param index индекс в  state vector<FileEntry> currentFiles
    void EnterDirectory(int index);

    /// @brief переходит в родительскую директорию
    void GoUp();

    /// @brief выполняет выбранный файл если для него написана логика в функции
    void ExecuteFile(int index);

    /// @brief удаляет выделеный файл или группу файлов из текущей директории
    void OnDeleteRequest();

    /// @brief копирует выделеный файл или группу файлов в буфер обмена
    void OnCopyRequest();

    void OnCutRequest();

    /// @brief вставляет выделеный файл или группу файлов из буфера обмена
    void OnPasteRequest();


    /// @brief запускает основной цикл программы в котором происходит рендеринг интерфейса и взаимодействие с пользователем
    void Run();

    ~AppCore();

};