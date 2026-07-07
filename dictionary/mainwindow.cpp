#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QApplication>
#include <QDir>
#include <QListWidgetItem>
#include <QStringConverter>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Проверка правописания");

    // Скрываем список предложений при запуске
    ui->suggestionsList->setVisible(false);
    ui->suggestionsList->setMaximumHeight(150);

    // ПОДКЛЮЧАЕМ СИГНАЛ К СЛОТУ ВРУЧНУЮ
    connect(ui->suggestionsList, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(on_suggestionsList_itemClicked(QListWidgetItem*)));

    // Ищем файл словаря
    QStringList paths;
    paths << QApplication::applicationDirPath();
    paths << QDir::currentPath();
    paths << ".";

    QString dictPath;
    for (const QString &path : paths) {
        QString test = path + "/dictionary.txt";
        if (QFile::exists(test)) {
            dictPath = test;
            break;
        }
    }

    if (!dictPath.isEmpty() && loadDictionary(dictPath)) {
        ui->resultLabel->setText(QString("Словарь загружен: %1 слов").arg(dictionary.size()));
        ui->resultLabel->setStyleSheet("color: green; font-weight: bold;");
    } else {
        ui->resultLabel->setText("ОШИБКА: файл dictionary.txt не найден!");
        ui->resultLabel->setStyleSheet("color: red; font-weight: bold;");
        QMessageBox::warning(this, "Ошибка",
                             "Не найден файл словаря dictionary.txt!\n"
                             "Поместите его в папку с программой.");
    }

    ui->inputLineEdit->setFocus();
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

    dictionary.clear();

    QByteArray data = file.readAll();
    file.close();

    QString text = QString::fromUtf8(data);
    QStringList lines = text.split('\n', Qt::SkipEmptyParts);

    for (const QString &line : lines) {
        QString trimmed = line.trimmed();
        if (!trimmed.isEmpty()) {
            dictionary.append(trimmed);
        }
    }

    return !dictionary.isEmpty();
}

bool MainWindow::binarySearch(const QString &word, int &index) const
{
    int left = 0;
    int right = dictionary.size() - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        int compare = word.compare(dictionary[mid], Qt::CaseInsensitive);

        if (compare == 0) {
            index = mid;
            return true;
        } else if (compare < 0) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    index = -1;
    return false;
}

int MainWindow::levenshteinDistance(const QString &s1, const QString &s2) const
{
    int len1 = s1.length();
    int len2 = s2.length();

    if (len1 == 0) return len2;
    if (len2 == 0) return len1;

    QVector<int> prev(len2 + 1);
    QVector<int> curr(len2 + 1);

    for (int j = 0; j <= len2; ++j) {
        prev[j] = j;
    }

    for (int i = 1; i <= len1; ++i) {
        curr[0] = i;
        for (int j = 1; j <= len2; ++j) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            curr[j] = qMin(qMin(curr[j - 1] + 1, prev[j] + 1), prev[j - 1] + cost);
        }
        prev = curr;
    }

    return prev[len2];
}

QVector<QString> MainWindow::findClosestWords(const QString &word, int maxCount) const
{
    QVector<QString> results;
    if (dictionary.isEmpty()) {
        return results;
    }

    // Вектор пар (расстояние, слово)
    QVector<QPair<int, QString>> candidates;

    for (const QString &dictWord : dictionary) {
        int distance = levenshteinDistance(word, dictWord);
        candidates.append(qMakePair(distance, dictWord));
    }

    // Сортируем по расстоянию
    std::sort(candidates.begin(), candidates.end(),
              [](const QPair<int, QString> &a, const QPair<int, QString> &b) {
                  return a.first < b.first;
              });

    // Берём первые maxCount слов с наименьшим расстоянием
    // Пропускаем точные совпадения (расстояние 0)
    int added = 0;
    for (const auto &candidate : candidates) {
        if (candidate.first == 0) continue;
        if (added >= maxCount) break;

        // Порог: если расстояние слишком большое, не предлагаем
        int maxDistance = qMax(2, word.length() / 3);
        if (candidate.first > maxDistance && candidate.first > 3) {
            break;
        }

        results.append(candidate.second);
        added++;
    }

    return results;
}

void MainWindow::showSuggestions(const QVector<QString> &suggestions)
{
    ui->suggestionsList->clear();

    if (suggestions.isEmpty()) {
        hideSuggestions();
        return;
    }

    for (const QString &word : suggestions) {
        ui->suggestionsList->addItem(word);
    }

    ui->suggestionsList->setVisible(true);
}

void MainWindow::hideSuggestions()
{
    ui->suggestionsList->clear();
    ui->suggestionsList->setVisible(false);
}

void MainWindow::on_checkButton_clicked()
{
    QString inputWord = ui->inputLineEdit->text().trimmed();

    // Скрываем старые предложения
    hideSuggestions();

    if (inputWord.isEmpty()) {
        ui->resultLabel->setText("⚠ Введите слово для проверки");
        ui->resultLabel->setStyleSheet("color: orange; font-weight: bold;");
        return;
    }

    if (dictionary.isEmpty()) {
        ui->resultLabel->setText("ОШИБКА: словарь не загружен!");
        ui->resultLabel->setStyleSheet("color: red; font-weight: bold;");
        return;
    }

    lastInputWord = inputWord;
    QString wordLower = inputWord.toLower();

    int index;
    bool found = binarySearch(wordLower, index);

    if (found) {
        ui->resultLabel->setText(QString("✓ Слово «%1» найдено в словаре (позиция %2)")
                                     .arg(inputWord)
                                     .arg(index + 1));
        ui->resultLabel->setStyleSheet("color: green; font-weight: bold;");
        hideSuggestions();
    } else {
        // Ищем несколько близких слов
        QVector<QString> closestWords = findClosestWords(wordLower, 5);

        if (!closestWords.isEmpty()) {
            ui->resultLabel->setText(QString("✗ Слово «%1» не найдено.\n"
                                             "Выберите правильный вариант из списка:")
                                         .arg(inputWord));
            ui->resultLabel->setStyleSheet("color: #E67E22; font-weight: bold;");

            // Показываем список предложений
            showSuggestions(closestWords);
        } else {
            ui->resultLabel->setText(QString("✗ Слово «%1» не найдено.\n"
                                             "Близких совпадений не обнаружено.")
                                         .arg(inputWord));
            ui->resultLabel->setStyleSheet("color: red; font-weight: bold;");
            hideSuggestions();
        }
    }
}

void MainWindow::on_inputLineEdit_returnPressed()
{
    on_checkButton_clicked();
}

void MainWindow::on_suggestionsList_itemClicked(QListWidgetItem *item)
{
    if (!item) return;

    QString selectedWord = item->text();

    // Вставляем выбранное слово в поле ввода
    ui->inputLineEdit->setText(selectedWord);

    // Показываем результат
    ui->resultLabel->setText(QString("✓ Вы выбрали: «%1»\nСлово найдено в словаре!")
                                 .arg(selectedWord));
    ui->resultLabel->setStyleSheet("color: green; font-weight: bold;");

    // Скрываем список
    hideSuggestions();

    // Устанавливаем фокус на поле ввода
    ui->inputLineEdit->setFocus();
    ui->inputLineEdit->selectAll();
}