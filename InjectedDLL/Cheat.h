// Cheat.h
#pragma once
#include <windows.h>

// Function declarations
DWORD_PTR GetBaseAddress();
void ReadGameMemory(DWORD_PTR baseAddress, DWORD_PTR offset);
void WriteGameMemory(DWORD_PTR baseAddress, DWORD_PTR offset, int newValue);
DWORD WINAPI MainThread(LPVOID lpParam);
