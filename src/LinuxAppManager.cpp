#include "LinuxAppManager.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>   // Для getenv
#include <gio/gio.h>
#include <unordered_set>



vector<AppInfo> LinuxAppManager::GetAllApps()
{
    vector<AppInfo> apps;

    GList* appList = g_app_info_get_all();

    for (GList* l = appList; l != nullptr; l = l->next) 
    {
        GAppInfo* gApp = G_APP_INFO(l->data);

        if (!g_app_info_should_show(gApp)) continue;

        AppInfo info;
        
        // Имя например Visual Studio Code
        const char* name = g_app_info_get_name(gApp);
        info.name = name ? string(name) : "Unknown";

        // Команда запуска например code %f
        const char* exec = g_app_info_get_commandline(gApp);
        info.execCommand = exec ? string(exec) : "";

        // Иконка
        GIcon* gIcon = g_app_info_get_icon(gApp);
        if (gIcon) {
            gchar* iconStr = g_icon_to_string(gIcon);
            info.iconName = iconStr ? string(iconStr) : "";
            g_free(iconStr);
        }


        apps.push_back(info);
    }

    // Чистим список и уменьшаем счетчик ссылок у объектов внутри
    g_list_free_full(appList, g_object_unref);

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
    vector<AppInfo> resultApps;
    unordered_set<string> addedIds; 

    // Определяем MIME-тип
    gboolean uncertain;
    gchar* mimeType = g_content_type_guess(filePath.c_str(), nullptr, 0, &uncertain);
    
    // Сначала получаем релевантные приложения (верх списка)
    GList* recommended = g_app_info_get_all_for_type(mimeType);
    
    // Получаем вообще все приложения в системе
    GList* allApps = g_app_info_get_all();

    auto processApp = [&](GAppInfo* gApp) {
        const char* id = g_app_info_get_id(gApp);
        string idStr = id ? id : "";

        // Если приложения в список если его ещё нет
        if (!idStr.empty() && addedIds.find(idStr) == addedIds.end() && g_app_info_should_show(gApp)) {
            AppInfo info;
            info.name = g_app_info_get_name(gApp);
            
            const char* exec = g_app_info_get_commandline(gApp);
            info.execCommand = exec ? string(exec) : "";

            GIcon* gIcon = g_app_info_get_icon(gApp);
            if (gIcon) {
                gchar* iconStr = g_icon_to_string(gIcon);
                info.iconName = iconStr ? string(iconStr) : "";
                g_free(iconStr);
            }

            resultApps.push_back(info);
            addedIds.insert(idStr);
        }
    };

    // Сначала проходим по рекомендованным
    for (GList* l = recommended; l != nullptr; l = l->next) {
        processApp(G_APP_INFO(l->data));
    }

    // Затем по всем остальным 
    for (GList* l = allApps; l != nullptr; l = l->next) {
        processApp(G_APP_INFO(l->data));
    }

    // Чистка памяти
    g_list_free_full(recommended, g_object_unref);
    g_list_free_full(allApps, g_object_unref);
    g_free(mimeType);

    return resultApps;
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