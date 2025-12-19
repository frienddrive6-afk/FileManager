
#include "AppCore.h"
#include "ConsoleRender.h"

#include <cstdlib>
#include <filesystem>
#include <string>

using namespace std;


AppCore::AppCore() 
{
    this->render = new ConsoleRender();
}

AppCore::~AppCore() 
{
    if (this->render != nullptr) {
        delete this->render;
    }
}

void AppCore::Init()
{
    if (getenv("HOME")) {
        Navigate(getenv("HOME"));
    } else {
        Navigate(filesystem::current_path());
    }
}


void AppCore::Navigate(const filesystem::path& path)
{
    state.SetPath(path);
    state.Refresh(FileSystemManager::LoadDirectory(path));
}



void AppCore::EnterDirectory(int index)
{
    if(state.GetCurrentFiles().size() > index && index >= 0)
    {
        const FileEntry& entry = state.GetCurrentFiles()[index];
        
        if(entry.IsDirectory())
        {
            Navigate(entry.GetPath());
        }
        else
        {
            #ifdef LOG_APP_CORE
            cout << "Это не директория" << endl;
            #endif
        }
    }
}



void AppCore::GoUp()
{
    Navigate(state.GetCurrentPath().parent_path());
}




void AppCore::ExecuteFile(int index)
{
    if(state.GetCurrentFiles().size() > index && index >= 0)
    {
        string extension = state.GetCurrentFiles()[index].GetPath().extension().string();

        if(extension == ".txt")
        {
            string command = "xdg-open \"" + state.GetCurrentFiles()[index].GetPath().string() + "\"";
            system(command.c_str());
        }
    }
}


void AppCore::OnDeleteRequest()
{
    for(const FileEntry& entry : state.GetCurrentFiles())
    {
        if(entry.IsSelected())
        {
            FileSystemManager::DeletePath(entry.GetPath());
        }
    }
    Navigate(state.GetCurrentPath());
}




void AppCore::OnCopyRequest()
{
    state.ClearClipboard(); 
    state.SetClipboardMode(ClipboardMode::Copy);

    filesystem::path path = state.GetCurrentPath();
    for(const FileEntry& entry : state.GetCurrentFiles())
    {
        if(entry.IsSelected())
        {
            state.AddToClipboard(entry.GetPath()); 
        }
    }
}

void AppCore::OnCutRequest()
{
    state.ClearClipboard(); 
    state.SetClipboardMode(ClipboardMode::Cut);

    filesystem::path path = state.GetCurrentPath();
    for(const FileEntry& entry : state.GetCurrentFiles())
    {
        if(entry.IsSelected())
        {
            state.AddToClipboard(entry.GetPath()); 
        }
    }
}




void AppCore::OnPasteRequest()
{
    if (state.GetClipboard().empty()) return;

    filesystem::path curr_path = state.GetCurrentPath();

    if(state.GetClipboard()[0].parent_path() == curr_path)
    {
        #ifdef LOG_APP_CORE
        cout << "Нельзя вставить файлы в ту же директорию" << endl;
        #endif
    }
    else
    {
        for(const filesystem::path& srcPath : state.GetClipboard())
        {
            filesystem::path destPath = curr_path / srcPath.filename();

            if(state.GetClipboardMode() == ClipboardMode::Copy)
            {
                FileSystemManager::Copy(srcPath, destPath);
            }
            else if(state.GetClipboardMode() == ClipboardMode::Cut)
            {
                FileSystemManager::Move(srcPath, destPath);
            }
        }
        
        if (state.GetClipboardMode() == ClipboardMode::Cut) {
            state.ClearClipboard();
        }
    }

    Navigate(state.GetCurrentPath());
}



void AppCore::Run()
{
    Init(); 

    while (true)
    {
        render->Draw(state);

        string input = render->ReadInput();

        if (input == "exit") 
        {
            break; 
        }
        else if (input == "..") 
        {
            GoUp();
        }
        else if (input == "cp") 
        {
            OnCopyRequest();
            render->ShowMessage("Файлы скопированы в буфер!");
        }
        else if (input == "cut") 
        {
            OnCutRequest();
            render->ShowMessage("Файлы вырезаны в буфер!");
        }
        else if (input == "pst") 
        {
            OnPasteRequest();
        }
        else if (input == "del") 
        {
            OnDeleteRequest();
        }else if(input == "cls" || input == "clearBuffer") 
        {
            state.ClearSelection(); 

            state.ClearClipboard();
            
            render->ShowMessage("Буфер очищен."); 
        }else if (input[0] == '+') 
        {
            try {
                int idx = stoi(input.substr(1));
                state.ToggleSelection(idx);
            } catch (...) {
                render->ShowMessage("Неверный формат числа!");
            }
        }
        else 
        {
            try {
                int idx = stoi(input);

                if(this->state.GetCurrentFiles().size() > idx && idx >= 0)
                {
                    if(this->state.GetCurrentFiles()[idx].IsDirectory())
                    {
                        EnterDirectory(idx);
                    }else
                    {
                        ExecuteFile(idx);
                    }
                    
                }

                
            } catch (...) {
                render->ShowMessage("Неизвестная команда!");
            }
        }
    }
}