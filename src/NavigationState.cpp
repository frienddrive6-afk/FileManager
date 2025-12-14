#include "NavigationState.h"
#include "FileEntry.h"
#include "main.h"
#include "Types.h"

#include <iostream>
#include <vector>
#include <filesystem>
#include <cstdlib>
#include <algorithm>



NavigationState::NavigationState() :
    clipboardMode{ClipboardMode::None},
    currentSortAlgo{CompareByNameAsc}
{
    if(std::getenv("HOME") == nullptr) {
        this->currentPath = filesystem::current_path();
    } else {
        this->currentPath = std::getenv("HOME");
    }

}

NavigationState::NavigationState(const filesystem::path& path, ClipboardMode clipboardMode, FileComparator sortAlgo) :
        currentPath{path},
        clipboardMode{clipboardMode},
        currentSortAlgo{sortAlgo}
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


void NavigationState::SetSortAlgo(FileComparator algo)
{
    this->currentSortAlgo = algo;
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



void NavigationState::SetClipboardMode(ClipboardMode mode)
{
    this->clipboardMode = mode;
}

ClipboardMode NavigationState::GetClipboardMode() const
{
    return this->clipboardMode;
}




void NavigationState::ClearClipboard()
{
    this->clipboard.clear();
    this->clipboardMode = ClipboardMode::None;
}


const vector<filesystem::path>& NavigationState::GetClipboard() const
{
    return this->clipboard;
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
    if (currentSortAlgo == nullptr) return;

    std::sort(currentFiles.begin(), currentFiles.end(), 
        [this](const FileEntry& a, const FileEntry& b) -> bool 
    {
        if (a.IsDirectory() != b.IsDirectory()) {
            return a.IsDirectory() > b.IsDirectory(); 
        }

        return this->currentSortAlgo(a, b);
    });
}