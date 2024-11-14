#include <iostream>
#include <windows.h>
#include <cstdlib>
#include <ctime>

using namespace std;

const int MAX_COUNT = 1000;

int main() {

    srand(static_cast<unsigned int>(time(0)));

    HANDLE hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        sizeof(int),
        L"Global\\SharedMemory"
    );

    if (hMapFile == NULL) {
        cerr << "Nu s-a putut crea obiectul de memorie partajată. Cod de eroare: " << GetLastError() << endl;
        return 1;
    }

    int* sharedMemory = static_cast<int*>(MapViewOfFile(
        hMapFile,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        sizeof(int)
    ));

    if (sharedMemory == NULL) {
        cerr << "Nu s-a putut mappa obiectul de memorie. Cod de eroare: " << GetLastError() << endl;
        CloseHandle(hMapFile);
        return 1;
    }

    *sharedMemory = 0;

    HANDLE hSemaphore = CreateSemaphore(
        NULL,
        1,
        1,
        L"Global\\MemorySemaphore"
    );

    if (hSemaphore == NULL) {
        cerr << "Nu s-a putut crea semaforul. Cod de eroare: " << GetLastError() << endl;
        UnmapViewOfFile(sharedMemory);
        CloseHandle(hMapFile);
        return 1;
    }

    while (*sharedMemory < MAX_COUNT) {
        WaitForSingleObject(hSemaphore, INFINITE);

        int currentValue = *sharedMemory;

       
        int coinFlip = rand() % 2;
        if (coinFlip == 0) {
            *sharedMemory = currentValue + 1;
            cout << "Procesul " << GetCurrentProcessId() << " a incrementat la valoarea: " << *sharedMemory << " (banul a cazut pe 0)" << endl;
        }
        else {
            cout << "Procesul " << GetCurrentProcessId() << " nu a incrementat (banul a cazut pe 1)" << endl;
        }
       
        ReleaseSemaphore(hSemaphore, 1, NULL);

        Sleep(50);
    }

    UnmapViewOfFile(sharedMemory);
    CloseHandle(hMapFile);
    CloseHandle(hSemaphore);

    cout << "Procesul " << GetCurrentProcessId() << " a terminat numaratoarea." << endl;
    system("pause");
    return 0;
}
