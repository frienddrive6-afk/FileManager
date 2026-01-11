#pragma once

#include "NavigationState.h"
#include "FileSystemManager.h"
#include "IRenderer.h"
#include "DirectoryWatcher.h"

#include <functional>


/// @brief класс основной логики программы
///@param state состояние навигации в файловой системе
///@param OnStateChanged функция которая будет вызываться при изменении состояния
///@param m_watcher мониторинг изменений
class AppCore
{
private:
    NavigationState state;
    DirectoryWatcher m_watcher;
    // IRenderer* render;
    

public:
    /// @brief функция которая будет вызываться при изменении состояния
    std::function<void(const NavigationState&)> OnStateChanged;


    AppCore();

    /// @brief возвращает текущее состояние навигации
    /// @return константная ссылка на NavigationState
    const NavigationState& GetState() const 
    { 
        return state; 
    }



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

    /// @brief вырезает выделеный файл или группу файлов в буфер обмена
    void OnCutRequest();

    /// @brief вставляет выделеный файл или группу файлов из буфера обмена
    void OnPasteRequest();

    /// @brief переименовывает выделеный файл через FileSystemManager
    /// @param name новое имя
    void OnRenameRequest(const string& name);

    /// @brief выделяет все файлы
    void OnSelectAllRequest();


    /// @brief запускает основной цикл программы в котором происходит рендеринг интерфейса и взаимодействие с пользователем
   void Run(IRenderer& io);

    ~AppCore();

    /// @brief устанавливает выделение на указанных индексах
    /// @param selectedIndices индексы которым нужно установить выделение
    void SetSelection(const std::vector<int>& selectedIndices);

    /// @return возвращает индексы выделенных файлов
    vector<string> whoIsSelacted() const;
    
    vector<filesystem::path> whoIsSelactedPath() const;

    /// @brief возвращает имя указанного индекса
    /// @param index индекс
    string getNameOnIndex(int index);

    /// @brief создает папку
    /// @param name имя папки
    void CreateFolder(string name);


    /// @brief Возвращает заполненную структуру со всей инфой
    /// @param targetPath путь к директории
    FileProperties GetProperties(const filesystem::path& targetPath);


};