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

using namespace std;




int main()
{   
    filesystem::path path = filesystem::current_path(); 

    FileEntry file{path, "example.txt", 1600000, FileType::RegularFile, filesystem::file_time_type::clock::now(), false};
    cout<<file.GetFormattedSize()<<endl;

    cout << "Hello, FileManager!" << endl;

    return 0;
}