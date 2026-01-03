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
#include "ConsoleRender.h"

//Qt
#include <QApplication>
#include "QtInterface/MainWindow.h"

using namespace std;




void consoleMode()
{

    AppCore core;
    ConsoleRender console;

    core.OnStateChanged = [&](const NavigationState& state) {
        console.Draw(state); 
    };

    core.Run(console);

}

//главная функция программы
int main(int argc, char *argv[])
{   
    //Вход в консольный режим без Qt
    if (argc > 1 && string(argv[1]) == "-console") {
        consoleMode(); 
        return 0;
    }

    QApplication app(argc, argv);

    AppCore core;

    MainWindow window(core); 
    window.show();

    core.OnStateChanged = [&](const NavigationState& state)
    {
        window.updateView(state);
    };

    core.Init(); 

    return app.exec();
}