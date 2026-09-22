#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <windows.h> 
#include <string>

using namespace std;

int main() {
    setlocale(LC_ALL, "Ru"); 

    string binFileName;
    int recordCount;

    cout << "Введите имя бинарного файла (например, data.bin): ";
    cin >> binFileName;
    cout << "Введите количество записей: ";
    cin >> recordCount;

    string creatorPath = "Creator.exe";

    string cmdLineCreator = creatorPath + " " + binFileName + " " + to_string(recordCount);

    STARTUPINFOA si;    
    PROCESS_INFORMATION pi; 

    ZeroMemory(&si, sizeof(si)); 
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    char* cmdLineChar = new char[cmdLineCreator.length() + 1];
    strcpy_s(cmdLineChar, cmdLineCreator.length() + 1, cmdLineCreator.c_str());

    cout << "Запуск Creator..." << endl;

    if (!CreateProcessA(
        NULL,          
        cmdLineChar,   
        NULL,           
        NULL,           
        FALSE,          
        0,             
        NULL,          
        NULL,          
        &si,           
        &pi             
    )) {
        cout << "Ошибка запуска Creator! Код ошибки: " << GetLastError() << endl;
        return 1;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    cout << "Creator завершил работу. Файл создан." << endl;
 
    cout << "\nСодержимое бинарного файла" << endl;
    FILE* binFile;
    fopen_s(&binFile, binFileName.c_str(), "rb");

    if (binFile) {
        struct employee {
            int num;
            char name[10];
            double hours;
        } emp;

        while (fread(&emp, sizeof(employee), 1, binFile)) {
            cout << "ID: " << emp.num << " | Имя: " << emp.name << " | Часы: " << emp.hours << endl;
        }
        fclose(binFile);
    }
    else {
        cout << "Не удалось открыть файл для чтения." << endl;
    }

    string reportFileName;
    double payPerHour;

    cout << "\nВведите имя файла отчета (например, report.txt): ";
    cin >> reportFileName;
    cout << "Введите оплату за час: ";
    cin >> payPerHour;

    string reporterPath = "Reporter.exe";
    string cmdLineReporter = reporterPath + " " + binFileName + " " + reportFileName + " " + to_string(payPerHour);

    char* cmdLineRepChar = new char[cmdLineReporter.length() + 1];
    strcpy(cmdLineRepChar, cmdLineReporter.c_str());

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    cout << "Запуск Reporter..." << endl;

    if (!CreateProcessA(NULL, cmdLineRepChar, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        cout << "Ошибка запуска Reporter! Код ошибки: " << GetLastError() << endl;
        return 1;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    cout << "Reporter завершил работу." << endl;

    cout << "\n Содержимое отчета " << endl;
    FILE* repFile;
    fopen_s(&repFile, reportFileName.c_str(), "r");

    if (repFile) {
        char line[256];
        while (fgets(line, sizeof(line), repFile)) {
            cout << line;
        }
        fclose(repFile);
    }
    else {
        cout << "Не удалось открыть файл отчета." << endl;
    }

    delete[] cmdLineChar; 
    delete[] cmdLineRepChar;

    cout << "\nРабота завершена. Нажмите Enter для выхода." << endl;
    cin.get(); cin.get(); 
    return 0;
}