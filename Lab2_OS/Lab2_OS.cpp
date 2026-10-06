#define _CRT_SECURE_NO_WARNINGS
#include <iostream>    
#include <windows.h>   
#include <climits>     

int* arr = nullptr;     
int n = 0;               
int minVal = INT_MAX;    
int maxVal = INT_MIN;    
double averageVal = 0;   

DWORD WINAPI MinMaxThread(LPVOID lpParam) {

    for (int i = 0; i < n; i++) {
        if (arr[i] < minVal) {
            minVal = arr[i];
        }
        if (arr[i] > maxVal) {
            maxVal = arr[i];
        }
        Sleep(7);
    }

    std::cout << "Минимум: " << minVal << ", Максимум: " << maxVal << std::endl;
    return 0; 
}

DWORD WINAPI AverageThread(LPVOID lpParam) {

    long long sum = 0; 
    for (int i = 0; i < n; i++) {
        sum += arr[i];
        Sleep(12);
    }

    averageVal = (double)sum / n; 
    std::cout << "Среднее арифметическое: " << averageVal << std::endl;
    return 0;
}

int main() {
    setlocale(LC_ALL, "Russian");

    std::cout << "Введите размер массива: ";
    std::cin >> n;

    arr = new int[n]; 

    std::cout << "Введите элементы массива:" << std::endl;
    for (int i = 0; i < n; i++) {
        std::cin >> arr[i];
    }

    std::cout << "\nИсходный массив: ";
    for (int i = 0; i < n; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;

    HANDLE hMinMax = CreateThread(
        NULL,           
        0,              
        MinMaxThread,   
        NULL,           
        0,              
        NULL           
    );

    HANDLE hAverage = CreateThread(
        NULL,
        0,
        AverageThread,
        NULL,
        0,
        NULL
    );

    if (hMinMax == NULL || hAverage == NULL) {
        std::cout << "Ошибка создания потока! Код: " << GetLastError() << std::endl;
        return 1;
    }

    WaitForSingleObject(hMinMax, INFINITE);
    WaitForSingleObject(hAverage, INFINITE);

    for (int i = 0; i < n; i++) {
        if (arr[i] == minVal || arr[i] == maxVal) {
            arr[i] = (int)averageVal; 
        }
    }

    std::cout << "\nИзмененный массив: ";
    for (int i = 0; i < n; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;

    CloseHandle(hMinMax);
    CloseHandle(hAverage);
    delete[] arr;

    std::cout << "\nРабота завершена. Нажмите Enter для выхода." << std::endl;
    std::cin.get(); std::cin.get();
    return 0;
}