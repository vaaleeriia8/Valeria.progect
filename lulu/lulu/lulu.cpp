#include <iostream>
#include <cstring>
#include <cctype>
#include <Windows.h>

using namespace std;

const int MAX_TEXT = 10000;

char toLowerCP1251(char ch) {
    unsigned char c = static_cast<unsigned char>(ch);

    if (c >= 'A' && c <= 'Z') {
        return c + 32;
    }

    //А-Я
    if (c >= 192 && c <= 223) {
        return c + 32;
    }

    //Ё
    if (c == 168) {
        return 184;
    }

    return ch;
}

bool isPunctuation(char ch) {
    unsigned char c = static_cast<unsigned char>(ch);
    return (c == '.' || c == ',' || c == '!' || c == '?' ||
        c == ';' || c == ':' || c == '-' || c == '(' || c == ')' ||
        c == '"' || c == '\'' || c == '…' || c == '—');
}

bool isSpace(char ch) {
    return (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');
}

void processText(const char* input, char* output) {
    int outPos = 0;
    bool lastWasSpace = false;
    bool lastWasPunct = false;
    int len = strlen(input);

    for (int i = 0; i < len; i++) {
        char ch = input[i];
        unsigned char uc = static_cast<unsigned char>(ch);

        if (ch == '\n' || ch == '\r' || ch == '\t') {
            continue;
        }

        if ((uc >= 'A' && uc <= 'Z') ||      
            (uc >= 192 && uc <= 223) ||      
            uc == 168) {
            ch = toLowerCP1251(ch);
            uc = static_cast<unsigned char>(ch);
        }

        if (isSpace(ch)) {
            if (!lastWasSpace && !lastWasPunct) {
                output[outPos++] = ' ';
                lastWasSpace = true;
            }
            continue;
        }

        if (isPunctuation(ch)) {
            if (lastWasSpace && outPos > 0 && output[outPos - 1] == ' ') {
                outPos--;
                lastWasSpace = false;
            }

            output[outPos++] = ch;
            lastWasPunct = true;
            lastWasSpace = false;
            continue;
        }

        if (lastWasPunct) {
            output[outPos++] = ' ';
            lastWasPunct = false;
            lastWasSpace = true;
        }

        output[outPos++] = ch;
        lastWasSpace = false;
    }

    while (outPos > 0 && output[outPos - 1] == ' ') {
        outPos--;
    }

    int cleanPos = 0;
    bool spaceAdded = false;

    for (int i = 0; i < outPos; i++) {
        if (isSpace(output[i])) {
            if (!spaceAdded) {
                output[cleanPos++] = ' ';
                spaceAdded = true;
            }
        }
        else {
            output[cleanPos++] = output[i];
            spaceAdded = false;
        }
    }

    output[cleanPos] = '\0';
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    cout << "Введите текст:" << endl;

    char input[MAX_TEXT];
    char output[MAX_TEXT];

    while (true) {
        cin.getline(input, MAX_TEXT);

        if (strlen(input) == 0) {
            break;
        }

        processText(input, output);

        cout << "Результат: " << endl << output << endl;
    }

    system("pause");
    return 0;
}

