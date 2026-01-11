#include "DirectoryWatcher.h"
#include "main.h"

#include <sys/inotify.h>
#include <unistd.h>
#include <iostream>

DirectoryWatcher::DirectoryWatcher() :
    m_inotifyFd(-1),
    m_watchDescriptor(-1),
    m_running(false)
{}

DirectoryWatcher::~DirectoryWatcher()
{
    Stop();
}


void DirectoryWatcher::SetCallback(Callback cb)
{
    m_callback = cb;
}


void DirectoryWatcher::Start(const string& path)
{
    Stop();

    m_path = path;
    m_inotifyFd = inotify_init();
    if(m_inotifyFd < 0)
    {
        #ifdef LOG_ENABLED
        cerr<<"Ошибка инициализации inotify"<<endl;
        #endif
        return;
    }


    uint32_t mack = IN_CREATE | IN_DELETE | IN_MOVE | IN_MODIFY;

    m_watchDescriptor = inotify_add_watch(m_inotifyFd, path.c_str(), mack);
    if(m_watchDescriptor < 0)
    {
        #ifdef LOG_ENABLED
        cerr<<"Ошибка добавления дескриптора"<<endl;
        #endif
        m_inotifyFd = -1;
        return;
    }

    m_running = true;
    m_worker = thread(&DirectoryWatcher::WorkerThread, this);        // запускаем поток


}


void DirectoryWatcher::Stop()
{
    if(!m_running)
    {
        return;
    }

    m_running = false;

    if(m_inotifyFd >= 0)
    {

        if(m_watchDescriptor >= 0)
        {
            inotify_rm_watch(m_inotifyFd, m_watchDescriptor);
        }

        close(m_inotifyFd);
        m_inotifyFd = -1;
        m_watchDescriptor = -1;

    }

    if(m_worker.joinable())
    {
        m_worker.join();
    }


}


void DirectoryWatcher::WorkerThread()
{

    const int BUF_LEN = 4096;
    char buffer[BUF_LEN];

    while(m_running)
    {

        int length =read(m_inotifyFd,buffer,BUF_LEN);

        if(length < 0)
        {
            if(!m_running)
            {
                break;
            }

            #ifdef LOG_ENABLED
            cerr<<"Ошибка чтения из inotify"<<endl;
            #endif
            break;
        }


        int i = 0;
        while (i < length)
        {
            struct inotify_event* event = (struct inotify_event*) &buffer[i];

            // if(event->len > 0)
            // {

            // }
                

            i += sizeof(struct inotify_event ) + event->len;
        }
        
        if(m_running && m_callback)
        {
            m_callback();
        }



    }


}
