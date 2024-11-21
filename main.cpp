#include <iostream>
#include <windows.h>

#define SHARED_MEMORY_NAME L"SharedMemory"
#define SEMAPHORE_NAME L"Semaphore"


int main() {
    HANDLE hMapFile;
    HANDLE hSemaphore;
    int* pData;
    int num;

    
    hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(int), SHARED_MEMORY_NAME);
    if (hMapFile == NULL) {
        std::cout << "Failed to create file mapping." << std::endl;
        return 1;
    }

    pData = (int*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(int));
    if (pData == NULL) {
        std::cout << "Failed to map view of file." << std::endl;
        CloseHandle(hMapFile);
        return 1;
    }

    hSemaphore = CreateSemaphore(NULL, 1, 1, SEMAPHORE_NAME);
    if (hSemaphore == NULL) {
        std::cout << "Failed to create semaphore." << std::endl;
        UnmapViewOfFile(pData);
        CloseHandle(hMapFile);
        return 1;
    }

    *pData = 1;  

    while (*pData <= 1000) {
        WaitForSingleObject(hSemaphore, INFINITE);
        num = *pData;
        std::cout << "Process 1: Reading number " << num << std::endl;

        if (rand() % 2 == 1) {
            *pData = num + 1;
            std::cout << "Process 1: Writing number " << *pData << std::endl;
        }
        else {
            std::cout << "Process 1: Skipping number" << std::endl;
        }

        ReleaseSemaphore(hSemaphore, 1, NULL);
        Sleep(100);  
    }

    UnmapViewOfFile(pData);
    CloseHandle(hMapFile);
    CloseHandle(hSemaphore);

    return 0;
}
