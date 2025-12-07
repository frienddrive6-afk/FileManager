#include <iostream>
#include <filesystem>
// #include <cstdlib> // Для getenv 
// #include <fstream>
// #include<string>
// #include <cstring>
// #include<cstdarg> //для безконечных аргументов функции
// #include <vector>
// #include <algorithm>
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




int main()
{   
    #ifdef LOG_ENABLED
    cout<<"Логирование включено"<<endl;

    //проверка GetFormattedSize для размера файла
    // filesystem::path path = filesystem::current_path(); 
    filesystem::path path = "/home/nomantb/Загрузки/test_file_Manedger";

    FileEntry file{path, "example.txt", 1600000, FileType::RegularFile, filesystem::file_time_type::clock::now(), false};
    cout<<file.GetFormattedSize()<<endl;


    //проверка сканирования директории
    FileSystemManager fsm;
    vector<FileEntry> entries = fsm.LoadDirectory(path);
    for (const auto& entry : entries) {
        cout << entry.GetName() << " - " << entry.GetFormattedSize() << endl;
    }
    
    //проверка получения родительского пути
    cout<<(string)fsm.GetParentPath(path)<<endl;

    cout<< [&fsm,&path]() -> string {
        if(fsm.CheckAccess(path)) {
            return "true у меня есть доступ к этой папке";
        } else {
            return "false у меня нет доступа к этой папке";
        }
    }()   <<endl;

    //проверка создания директории
    fsm.CreateDirectory(path,"NewFolderTestWithCode");

    fsm.DeletePath(path / "NewFolderTestWithCode");


    fsm.CreateDirectory(path,"RenameTestFolder");
    fsm.Rename(path / "RenameTestFolder","ThisFolderRenamed");

    fsm.Copy(path / "ThisFolderRenamed",path / "CopiedFolderTest");

    fsm.CreateEmtyTXTFile(path,"CreatedEmptyFile.txt");

    fsm.Move(path / "CreatedEmptyFile.txt",path / "ThisFolderRenamed/CreatedEmptyFile.txt");

    



    cout << "Hello, FileManager!" << endl;
    #endif

    return 0;
}