#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QApplication>
#include <QDir>
#include <QListWidgetItem>
#include <QRegularExpression>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Преобразователь дат");

    // Скрываем всплывающий список
    ui->popupList->setVisible(false);
    ui->popupList->setMaximumHeight(150);

    // Подключаем сигнал для списка
    connect(ui->popupList, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(on_popupList_itemClicked(QListWidgetItem*)));

    // Очищаем поля
    ui->digitalDateLabel->setText("");
    ui->russianDateLabel->setText("");
    ui->russianDigitalDateLabel->setText("");

    // Загружаем словарь
    QStringList paths;
    paths << QApplication::applicationDirPath();
    paths << QDir::currentPath();
    paths << ".";

    QString dictPath;
    for (const QString &path : paths) {
        QString test = path + "/dates.txt";
        if (QFile::exists(test)) {
            dictPath = test;
            break;
        }
    }

    if (!dictPath.isEmpty() && loadDictionary(dictPath)) {
        ui->titleLabel->setText(QString("Словарь загружен: месяцы %1, дни %2, числа %3")
                                    .arg(monthEnToRu.size())
                                    .arg(dayEnToRu.size())
                                    .arg(numberEnToRu.size()));
        ui->titleLabel->setStyleSheet("color: green; font-weight: bold;");
    } else {
        ui->titleLabel->setText("ОШИБКА: файл dates.txt не найден!");
        ui->titleLabel->setStyleSheet("color: red; font-weight: bold;");
        QMessageBox::warning(this, "Ошибка",
                             "Не найден файл словаря dates.txt!\n"
                             "Поместите его в папку с программой.");
    }

    ui->inputLineEdit->setFocus();
    ui->inputLineEdit->setPlaceholderText("e.g. six september one");
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::loadDictionary(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    monthEnToRu.clear();
    monthEnToNum.clear();
    dayEnToRu.clear();
    dayEnToNum.clear();
    numberEnToRu.clear();
    numberEnToNum.clear();

    QByteArray data = file.readAll();
    file.close();

    QString text = QString::fromUtf8(data);
    QStringList lines = text.split('\n', Qt::SkipEmptyParts);

    for (const QString &line : lines) {
        QString trimmed = line.trimmed();
        if (trimmed.isEmpty() || trimmed.startsWith("#")) continue;

        QStringList parts = trimmed.split(' ');
        if (parts.size() >= 3) {
            QString english = parts[0].toLower();
            QString russian = parts[1];
            int number = parts[2].toInt();

            // Определяем тип: месяц, день недели или число
            QStringList months = {"january", "february", "march", "april", "may", "june",
                                  "july", "august", "september", "october", "november", "december",
                                  "january_gen", "february_gen", "march_gen", "april_gen", "may_gen",
                                  "june_gen", "july_gen", "august_gen", "september_gen", "october_gen",
                                  "november_gen", "december_gen",
                                  "jan", "feb", "mar", "apr", "jun", "jul", "aug", "sep", "oct", "nov", "dec"};

            QStringList days = {"monday", "tuesday", "wednesday", "thursday", "friday",
                                "saturday", "sunday", "mon", "tue", "wed", "thu", "fri", "sat", "sun"};

            if (months.contains(english)) {
                monthEnToRu[english] = russian;
                monthEnToNum[english] = number;
            } else if (days.contains(english)) {
                dayEnToRu[english] = russian;
                dayEnToNum[english] = number;
            } else {
                numberEnToRu[english] = russian;
                numberEnToNum[english] = number;
            }
        }
    }

    return !monthEnToRu.isEmpty() && !dayEnToRu.isEmpty() && !numberEnToRu.isEmpty();
}

int MainWindow::wordToNumber(const QString &word) const
{
    return numberEnToNum.value(word.toLower(), -1);
}

