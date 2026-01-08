#include <iostream>
#include <filesystem>
#include <fstream>
// #include <cstdlib> // Для getenv 
// #include <filesystemtream>
// #include<string>
// #include <cstring>
// #include<cstdarg> //для безконечных аргументов функции
#include <vector>
#include <algorithm>
// #include <unistd.h> //задежка
// #include<cmath>
// #include<math.h>
// #include <iomanip>
// #include <sstream>
// #include <ncurses.h>
// #include <chrono> 
// #include <random>

#include "main.h"
#include "FileEntry.h"
#include "FileSystemManager.h"

using namespace std;


vector<FileEntry> FileSystemManager::LoadDirectory(const filesystem::path& path)
{
    vector<FileEntry> files;

    error_code ec;
    if (!filesystem::exists(path, ec) || !filesystem::is_directory(path, ec)) {
        return files; 
    }


    auto iterator = filesystem::directory_iterator(
        path, 
        filesystem::directory_options::skip_permission_denied, 
        ec
    );


    if (ec) {
        return files; 
    }


    for (const auto& entry : iterator) {
        try {
            filesystem::path filePath = entry.path();
            string fileName = filePath.filename().string();
            
            FileType type = FileType::Unknown;
            if (entry.is_directory()) type = FileType::Directory;
            else if (entry.is_regular_file()) type = FileType::RegularFile;
            else if (entry.is_symlink()) type = FileType::Symlink;

            uintmax_t size = 0;
            if (type == FileType::RegularFile) {
                error_code size_ec;
                size = filesystem::file_size(entry, size_ec);
                if (size_ec) size = 0;
            }

            error_code time_ec;
            auto time = filesystem::last_write_time(entry, time_ec);
            if (time_ec) time = filesystem::file_time_type::min();

            files.emplace_back(filePath, fileName, size, type, time, false);
        }
        catch (...) {

            continue; 
        }
    }

    sort(files.begin(), files.end(), [](const FileEntry& a, const FileEntry& b) {
        if (a.IsDirectory() != b.IsDirectory()) {
            return a.IsDirectory() > b.IsDirectory(); 
        }

        return a.GetName() < b.GetName();
    });

    return files;
}





filesystem::path FileSystemManager::GetParentPath(const filesystem::path& path)
{
    return path.parent_path();
}


bool FileSystemManager::CheckAccess(const filesystem::path& path)
{
    std::error_code ec;
    filesystem::directory_iterator it(path, ec); 

    if (!ec) {
        return true;
    } else {
        #ifdef LOG_ENABLED
        std::cerr << "Ошибка доступа к директории \"" << path << "\": " << ec.message() << std::endl;
        #endif
        return false;
    }

}



void FileSystemManager::CreateDirectory(const filesystem::path& path,const string& name)
{
    filesystem::path newDirPath = path / name;
    error_code ec;
    filesystem::create_directory(newDirPath,ec);

    #ifdef LOG_ENABLED
    if(ec) 
    {
        cerr<<"Ошибка создания директории: "<<ec.message()<<endl;
    }else{
        cout<<"Директория успешно создана: "<<newDirPath<<endl;
    }
    #endif
}


void FileSystemManager::DeletePath(const filesystem::path& path)
{
    error_code ec;

    if (!filesystem::exists(path, ec)) {
        return;
    }

    if (filesystem::is_directory(path, ec)) {
        for(const auto& entry : filesystem::directory_iterator(path)) {
            DeletePath(entry.path());
        }
    }

    filesystem::remove(path, ec);

    #ifdef LOG_ENABLED
    if(ec) {
        cerr << "Ошибка удаления: " << path << " -> " << ec.message() << endl;
    }else{
        cout << "Удалено успешно: " << path << endl;
    }
    #endif
}


void FileSystemManager::Rename(const filesystem::path& oldPath,const string& name)
{
    filesystem::path newPath = oldPath.parent_path() / name;
    error_code ec;
    filesystem::rename(oldPath, newPath, ec);

    #ifdef LOG_ENABLED
    if(ec) {
        cerr << "Ошибка переименования: " << ec.message() << endl;
    }else{
        cout << "Переименовано успешно: " << newPath << endl;
    }
    #endif

}



