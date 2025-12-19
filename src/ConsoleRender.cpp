#include "ConsoleRender.h"
#include <cstdlib> // Для system("clear")

void ConsoleRender::Draw(const NavigationState& state)
{
    system("clear"); 

    filesystem::path path = state.GetCurrentPath();
    const vector<FileEntry>& files = state.GetCurrentFiles();

    cout << "----------------------------------------------------" << endl;
    cout << "PATH: " << path.string() << endl;
    
    if (state.GetClipboardMode() != ClipboardMode::None)
    {
        string mode = (state.GetClipboardMode() == ClipboardMode::Copy) ? "COPY" : "CUT";
        cout << "BUFFER: " << state.GetClipboard().size() << " files [" << mode << "]" << endl; 
    }
    
    cout << "----------------------------------------------------" << endl;

    for(int i = 0; i < files.size(); ++i)
    {
        if(files[i].IsSelected())
        {
            cout << "[*] ";
        }
        else
        {
            cout << "[ ] ";
        }

        if(files[i].IsDirectory())
        {
            cout << "<DIR>   ";
        }
        else
        {
            cout << "        ";
        }

        cout << i << "\t" << files[i].GetName() << "\t\t" << files[i].GetFormattedSize() << endl;
    }

    cout << "----------------------------------------------------" << endl;
    cout << "CMDS: number (go), .. (back), +num (sel), cp, cut, pst, del,cls or clearBuffer(clear copy Buffer) , exit" << endl;
}

void ConsoleRender::ShowMessage(const string& message)
{
    cout << "INFO: " << message << endl;
    
    cout << "Press ENTER to continue...";
    cin.ignore(1000, '\n'); 
    cin.get(); 
}

string ConsoleRender::ReadInput()
{
    cout << "> ";
    string input;
    cin >> input;
    return input;
}