QString MainWindow::numberToRussian(int num, bool isDay, bool isYear) const
{
    // Для дня используем средний род (порядковые)
    if (isDay) {
        // Специальные слова для дня (средний род)
        QMap<int, QString> dayNumbers;
        dayNumbers[1] = "первое";
        dayNumbers[2] = "второе";
        dayNumbers[3] = "третье";
        dayNumbers[4] = "четвертое";
        dayNumbers[5] = "пятое";
        dayNumbers[6] = "шестое";
        dayNumbers[7] = "седьмое";
        dayNumbers[8] = "восьмое";
        dayNumbers[9] = "девятое";
        dayNumbers[10] = "десятое";
        dayNumbers[11] = "одиннадцатое";
        dayNumbers[12] = "двенадцатое";
        dayNumbers[13] = "тринадцатое";
        dayNumbers[14] = "четырнадцатое";
        dayNumbers[15] = "пятнадцатое";
        dayNumbers[16] = "шестнадцатое";
        dayNumbers[17] = "семнадцатое";
        dayNumbers[18] = "восемнадцатое";
        dayNumbers[19] = "девятнадцатое";
        dayNumbers[20] = "двадцатое";
        dayNumbers[21] = "двадцать первое";
        dayNumbers[22] = "двадцать второе";
        dayNumbers[23] = "двадцать третье";
        dayNumbers[24] = "двадцать четвертое";
        dayNumbers[25] = "двадцать пятое";
        dayNumbers[26] = "двадцать шестое";
        dayNumbers[27] = "двадцать седьмое";
        dayNumbers[28] = "двадцать восьмое";
        dayNumbers[29] = "двадцать девятое";
        dayNumbers[30] = "тридцатое";
        dayNumbers[31] = "тридцать первое";

        if (dayNumbers.contains(num)) {
            return dayNumbers[num];
        }
        return QString::number(num) + "-е";
    }

    // Для года используем количественные числительные
    if (isYear) {
        for (auto it = numberEnToRu.begin(); it != numberEnToRu.end(); ++it) {
            if (numberEnToNum[it.key()] == num) {
                return it.value();
            }
        }
        return QString::number(num);
    }

    return QString::number(num);
}

int MainWindow::parseYear(const QStringList &words, int &pos) const
{
    int year = 0;
    int currentPos = pos;

    // Проверяем "two thousand" -> 2000
    if (currentPos + 1 < words.size() &&
        words[currentPos].toLower() == "two" &&
        words[currentPos + 1].toLower() == "thousand") {

        year = 2000;
        currentPos += 2;

        // Читаем остаток года
        while (currentPos < words.size()) {
            int num = wordToNumber(words[currentPos]);
            if (num > 0 && num < 100) {
                year += num;
                currentPos++;
            } else {
                break;
            }
        }
        pos = currentPos;
        return year;
    }

    // Обычный год: "twenty twenty six" -> 2026
    while (currentPos < words.size()) {
        int num = wordToNumber(words[currentPos]);
        if (num > 0 && num < 100) {
            year = year * 100 + num;
            currentPos++;
        } else {
            break;
        }
    }
    pos = currentPos;
    return year;
}

QString MainWindow::parseDate(const QString &text,
                              QString &digitalDate,
                              QString &russianDate,
                              QString &russianDigitalDate)
{
    if (text.isEmpty()) {
        digitalDate = "";
        russianDate = "";
        russianDigitalDate = "";
        return "";
    }

    QStringList words = text.toLower().split(QRegularExpression("\\s+"));

    int day = -1;
    int month = -1;
    int year = -1;
    QString dayRussian;
    QString monthRussian;
    int pos = 0;

    // Ищем день
    while (pos < words.size()) {
        QString word = words[pos];

        // Проверяем день недели
        if (dayEnToNum.contains(word)) {
            day = dayEnToNum[word];
            dayRussian = dayEnToRu[word];
            pos++;
            break;
        }
        // Проверяем число (1-31)
        int num = wordToNumber(word);
        if (num > 0 && num <= 31) {
            day = num;
            dayRussian = numberToRussian(num, true, false);
            pos++;
            break;
        }
        pos++;
    }

    // Ищем месяц (используем родительный падеж)
    while (pos < words.size()) {
        QString word = words[pos];

        // Проверяем месяц в родительном падеже
        QString genKey = word + "_gen";
        if (monthEnToNum.contains(genKey)) {
            month = monthEnToNum[genKey];
            monthRussian = monthEnToRu[genKey];
            pos++;
            break;
        }
        // Проверяем обычный месяц
        if (monthEnToNum.contains(word)) {
            month = monthEnToNum[word];
            // Для родительного падежа
            QString base = monthEnToRu[word];
            if (base.endsWith("ь")) {
                monthRussian = base.left(base.length() - 1) + "я";
            } else if (base.endsWith("й")) {
                monthRussian = base.left(base.length() - 1) + "я";
            } else if (base.endsWith("т")) {
                monthRussian = base + "а";
            } else if (base.endsWith("т")) {
                monthRussian = base + "а";
            } else {
                monthRussian = base + "а";
            }
            pos++;
            break;
        }
        pos++;
    }

    // Ищем год
    if (pos < words.size()) {
        year = parseYear(words, pos);
    }

    // Формируем результат
    if (day > 0 && month > 0) {
        // Цифровой формат (английский): YYYY-MM-DD
        digitalDate = QString("%1-%2-%3")
                          .arg(year > 0 ? QString::number(year) : "????", 4, '0')
                          .arg(month, 2, 10, QChar('0'))
                          .arg(day, 2, 10, QChar('0'));

        // Русский текстовый формат
        QString yearRussian;
        if (year > 0) {
            yearRussian = numberToRussian(year, false, true);
        } else {
            yearRussian = "????";
        }

        russianDate = QString("%1 %2 %3")
                          .arg(dayRussian)
                          .arg(monthRussian)
                          .arg(yearRussian);

        // Русский цифровой формат: DD.MM.YYYY
        russianDigitalDate = QString("%1.%2.%3")
                                 .arg(day, 2, 10, QChar('0'))
                                 .arg(month, 2, 10, QChar('0'))
                                 .arg(year > 0 ? QString::number(year) : "????");

        return digitalDate;
    }

    digitalDate = "";
    russianDate = "";
    russianDigitalDate = "";
    return "";
}

