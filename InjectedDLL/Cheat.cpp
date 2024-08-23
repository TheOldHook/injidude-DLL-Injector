#include "Cheat.h"
#include "Offsets.h"
#include <iostream>

DWORD_PTR GetBaseAddress() {
    return (DWORD_PTR)GetModuleHandle(NULL);
}

void ReadGameMemory(DWORD_PTR baseAddress, DWORD_PTR offset) {
    DWORD_PTR targetAddress = baseAddress + offset;
    int value = 0;

    if (ReadProcessMemory(GetCurrentProcess(), (void*)targetAddress, &value, sizeof(value), NULL)) {
        std::cout << "Value at address " << std::hex << targetAddress << ": " << value << std::endl;
    }
    else {
        std::cerr << "Failed to read memory." << std::endl;
    }
}

void WriteGameMemory(DWORD_PTR baseAddress, DWORD_PTR offset, int newValue) {
    DWORD_PTR targetAddress = baseAddress + offset;

    if (WriteProcessMemory(GetCurrentProcess(), (void*)targetAddress, &newValue, sizeof(newValue), NULL)) {
        std::cout << "Successfully wrote " << newValue << " to address " << std::hex << targetAddress << std::endl;
    }
    else {
        std::cerr << "Failed to write memory." << std::endl;
    }
}

DWORD WINAPI MainThread(LPVOID lpParam) {
    DWORD_PTR baseAddress = GetBaseAddress();

    std::cout << "Base address of the module: " << std::hex << baseAddress << std::endl;

    ReadGameMemory(baseAddress, OFFSET_HEALTH);
    WriteGameMemory(baseAddress, OFFSET_HEALTH, 999);
    ReadGameMemory(baseAddress, OFFSET_HEALTH);

    ReadGameMemory(baseAddress, OFFSET_AMMO);
    WriteGameMemory(baseAddress, OFFSET_AMMO, 999);
    ReadGameMemory(baseAddress, OFFSET_AMMO);

    return 0;
}
