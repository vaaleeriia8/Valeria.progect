#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QPushButton>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void checkPassword();
    void moveHello();

private:
    QLabel *statusLabel;
    QLineEdit *passwordInput1;
    QLineEdit *passwordInput2;
    QPushButton *enterButton;
    QPushButton *clearButton;

    QLabel *helloLabel;
    QTimer *animationTimer;

    int xPos, yPos;
    int step;
    int maxSteps;
    QString savedPassword;

    bool checkPasswordComplexity(const QString &password);
    void showRegistrationScreen();
    void showAnimationScreen();
};

#endif // MAINWINDOW_H