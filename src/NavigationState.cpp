#include "NavigationState.h"
#include "FileEntry.h"
#include "main.h"
#include "Types.h"
#include "FileSystemManager.h"

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
    if(this->currentPath != path)
    {
        if(!currentPath.empty())
        {
            m_backHistory.push_back(currentPath);
        }

        m_forwardHistory.clear();

        this->currentPath = path;
    }

}

void NavigationState::SetCurrentFiles(const vector<FileEntry>& files)
{
    this->currentFiles.clear();
    this->currentFiles = files;
    SortFiles();

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


vector<string> NavigationState::whoIsSelacted() const
{
    vector<string> selectedFiles;

    for(FileEntry file : currentFiles)
    {
        if(file.IsSelected())
        {
            string temp = file.GetPath().string();
            selectedFiles.push_back(temp);
        }
    }

    return selectedFiles;
}

vector<filesystem::path> NavigationState::whoIsSelactedPath() const
{

    vector<filesystem::path> selectedFiles;

    for(FileEntry file : currentFiles)
    {
        if(file.IsSelected())
        {
            selectedFiles.push_back(file.GetPath());
        }
    }

    return selectedFiles;
}


void NavigationState::SetFileSelection(int index, bool selected)
{
    if (index >= 0 && index < currentFiles.size())
    {
        currentFiles[index].SetSelected(selected);
    }
}




void NavigationState::PushToBackHistory(const filesystem::path& path)
{
    m_backHistory.push_back(path);
}


filesystem::path NavigationState::PopBackHistory()
{
    filesystem::path path = m_backHistory.back();
    m_backHistory.pop_back();
    return path;
}


void NavigationState::PushToForwardHistory(const filesystem::path& path)
{
    m_forwardHistory.push_back(path);
}


filesystem::path NavigationState::PopForwardHistory()
{
    filesystem::path path = m_forwardHistory.back();
    m_forwardHistory.pop_back();
    return path;
}

void NavigationState::ClearForwardHistory()
{
    m_forwardHistory.clear();
}


bool NavigationState::CanGoBack() const
{
    return !m_backHistory.empty();
}

bool NavigationState::CanGoForward() const
{
    return !m_forwardHistory.empty();
}


void NavigationState::StepBack()
{
    if(m_backHistory.empty())
    {
        return;
    }

    m_forwardHistory.push_back(this->currentPath);

    this->currentPath = m_backHistory.back();

    #ifdef LOG_ENABLED
    cout << "StepBack: " << this->currentPath << endl;
    #endif

    m_backHistory.pop_back();
}


void NavigationState::StepForward()
{
    if(m_forwardHistory.empty())
    {
        return;
    }

    m_backHistory.push_back(this->currentPath);

    this->currentPath = m_forwardHistory.back();

    #ifdef LOG_ENABLED
    cout << "StepForward: " << this->currentPath << endl;
    #endif

    m_forwardHistory.pop_back();
}