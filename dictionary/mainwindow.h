#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QString>

class QListWidgetItem;  // <- ОБЯЗАТЕЛЬНО ДОБАВЬТЕ ЭТУ СТРОКУ

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_checkButton_clicked();
    void on_inputLineEdit_returnPressed();
    void on_suggestionsList_itemClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;
    QVector<QString> dictionary;
    QString lastInputWord;

    bool loadDictionary(const QString &filePath);
    bool binarySearch(const QString &word, int &index) const;
    int levenshteinDistance(const QString &s1, const QString &s2) const;
    QVector<QString> findClosestWords(const QString &word, int maxCount = 5) const;
    void showSuggestions(const QVector<QString> &suggestions);
    void hideSuggestions();
};

#endif // MAINWINDOW_H