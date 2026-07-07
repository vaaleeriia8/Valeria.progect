#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include <QString>
#include <QVector>

class QListWidgetItem;

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
    void on_inputLineEdit_textChanged(const QString &text);
    void on_popupList_itemClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;

    // Словари
    QMap<QString, QString> monthEnToRu;      // месяц англ -> русский
    QMap<QString, int> monthEnToNum;         // месяц англ -> число
    QMap<QString, QString> dayEnToRu;        // день англ -> русский
    QMap<QString, int> dayEnToNum;           // день англ -> число
    QMap<QString, QString> numberEnToRu;     // число англ -> русский
    QMap<QString, int> numberEnToNum;        // число англ -> число

    bool loadDictionary(const QString &filePath);
    QString parseDate(const QString &text,
                      QString &digitalDate,
                      QString &russianDate,
                      QString &russianDigitalDate);
    int wordToNumber(const QString &word) const;
    QString numberToRussian(int num, bool isDay = false, bool isYear = false) const;
    int parseYear(const QStringList &words, int &pos) const;
    QVector<QString> findSuggestions(const QString &word) const;
    void showPopup(const QVector<QString> &suggestions);
    void hidePopup();
    void updateDateDisplay();
};

#endif // MAINWINDOW_H