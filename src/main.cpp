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
#include "NavigationState.h"
#include "AppCore.h"
#include "IRenderer.h"

//Qt
#include <QApplication>
#include "QtInterface/MainWindow.h"

using namespace std;



//главная функция программы
int main(int argc, char *argv[])
{   

    QApplication app(argc, argv);

    MainWindow window;
    window.show();


    // AppCore app;

    // app.Run();

    return app.exec();
}