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

    setWindowTitle("Англо-русский переводчик");

    // Скрываем всплывающий список
    ui->popupList->setVisible(false);
    ui->popupList->setMaximumHeight(150);

    // Подключаем сигнал для списка
    connect(ui->popupList, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(on_popupList_itemClicked(QListWidgetItem*)));

    // Загружаем словарь
    QStringList paths;
    paths << QApplication::applicationDirPath();
    paths << QDir::currentPath();
    paths << ".";

    QString dictPath;
    for (const QString &path : paths) {
        QString test = path + "/en_ru.txt";
        if (QFile::exists(test)) {
            dictPath = test;
            break;
        }
    }

    if (!dictPath.isEmpty() && loadDictionary(dictPath)) {
        ui->statusLabel->setText(QString("Словарь загружен: %1 слов").arg(dictionary.size()));
        ui->statusLabel->setStyleSheet("color: green; font-weight: bold;");
    } else {
        ui->statusLabel->setText("ОШИБКА: файл en_ru.txt не найден!");
        ui->statusLabel->setStyleSheet("color: red; font-weight: bold;");
        QMessageBox::warning(this, "Ошибка",
                             "Не найден файл словаря en_ru.txt!\n"
                             "Поместите его в папку с программой.");
    }

    ui->textEdit->setFocus();
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
        if (trimmed.isEmpty()) continue;

        QStringList parts = trimmed.split(' ');
        if (parts.size() >= 2) {
            QString english = parts[0].toLower();
            QString russian = parts.mid(1).join(' ');
            dictionary[english] = russian;
        }
    }

    return !dictionary.isEmpty();
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

QVector<QString> MainWindow::findSuggestions(const QString &word, int maxCount) const
{
    QVector<QString> results;
    if (dictionary.isEmpty()) return results;

    QVector<QPair<int, QString>> candidates;

    for (auto it = dictionary.begin(); it != dictionary.end(); ++it) {
        QString key = it.key();
        int distance = levenshteinDistance(word, key);
        candidates.append(qMakePair(distance, key));
    }

    std::sort(candidates.begin(), candidates.end(),
              [](const QPair<int, QString> &a, const QPair<int, QString> &b) {
                  return a.first < b.first;
              });

    int added = 0;
    for (const auto &candidate : candidates) {
        if (candidate.first == 0) continue;
        if (added >= maxCount) break;
        if (candidate.first > 3) break;

        QString translation = getTranslation(candidate.second);
        if (!translation.isEmpty()) {
            results.append(candidate.second + " → " + translation);
        }
        added++;
    }

    return results;
}

QString MainWindow::getTranslation(const QString &word) const
{
    return dictionary.value(word.toLower(), "");
}

// ИСПРАВЛЕННАЯ ФУНКЦИЯ - убрали const, так как она изменяет строку
bool MainWindow::isPastTense(const QString &sentence) const
{
    QStringList words = sentence.toLower().split(QRegularExpression("\\s+"));
    for (const QString &word : words) {
        QString clean = word;
        clean.remove(QRegularExpression("[^a-zA-Z]"));  // ← теперь работает
        if (clean.endsWith("ed") && clean.length() > 3) {
            return true;
        }
    }
    return false;
}

// ИСПРАВЛЕННАЯ ФУНКЦИЯ
QString MainWindow::conjugateVerb(const QString &verb, bool pastTense) const
{
    if (!pastTense) {
        return verb;
    }

    // Простое прошедшее время
    if (verb.endsWith("ть")) {
        QString base = verb.left(verb.length() - 2);
        return base + "л";
    } else if (verb.endsWith("ь")) {
        QString base = verb.left(verb.length() - 1);
        return base + "л";
    } else {
        return verb + "л";
    }
}

QString MainWindow::translateSentence(const QString &sentence) const
{
    if (sentence.isEmpty()) {
        return "";
    }

    QStringList words = sentence.split(QRegularExpression("\\s+"));
    QStringList translatedWords;

    bool pastTense = isPastTense(sentence);

    for (const QString &word : words) {
        QString cleanWord = word.toLower();
        cleanWord.remove(QRegularExpression("[^a-zA-Z']"));

        if (cleanWord.isEmpty()) {
            translatedWords.append(word);
            continue;
        }

        QString translation = getTranslation(cleanWord);

        if (translation.isEmpty()) {
            translatedWords.append(QString("[%1]").arg(word));
        } else {
            // Проверяем, не глагол ли это в прошедшем времени
            if (cleanWord.endsWith("ed") && cleanWord.length() > 3) {
                QString baseVerb = cleanWord.left(cleanWord.length() - 2);
                QString ruBase = getTranslation(baseVerb);
                if (!ruBase.isEmpty()) {
                    translation = conjugateVerb(ruBase, true);
                }
            }
            translatedWords.append(translation);
        }
    }

    return translatedWords.join(" ");
}

void MainWindow::showPopup(const QVector<QString> &suggestions)
{
    ui->popupList->clear();

    if (suggestions.isEmpty()) {
        hidePopup();
        return;
    }

    // Определяем позицию для всплывающего списка (под курсором)
    QRect cursorRect = ui->textEdit->cursorRect();
    QPoint globalPos = ui->textEdit->mapToGlobal(cursorRect.bottomLeft());
    QPoint widgetPos = ui->textEdit->mapFromGlobal(globalPos);

    ui->popupList->move(widgetPos.x(), widgetPos.y() + 10);

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

void MainWindow::on_translateButton_clicked()
{
    QString text = ui->textEdit->toPlainText().trimmed();

    if (text.isEmpty()) {
        ui->resultLabel->setText("⚠ Введите текст для перевода");
        ui->resultLabel->setStyleSheet("color: orange; font-weight: bold;");
        return;
    }

    QString translation = translateSentence(text);

    if (translation.isEmpty()) {
        ui->resultLabel->setText("Не удалось перевести текст");
        ui->resultLabel->setStyleSheet("color: red; font-weight: bold;");
    } else {
        ui->resultLabel->setText(translation);
        ui->resultLabel->setStyleSheet("color: blue; font-weight: bold; font-size: 14px;");
    }
}

void MainWindow::on_textEdit_textChanged()
{
    QString text = ui->textEdit->toPlainText();

    if (text.length() >= 3) {
        // Находим последнее слово
        QStringList words = text.split(QRegularExpression("\\s+"));
        if (!words.isEmpty()) {
            QString lastWord = words.last();
            QString cleanWord = lastWord.toLower();
            cleanWord.remove(QRegularExpression("[^a-zA-Z]"));

            if (cleanWord.length() >= 3) {
                QVector<QString> suggestions = findSuggestions(cleanWord);
                if (!suggestions.isEmpty()) {
                    showPopup(suggestions);
                } else {
                    hidePopup();
                }
                return;
            }
        }
    }
    hidePopup();
}

void MainWindow::on_popupList_itemClicked(QListWidgetItem *item)
{
    if (!item) return;

    QString selected = item->text();
    QString english = selected.split(" → ").first();

    // Заменяем последнее слово в тексте
    QString currentText = ui->textEdit->toPlainText();
    QStringList words = currentText.split(QRegularExpression("\\s+"));
    if (!words.isEmpty()) {
        words[words.size() - 1] = english;
        ui->textEdit->setPlainText(words.join(" "));
    }

    hidePopup();
    ui->textEdit->setFocus();
    ui->textEdit->moveCursor(QTextCursor::End);

    // Обновляем перевод
    on_translateButton_clicked();
}