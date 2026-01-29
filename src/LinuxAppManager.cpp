#include "LinuxAppManager.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>   // Для getenv
#include <gio/gio.h>


const vector<string> APP_DIRS = {
    "/usr/share/applications",
    "/usr/local/share/applications",
    // ~/.local/share/applications
};


vector<AppInfo> LinuxAppManager::GetAllApps()
{
    vector<AppInfo> apps;
    vector<string> dirs = APP_DIRS;

    const char* home = getenv("HOME");
    if (home) {
        dirs.push_back(string(home) + "/.local/share/applications");
    }

    for (const auto& dirPath : dirs) {
        if (!filesystem::exists(dirPath)) continue;

        for (const auto& entry : filesystem::directory_iterator(dirPath)) {
            if (entry.path().extension() == ".desktop") {
                AppInfo app = ParseDesktopFile(entry.path());
                
                if (!app.name.empty() && !app.execCommand.empty()) {
                    apps.push_back(app);
                }
            }
        }
    }
    return apps;
}

AppInfo LinuxAppManager::ParseDesktopFile(const filesystem::path& path)
{
    AppInfo info;
    info.isTerminal = false;
    
    ifstream file(path);
    if (!file.is_open()) 
    {
        return info;
    }

    string line;
    bool isDesktopEntrySection = false;
    bool noDisplay = false;

    while (getline(file, line)) {
        if (line.find("[Desktop Entry]") != string::npos) {
            isDesktopEntrySection = true;
            continue;
        }
        
        if (line.front() == '[' && line != "[Desktop Entry]") {
            if (isDesktopEntrySection) break;
        }

        if (!isDesktopEntrySection) continue;

        
        if (line.find("Name=") == 0) {
            if (info.name.empty())
                info.name = line.substr(5); 
        }
        else if (line.find("Icon=") == 0) {
            info.iconName = line.substr(5);
        }
        else if (line.find("Exec=") == 0) {
            info.execCommand = line.substr(5);
        }
        else if (line.find("NoDisplay=true") == 0) {
            noDisplay = true;
        }
        else if (line.find("Terminal=true") == 0) {
            info.isTerminal = true;
        }
        else if (line.find("MimeType=") == 0) {
            string mimes = line.substr(9);
            info.supportedMimeTypes = SplitString(mimes, ';');
        }
    }

    if (noDisplay) {
        return AppInfo{}; 
    }

    return info;
}

vector<AppInfo> LinuxAppManager::GetAppsForFile(const filesystem::path& filePath)
{
    vector<AppInfo> allApps = GetAllApps();
    
    string mime = GetMimeType(filePath);

    std::stable_sort(allApps.begin(), allApps.end(), 
        [&mime](const AppInfo& a, const AppInfo& b) {
            bool aSupports = a.Supports(mime);
            bool bSupports = b.Supports(mime);
            
            return aSupports && !bSupports; 
        }
    );

    return allApps;
}

vector<string> LinuxAppManager::SplitString(const string& str, char delimiter)
{
    vector<string> tokens;
    string token;
    for (char ch : str) {
        if (ch == delimiter) {
            if (!token.empty()) tokens.push_back(token);
            token.clear();
        } else {
            token += ch;
        }
    }
    if (!token.empty()) tokens.push_back(token);
    return tokens;
}


string LinuxAppManager::GetMimeType(const std::filesystem::path& path)
{
    // Инициализирует GIO для работы с типами файлов g_content_type_guess принимает путь, данные и размер
    char* content_type = g_content_type_guess(path.c_str(), nullptr, 0, nullptr);
    
    if (!content_type) {
        return "application/octet-stream";
    }

    //g_content_type_get_mime_type что бы получить стандартный MIME (например, image/jpeg)
    char* mime_type = g_content_type_get_mime_type(content_type);
    
    string result = mime_type ? mime_type : "application/octet-stream";

    // Освобождаем память выделенную GLib
    g_free(content_type);
    g_free(mime_type);

    return result;
}