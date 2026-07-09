#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QStringList>
#include <QRandomGenerator>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QPixmap>
#include <QFile>
#include <QPropertyAnimation>
#include <QDir>
#include <QDialog>
#include <QLineEdit>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class NameInputDialog : public QDialog
{
    Q_OBJECT
public:
    explicit NameInputDialog(QWidget *parent = nullptr);
    QString getEnteredName() const;

private:
    QLineEdit *nameLineEdit;
    QPushButton *okButton;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startGame();
    void nextWord();

private:
    Ui::MainWindow *ui;

    struct Student {
        int id;
        QString name;
        QString photoFileName;
        bool isFemale;
        QLabel *photoLabel;
        QLabel *wordLabel;
        bool active;
    };
    QVector<Student> students;

    QStringList rhymes;
    QStringList currentWords;
    int currentWordIndex;
    int currentStudentIndex;

    QTimer *countTimer;
    QPushButton *startButton;
    QLabel *crownLabel;
    QLabel *longestWordLabel;
    QLabel *centerWordLabel;

    QMediaPlayer *upsSound;
    QAudioOutput *upsAudioOutput;
    QMediaPlayer *victoryMusic;
    QAudioOutput *musicAudioOutput;

    QString targetWinnerName;

    void loadPhotos();
    void loadRhymes();
    void loadSounds();
    void arrangeInCircle();
    void chooseRandomStart();
    void eliminateStudent();
    void startNewRound();
    void showWinner(int winnerIndex);
    void resetGame();
    void animateToCorner(QLabel *label);
    QString findLongestWord(const QString &rhyme);
    void highlightCurrentStudent();
    void ensureTargetWins();
};

#endif