#pragma once

#include <string>
#include <functional> 
#include <thread>     
#include <atomic>   

using namespace std;

using Callback = function<void()>;


/// @brief класс для мониторинга изменений в директории
///@param m_inotifyFd дескриптор inotify
///@param m_watchDescriptor дескриптор следящий за директорией
///@param m_path путь к директории
///@param m_callback функция которая будет вызываться при изменении
///@param m_worker поток для мониторинга изменений
///@param m_running статус мониторинга
class DirectoryWatcher
{
private:
    int m_inotifyFd;
    int m_watchDescriptor;

    string m_path;
    Callback m_callback;

    thread m_worker;
    atomic<bool> m_running;

    void WorkerThread();

public:

    DirectoryWatcher();
    ~DirectoryWatcher();

    /// @brief запускает мониторинг 
    /// @param path путь к директории
    void Start(const string& path);

    /// @brief останавливает мониторинг
    void Stop();

    /// @brief устанавливает функцию которая будет вызываться при изменении
    void SetCallback(Callback cb);


};
