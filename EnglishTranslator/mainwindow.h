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
    void on_translateButton_clicked();
    void on_textEdit_textChanged();
    void on_popupList_itemClicked(QListWidgetItem *item);

private:
    Ui::MainWindow *ui;

    // Словарь: английское слово -> русский перевод
    QMap<QString, QString> dictionary;

    bool loadDictionary(const QString &filePath);
    int levenshteinDistance(const QString &s1, const QString &s2) const;
    QVector<QString> findSuggestions(const QString &word, int maxCount = 5) const;
    QString getTranslation(const QString &word) const;
    QString translateSentence(const QString &sentence) const;
    bool isPastTense(const QString &sentence) const;
    QString conjugateVerb(const QString &verb, bool pastTense) const;
    void showPopup(const QVector<QString> &suggestions);
    void hidePopup();
};

#endif // MAINWINDOW_H