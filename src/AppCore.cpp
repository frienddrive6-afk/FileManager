
#include "AppCore.h"
#include "ConsoleRender.h"

#include <cstdlib>
#include <filesystem>
#include <string>

using namespace std;


AppCore::AppCore() 
{

}

AppCore::~AppCore() 
{

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

    if (OnStateChanged) {
        OnStateChanged(state);
    }
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

    if (OnStateChanged) {
        OnStateChanged(state);
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

    if (OnStateChanged) {
        OnStateChanged(state);
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


void AppCore::OnRenameRequest(const string& name)
{
    filesystem::path oldPath;
    bool found = false;

    for(const FileEntry& entry : state.GetCurrentFiles())
    {
        if(entry.IsSelected())
        {
            oldPath = entry.GetPath();
            found = true;
            break; 
        }
    }

    
    if (!found) return;

    FileSystemManager::Rename(oldPath, name);

    Navigate(state.GetCurrentPath());

}

void AppCore::OnSelectAllRequest()
{
    state.SelectAll();
}



void AppCore::Run(IRenderer& io)
{
    Init(); 

    while (true)
    {
        // render->Draw(state);

        string input = io.ReadInput();

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
            io.ShowMessage("Файлы скопированы в буфер!");
        }
        else if (input == "cut") 
        {
            OnCutRequest();
            io.ShowMessage("Файлы вырезаны в буфер!");
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
            
            io.ShowMessage("Буфер очищен."); 
        }else if (input[0] == '+') 
        {
            try {
                int idx = stoi(input.substr(1));
                state.ToggleSelection(idx);
            } catch (...) {
                io.ShowMessage("Неверный формат числа!");
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
                io.ShowMessage("Неизвестная команда!");
            }
        }
    }
}




void AppCore::SetSelection(const std::vector<int>& selectedIndices)
{
    state.ClearSelection();

    for (int index : selectedIndices) {
        state.SetFileSelection(index, true);
    }
}


vector<string> AppCore::whoIsSelacted() const
{
    return state.whoIsSelacted();
}

vector<filesystem::path> AppCore::whoIsSelactedPath() const
{
    return state.whoIsSelactedPath();
}


string AppCore::getNameOnIndex(int index)
{
    return state.GetCurrentFiles()[index].GetName();
}


void AppCore::CreateFolder(string name)
{

    FileSystemManager::CreateDirectory(state.GetCurrentPath(), name);

    Navigate(state.GetCurrentPath());

}


FileProperties AppCore::GetProperties(const filesystem::path& targetPath)
{

    FileProperties props;
    
    //  Базовая инфа
    props.name = targetPath.filename().string();
    props.fullPath = targetPath.string();
    props.parentPath = targetPath.parent_path().string();
    
    if (props.name.empty()) props.name = props.fullPath;

    // Дата и Тип
    error_code ec;
    props.isDirectory = filesystem::is_directory(targetPath, ec);
    auto time = filesystem::last_write_time(targetPath, ec);
    props.dateModified = FileSystemManager::FormatTime(time);

    //  Подсчет размеров
    if (props.isDirectory) {            // Если папка  запускаем рекурсивный сканер
        DirectoryInfo stats = FileSystemManager::GetDirectoryInfo(targetPath);
        props.size = stats.totalSize;
        props.fileCount = stats.fileCount;
        props.folderCount = stats.folderCount;
    } 
    else {                              // Если файл берем размер 
        props.size = filesystem::file_size(targetPath, ec);
        props.fileCount = 0;
        props.folderCount = 0;
    }

    return props;

}