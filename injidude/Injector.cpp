#include <windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <iostream>
#include <string>
#include <commdlg.h>

DWORD GetProcessID(const wchar_t* processName) {
    DWORD processID = 0;
    PROCESSENTRY32 pe32;
    HANDLE hProcessSnap;

    pe32.dwSize = sizeof(PROCESSENTRY32);
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (Process32First(hProcessSnap, &pe32)) {
        do {
            if (_wcsicmp(pe32.szExeFile, processName) == 0) {
                processID = pe32.th32ProcessID;
                break;
            }
        } while (Process32Next(hProcessSnap, &pe32));
    }

    CloseHandle(hProcessSnap);
    return processID;
}

bool InjectDLL(DWORD processID, const char* dllPath) {
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    if (hProcess == NULL) return false;

    // Convert char* dllPath to wchar_t* for compatibility
    int len = MultiByteToWideChar(CP_ACP, 0, dllPath, -1, NULL, 0);
    wchar_t* wideDllPath = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, dllPath, -1, wideDllPath, len);

    LPVOID allocMem = VirtualAllocEx(hProcess, NULL, (len + 1) * sizeof(wchar_t), MEM_COMMIT, PAGE_READWRITE);
    if (allocMem == NULL) {
        delete[] wideDllPath;
        return false;
    }

    WriteProcessMemory(hProcess, allocMem, wideDllPath, (len + 1) * sizeof(wchar_t), NULL);

    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0,
        (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryW"),
        allocMem, 0, NULL);

    delete[] wideDllPath;

    if (hThread == NULL) return false;

    WaitForSingleObject(hThread, INFINITE);

    VirtualFreeEx(hProcess, allocMem, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    return true;
}

std::string OpenFileDialog() {
    char fileName[MAX_PATH] = "";
    OPENFILENAMEA ofn;  // Use OPENFILENAMEA for ANSI (narrow strings)
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = "DLL Files\0*.dll\0All Files\0*.*\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "dll";

    if (GetOpenFileNameA(&ofn)) {  // Use GetOpenFileNameA for ANSI (narrow strings)
        return std::string(fileName);
    }

    return "";
}


void PrintIntro() {
    // ANSI escape code for orange color (ANSI 38;5;214 represents a close approximation to orange)
    const std::string orange = "\033[38;5;214m";
    // ANSI escape code to reset color
    const std::string reset = "\033[0m";

    std::cout << orange
        << " ______                        __              __            \n"
        << "/\\__  _\\            __  __    /\\ \\            /\\ \\           \n"
        << "\\/_/\\ \\/     ___   /\\_\\/\\_\\   \\_\\ \\  __  __   \\_\\ \\     __   \n"
        << "   \\ \\ \\   /' _ `\\ \\/\\ \\/\\ \\  /'_` \\/\\ \\/\\ \\  /'_` \\  /'__`\\ \n"
        << "    \\_\\ \\__/\\ \\/\\ \\ \\ \\ \\ \\ \\/\\ \\L\\ \\ \\ \\_\\ \\/\\ \\L\\ \\/\\  __/ \n"
        << "    /\\_____\\ \\_\\ \\_\\_\\ \\ \\ \\_\\ \\___,_\\ \\____/\\ \\___,_\\ \\____\\\n"
        << "    \\/_____/\\/_/\\/_/\\ \\_\\ \\/_/\\/__/,_ /\\/___/  \\/__,_ /\\/____/\n"
        << "                   \\ \\____/ Created by ya boy wehouse         \n"
        << "                    \\/___/                                    \n"
        << reset;  // Reset color to default
}

int main() {

    PrintIntro();  // Call the function to print the intro


    // Prompt user for process name
    std::wcout << L"Enter the target process name (e.g., TargetGame.exe): ";
    std::wstring processName;
    std::getline(std::wcin, processName);

    // Prompt user to select the DLL
    std::cout << "Select the DLL you wish to inject." << std::endl;
    std::string dllPath = OpenFileDialog();

    if (dllPath.empty()) {
        std::cerr << "No DLL selected." << std::endl;
        return 1;
    }

    DWORD processID = GetProcessID(processName.c_str());

    if (processID == 0) {
        std::cerr << "Failed to find process ID, check your shit dude." << std::endl;
        std::cin.get();
        return 1;
    }

    if (InjectDLL(processID, dllPath.c_str())) {
        std::cout << "DLL injected successfully!" << std::endl;
        std::cin.get();
    }
    else {
        std::cerr << "DLL injection failed." << std::endl;
        std::cin.get();
    }

    // Wait for user input before closing the terminal
    std::cout << "Press Enter to exit...";
    std::cin.get();  // Wait for user to press Enter

    return 0;
}
