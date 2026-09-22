#include <iostream>
#include <fstream> 
#include <string>
#include <windows.h> 

using namespace std;

struct employee {
    int num;
    char name[10];
    double hours;
};

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Ru");

    if (argc < 3) {
        cout << "Ошибка: недостаточно аргументов. Используйте: Creator.exe <file> <count>" << endl;
        return 1;
    }

    string filename = argv[1];
    int count = atoi(argv[2]); 

    ofstream outFile(filename, ios::binary);
    if (!outFile) {
        cout << "Не удалось создать файл." << endl;
        return 1;
    }

    cout << "Создание файла " << filename << " с " << count << " записями" << endl;

    for (int i = 0; i < count; i++) {
        employee emp;
        cout << "\nЗапись №" << i + 1 << endl;
        cout << "Введите номер сотрудника: ";
        cin >> emp.num;
        cout << "Введите имя сотрудника: ";
        cin >> emp.name; 
        cout << "Введите отработанные часы: ";
        cin >> emp.hours;

        outFile.write((char*)&emp, sizeof(employee));
    }

    outFile.close();
    cout << "Файл успешно создан." << endl;
    return 0;
}