QVector<QString> MainWindow::findSuggestions(const QString &word) const
{
    QVector<QString> results;
    QString wordLower = word.toLower();

    // Ищем в месяцах
    for (auto it = monthEnToRu.begin(); it != monthEnToRu.end(); ++it) {
        if (it.key().startsWith(wordLower)) {
            results.append(it.key() + " → " + it.value());
        }
    }

    // Ищем в днях недели
    for (auto it = dayEnToRu.begin(); it != dayEnToRu.end(); ++it) {
        if (it.key().startsWith(wordLower)) {
            results.append(it.key() + " → " + it.value());
        }
    }

    // Ищем в числах
    for (auto it = numberEnToRu.begin(); it != numberEnToRu.end(); ++it) {
        if (it.key().startsWith(wordLower)) {
            results.append(it.key() + " → " + it.value());
        }
    }

    // Ограничиваем количество
    if (results.size() > 10) {
        results.resize(10);
    }

    return results;
}

void MainWindow::showPopup(const QVector<QString> &suggestions)
{
    ui->popupList->clear();

    if (suggestions.isEmpty()) {
        hidePopup();
        return;
    }

    // Размещаем список внизу окна
    int x = ui->inputLineEdit->geometry().x();
    int y = this->height() - 170; // 170 = высота списка + отступ
    int width = ui->inputLineEdit->width();

    ui->popupList->setGeometry(x, y, width, 150);

    for (const QString &item : suggestions) {
        ui->popupList->addItem(item);
    }

    ui->popupList->setVisible(true);
    ui->popupList->raise();
}

void MainWindow::hidePopup()
{
    ui->popupList->clear();
    ui->popupList->setVisible(false);
}

void MainWindow::on_inputLineEdit_textChanged(const QString &text)
{
    // Показываем подсказки при вводе
    if (text.length() >= 1) {
        // Находим последнее слово
        QStringList words = text.split(QRegularExpression("\\s+"));
        if (!words.isEmpty()) {
            QString lastWord = words.last();
            if (lastWord.length() >= 1) {
                QVector<QString> suggestions = findSuggestions(lastWord);
                if (!suggestions.isEmpty()) {
                    showPopup(suggestions);
                } else {
                    hidePopup();
                }
            }
        }
    } else {
        hidePopup();
    }

    // Обновляем отображение даты
    updateDateDisplay();
}

void MainWindow::updateDateDisplay()
{
    QString text = ui->inputLineEdit->text();
    QString digitalDate, russianDate, russianDigitalDate;

    parseDate(text, digitalDate, russianDate, russianDigitalDate);

    if (!digitalDate.isEmpty()) {
        ui->digitalDateLabel->setText(digitalDate);
        ui->digitalDateLabel->setStyleSheet("color: blue; font-weight: bold; font-size: 18px;");
        ui->digitalDateLabel->setVisible(true);

        ui->russianDateLabel->setText(russianDate);
        ui->russianDateLabel->setStyleSheet("color: green; font-weight: bold; font-size: 18px;");
        ui->russianDateLabel->setVisible(true);

        ui->russianDigitalDateLabel->setText(russianDigitalDate);
        ui->russianDigitalDateLabel->setStyleSheet("color: red; font-weight: bold; font-size: 18px;");
        ui->russianDigitalDateLabel->setVisible(true);
    } else {
        ui->digitalDateLabel->setText("");
        ui->russianDateLabel->setText("");
        ui->russianDigitalDateLabel->setText("");
    }
}

void MainWindow::on_popupList_itemClicked(QListWidgetItem *item)
{
    if (!item) return;

    QString selected = item->text();
    QString english = selected.split(" → ").first();

    // Заменяем последнее слово в тексте
    QString currentText = ui->inputLineEdit->text();
    QStringList words = currentText.split(QRegularExpression("\\s+"));
    if (!words.isEmpty()) {
        words[words.size() - 1] = english;
        ui->inputLineEdit->setText(words.join(" "));
    }

    hidePopup();
    ui->inputLineEdit->setFocus();
    ui->inputLineEdit->end(false);
}