void FileSystemManager::Copy(const filesystem::path& soursePath,const filesystem::path& newPath)
{
    if(!filesystem::exists(soursePath))
    {
        #ifdef LOG_ENABLED
        cerr<<"Ошибка копирования: исходный путь не существует "<<soursePath<<endl;
        #endif
        return;
    }

    error_code ec;
    filesystem::copy(soursePath,newPath,filesystem::copy_options::recursive | filesystem::copy_options::overwrite_existing,ec);


    #ifdef LOG_ENABLED
    if(ec) {
        cerr << "Ошибка копирования: " << ec.message() << endl;
    }else{
        cout << "Скопировано успешно: " << newPath << endl; 
    }
    #endif
}


void FileSystemManager::Move(const filesystem::path& soursPath,const filesystem::path& newPath)
{
    error_code ec;
    filesystem::rename(soursPath,newPath,ec);

    #ifdef LOG_ENABLED
    if(ec) {
        cerr << "Ошибка перемещения: " << ec.message() << endl;
    }else{
        cout << "Перемещено успешно: " << newPath << endl; 
    }
    #endif
}


void FileSystemManager::CreateEmtyTXTFile(const filesystem::path& path,const string& name)
{
    filesystem::path filePath = path / name;
    ofstream file(filePath);
    if(!file.is_open())
    {
        #ifdef LOG_ENABLED
        cerr<<"Ошибка создания файла: "<<filePath<<endl;
        #endif
        return;
    }else{
        file.close();
        #ifdef LOG_ENABLED
        cout<<"Файл успешно создан: "<<filePath<<endl;
        #endif
    }


}




DirectoryInfo FileSystemManager::GetDirectoryInfo(const filesystem::path& path)
{

    DirectoryInfo info = {0, 0, 0};
    
    // Проверяем существование пути и является ли он директорией
    if(!filesystem::exists(path) || !filesystem::is_directory(path))
    {
        return info;
    }

    auto options = filesystem::directory_options::skip_permission_denied;  // чтобы избежать исключений при отсутствии доступа к папкам
 

    for(const auto& enty : filesystem::recursive_directory_iterator(path, options))
    {
        std::error_code ec;
        
        if(enty.is_regular_file(ec))
        {
            info.fileCount++;
            info.totalSize += filesystem::file_size(enty.path(), ec);
            
        }else if(enty.is_directory(ec))
        {
            info.folderCount++;
        }
        
        if(ec)
        {
            #ifdef LOG_ENABLED
            cerr<<"Ошибка получения информации о директории: "<<ec.message()<<endl;
            #endif
        }

    }


    return info;

}



string FileSystemManager::FormatTime(filesystem::file_time_type ftime)
{

    // Преобразование file_time_type в time_t
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        ftime - filesystem::file_time_type::clock::now() + std::chrono::system_clock::now()
    );
    std::time_t tt = std::chrono::system_clock::to_time_t(sctp);
    
    std::tm* gmt = std::localtime(&tt);
    std::stringstream ss;
    ss.imbue(std::locale("ru_RU.UTF-8"));
    
    // Формат: %e (день), %B (полный месяц), %Y (год), %H:%M:%S (время)
    ss << std::put_time(gmt, "%e %B %Y %H:%M:%S");
    return ss.str();

}


string FileSystemManager::FormatSize(uintmax_t size)
{

    const char* suffixes[] = { "B", "KB", "MB", "GB", "TB", "PB" };
    int suffixIndex = 0;
    
    double doubleSize = static_cast<double>(size);

    while (doubleSize >= 1024.0 && suffixIndex < 5)
    {
        doubleSize /= 1024.0;
        suffixIndex++;
    }

    stringstream ss;
    ss << fixed << setprecision(2) << doubleSize << " " << suffixes[suffixIndex];
    return ss.str();


}