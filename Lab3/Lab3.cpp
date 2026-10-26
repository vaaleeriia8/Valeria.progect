#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h>
#include <vector>
#include <cstdlib>

int* arr = nullptr;
int n = 0;
int numMarkers = 0;
std::vector<HANDLE> hMarkers;

HANDLE hStartEvent;                
std::vector<HANDLE> hReadyEvents;    
std::vector<HANDLE> hContEvents;     
std::vector<HANDLE> hDeadEvents;     

CRITICAL_SECTION csConsole;

DWORD WINAPI MarkerThread(LPVOID lpParam) {
    int id = (int)lpParam;
    srand(id);

    WaitForSingleObject(hStartEvent, INFINITE);

    int markedCount = 0;

    while (true) {
        int index = rand() % n;

        if (arr[index] == 0) {
            Sleep(5);

            EnterCriticalSection(&csConsole);
            if (arr[index] == 0) { 
                arr[index] = id;
                markedCount++;
            }
            LeaveCriticalSection(&csConsole);

            Sleep(5);
        }
        else {

            EnterCriticalSection(&csConsole);
            std::cout << "Поток " << id
                << ": помечено = " << markedCount
                << ", невозможно пометить индекс = " << index
                << " (значение: " << arr[index] << ")" << std::endl;
            LeaveCriticalSection(&csConsole);

            SetEvent(hReadyEvents[id - 1]);

            HANDLE events[2] = { hContEvents[id - 1], hDeadEvents[id - 1] };
            DWORD waitResult = WaitForMultipleObjects(2, events, FALSE, INFINITE);

            if (waitResult == WAIT_OBJECT_0 + 1) {
                for (int i = 0; i < n; i++) {
                    if (arr[i] == id) arr[i] = 0;
                }
                return 0;
            }
        }
    }
    return 0;
}

int main() {
    setlocale(LC_ALL, "Russian");

    std::cout << "Введите размер массива: ";
    std::cin >> n;
    arr = new int[n];
    for (int i = 0; i < n; i++) arr[i] = 0;

    std::cout << "Введите количество потоков marker: ";
    std::cin >> numMarkers;

    InitializeCriticalSection(&csConsole);

    hStartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    hMarkers.resize(numMarkers);
    hReadyEvents.resize(numMarkers);
    hContEvents.resize(numMarkers);
    hDeadEvents.resize(numMarkers);

    for (int i = 0; i < numMarkers; i++) {
        hReadyEvents[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
        hContEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
        hDeadEvents[i] = CreateEvent(NULL, FALSE, FALSE, NULL);

        hMarkers[i] = CreateThread(NULL, 0, MarkerThread, (LPVOID)(i + 1), 0, NULL);
    }

    std::cout << "Даем сигнал на старт всем потокам..." << std::endl;
    SetEvent(hStartEvent);

    int activeMarkers = numMarkers;

    while (activeMarkers > 0) {
        std::cout << "Ожидание сигналов от потоков..." << std::endl;

        std::vector<HANDLE> activeReadyEvents;
        for (int i = 0; i < numMarkers; i++) {
            if (hMarkers[i] != NULL) { 
                activeReadyEvents.push_back(hReadyEvents[i]);
            }
        }

        WaitForMultipleObjects(activeReadyEvents.size(), activeReadyEvents.data(), TRUE, INFINITE);

        EnterCriticalSection(&csConsole);
        std::cout << "Массив: ";
        for (int i = 0; i < n; i++) std::cout << arr[i] << " ";
        std::cout << std::endl;
        LeaveCriticalSection(&csConsole);

        int killId = 0;
        bool validInput = false;

        while (!validInput) {
            std::cout << "Введите номер потока для завершения (1.." << numMarkers << "): ";
            std::cin >> killId;

            if (killId < 1 || killId > numMarkers) {
                std::cout << "Ошибка: неверный номер. Попробуйте снова.\n";
            }
            else if (hMarkers[killId - 1] == NULL) {
                std::cout << "Ошибка: поток " << killId << " уже завершен. Выберите другой.\n";
            }
            else {
                validInput = true;
            }
        }

        SetEvent(hDeadEvents[killId - 1]);

        WaitForSingleObject(hMarkers[killId - 1], INFINITE);
        CloseHandle(hMarkers[killId - 1]);
        hMarkers[killId - 1] = NULL;
        activeMarkers--;

        EnterCriticalSection(&csConsole);
        std::cout << "Массив после удаления потока " << killId << ": ";
        for (int i = 0; i < n; i++) std::cout << arr[i] << " ";
        std::cout << std::endl;
        LeaveCriticalSection(&csConsole);

        for (int i = 0; i < numMarkers; i++) {
            if (hMarkers[i] != NULL) {
                ResetEvent(hReadyEvents[i]);
                SetEvent(hContEvents[i]);
            }
        }
    }

    std::cout << "Все потоки завершены. Работа окончена." << std::endl;

    for (int i = 0; i < numMarkers; i++) {
        CloseHandle(hReadyEvents[i]);
        CloseHandle(hContEvents[i]);
        CloseHandle(hDeadEvents[i]);
    }
    CloseHandle(hStartEvent);
    DeleteCriticalSection(&csConsole);
    delete[] arr;

    std::cin.get(); std::cin.get();
    return 0;
}