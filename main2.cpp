#include <iostream>
#include <windows.h>

#define SHARED_MEMORY_NAME L"SharedMemory"
#define SEMAPHORE_NAME L"Semaphore"


void process2_main() {
    HANDLE hMapFile;
    HANDLE hSemaphore;
    int* pData;
    int num;

    hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHARED_MEMORY_NAME);
    if (hMapFile == NULL) {
        std::cout << "Failed to open file mapping." << std::endl;
        return;
    }

    pData = (int*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(int));
    if (pData == NULL) {
        std::cout << "Failed to map view of file." << std::endl;
        CloseHandle(hMapFile);
        return;
    }

    hSemaphore = OpenSemaphore(SYNCHRONIZE | SEMAPHORE_MODIFY_STATE, FALSE, SEMAPHORE_NAME);
    if (hSemaphore == NULL) {
        std::cout << "Failed to open semaphore." << std::endl;
        UnmapViewOfFile(pData);
        CloseHandle(hMapFile);
        return;
    }

    while (*pData <= 1000) {
        WaitForSingleObject(hSemaphore, INFINITE);
        num = *pData;
        std::cout << "Process 2: Reading number " << num << std::endl;

        if (rand() % 2 == 1) {
            *pData = num + 1;
            std::cout << "Process 2: Writing number " << *pData << std::endl;
        }
        else {
            std::cout << "Process 2: Skipping number" << std::endl;
        }

        ReleaseSemaphore(hSemaphore, 1, NULL);
        Sleep(100);  
    }

    UnmapViewOfFile(pData);
    CloseHandle(hMapFile);
    CloseHandle(hSemaphore);
}
