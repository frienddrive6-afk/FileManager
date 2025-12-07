#include "NavigationState.h"
#include "FileEntry.h"

#include <iostream>
#include <vector>
#include <filesystem>
#include <cstdlib>
#include <algorithm>



NavigationState::NavigationState() :
    isCutMode{false},
    currentSort{SortMode::ByNameAsc}
{
    if(std::getenv("HOME") == nullptr) {
        this->currentPath = filesystem::current_path();
    } else {
        this->currentPath = std::getenv("HOME");
    }

}

NavigationState::NavigationState(const filesystem::path& path,bool cutMode,SortMode sortMode) :
        currentPath{path},
        isCutMode{cutMode},
        currentSort{sortMode}
{}


void NavigationState::SetPath(const filesystem::path& path)
{
    this->currentPath = path;
}

void NavigationState::Refresh(const vector<FileEntry>& files)
{
    this->currentFiles = files;
    SortFiles();
}


void NavigationState::ToggleSelection(const int index)
{
    if(this->currentFiles.size() > index && index >= 0)
    {
        this->currentFiles[index].SetSelected(!this->currentFiles[index].IsSelected());
    }
}


void NavigationState::SelectAll()
{
    for(int i{}; i < this->currentFiles.size(); ++i)
    {
        this->currentFiles[i].SetSelected(true);
    }
}


void NavigationState::ClearSelection()
{
    for(int i{}; i < this->currentFiles.size(); ++i)
    {
        this->currentFiles[i].SetSelected(false);
    }
}


void NavigationState::AddToClipboard(const filesystem::path& path)
{
    for(const auto& item : clipboard) {
        if(item == path) return;
    }
    clipboard.push_back(path);
}

void NavigationState::AddToClipboard(const vector<filesystem::path>& arrPath)
{
    for(const auto& path : arrPath) {
        AddToClipboard(path);
    }
}


void NavigationState::SetCutMode(const bool cutMode)
{
    this->isCutMode = cutMode;
}

void NavigationState::SetSortMode(SortMode mode)
{
    this->currentSort = mode;
    SortFiles();
}


filesystem::path NavigationState::GetCurrentPath() const
{
    return this->currentPath;
}


const vector<FileEntry>& NavigationState::GetCurrentFiles() const
{
    return this->currentFiles;
}


void NavigationState::SortFiles()
{
    std::sort(currentFiles.begin(), currentFiles.end(), [this](const FileEntry& a, const FileEntry& b) {
        
        if (a.IsDirectory() != b.IsDirectory()) {
            return a.IsDirectory() > b.IsDirectory(); 
        }

        switch (currentSort) {
            case SortMode::ByNameDesc: // Я-А
                return a.GetName() > b.GetName();
            
            case SortMode::BySize: 
                return a.GetSize() < b.GetSize();
                
            case SortMode::ByDate:
               return a.GetLastWriteTime() > b.GetLastWriteTime();

            case SortMode::ByNameAsc:  // Имя А-Я
            default:
                return a.GetName() < b.GetName();
        }
    });
}