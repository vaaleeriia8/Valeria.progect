#include <iostream>
#include <fstream>
#include <string>
#include <iomanip> 
#include <windows.h>

using namespace std;

struct employee {
    int num;
    char name[10];
    double hours;
};

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Ru");

    if (argc < 4) {
        cout << "Ошибка: недостаточно аргументов." << endl;
        return 1;
    }

    string binFilename = argv[1];
    string reportFilename = argv[2];
    double payPerHour = atof(argv[3]); 

    ifstream inFile(binFilename, ios::binary);
    if (!inFile) {
        cout << "Не удалось открыть бинарный файл." << endl;
        return 1;
    }

    ofstream reportFile(reportFilename);
    if (!reportFile) {
        cout << "Не удалось создать файл отчета." << endl;
        return 1;
    }

    reportFile << "Отчет по файлу \"" << binFilename << "\"" << endl;
    reportFile << "------------------------------------------" << endl;
    reportFile << "Номер | Имя       | Часы | Зарплата" << endl;
    reportFile << "------------------------------------------" << endl;

    employee emp;
    while (inFile.read((char*)&emp, sizeof(employee))) {
        double salary = emp.hours * payPerHour;

        reportFile << setw(5) << emp.num << " | "
            << setw(9) << emp.name << " | "
            << setw(4) << emp.hours << " | "
            << salary << endl;
    }

    cout << "Отчет создан: " << reportFilename << endl;

    inFile.close();
    reportFile.close();
    return 0;
}