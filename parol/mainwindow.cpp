#include "MainWindow.h"
#include <QApplication>
#include <QPalette>
#include <QFont>
#include <QMessageBox>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), xPos(0), yPos(0), step(0), maxSteps(0)
{
    setWindowTitle("Регистрация");
    resize(400, 250);

    QRect screenGeometry = QApplication::primaryScreen()->geometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);

    showRegistrationScreen();
}

MainWindow::~MainWindow()
{
    delete animationTimer;
}

void MainWindow::showRegistrationScreen()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QLabel *titleLabel = new QLabel("Регистрация", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont("Arial", 14, QFont::Bold);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    QLabel *requirementsLabel = new QLabel(
        "Пароль: минимум 8 символов\n"
        "Нужны заглавные, строчные буквы, цифры и символы",
        this
        );
    requirementsLabel->setAlignment(Qt::AlignCenter);
    requirementsLabel->setStyleSheet("font-size: 9px; color: gray;");
    mainLayout->addWidget(requirementsLabel);

    mainLayout->addSpacing(10);

    QHBoxLayout *passwordLayout1 = new QHBoxLayout();
    QLabel *label1 = new QLabel("Пароль:", this);
    label1->setFixedWidth(60);
    passwordLayout1->addWidget(label1);

    passwordInput1 = new QLineEdit(this);
    passwordInput1->setEchoMode(QLineEdit::Password);
    passwordInput1->setPlaceholderText("Введите пароль");
    passwordLayout1->addWidget(passwordInput1);

    mainLayout->addLayout(passwordLayout1);

    QHBoxLayout *passwordLayout2 = new QHBoxLayout();
    QLabel *label2 = new QLabel("Повторите:", this);
    label2->setFixedWidth(60);
    passwordLayout2->addWidget(label2);

    passwordInput2 = new QLineEdit(this);
    passwordInput2->setEchoMode(QLineEdit::Password);
    passwordInput2->setPlaceholderText("Повторите пароль");
    passwordLayout2->addWidget(passwordInput2);

    mainLayout->addLayout(passwordLayout2);

    mainLayout->addSpacing(10);

    statusLabel = new QLabel("", this);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet("color: red;");
    mainLayout->addWidget(statusLabel);

    QHBoxLayout *buttonLayout = new QHBoxLayout();

    enterButton = new QPushButton("Войти", this);
    buttonLayout->addWidget(enterButton);

    clearButton = new QPushButton("Очистить", this);
    buttonLayout->addWidget(clearButton);

    mainLayout->addLayout(buttonLayout);

    connect(enterButton, &QPushButton::clicked, this, &MainWindow::checkPassword);
    connect(clearButton, &QPushButton::clicked, [this]() {
        passwordInput1->clear();
        passwordInput2->clear();
        statusLabel->setText("");
    });

    connect(passwordInput1, &QLineEdit::returnPressed, this, &MainWindow::checkPassword);
    connect(passwordInput2, &QLineEdit::returnPressed, this, &MainWindow::checkPassword);
}

bool MainWindow::checkPasswordComplexity(const QString &password)
{
    if (password.length() < 8) {
        statusLabel->setText("Ошибка: минимум 8 символов");
        return false;
    }

    bool hasUpper = false;
    bool hasLower = false;
    bool hasDigit = false;
    bool hasSymbol = false;

    for (QChar ch : password) {
        if (ch.isUpper()) hasUpper = true;
        else if (ch.isLower()) hasLower = true;
        else if (ch.isDigit()) hasDigit = true;
        else hasSymbol = true;
    }

    if (!hasUpper) {
        statusLabel->setText("Ошибка: нужны заглавные буквы");
        return false;
    }
    if (!hasLower) {
        statusLabel->setText("Ошибка: нужны строчные буквы");
        return false;
    }
    if (!hasDigit) {
        statusLabel->setText("Ошибка: нужны цифры");
        return false;
    }
    if (!hasSymbol) {
        statusLabel->setText("Ошибка: нужны спецсимволы");
        return false;
    }

    return true;
}

void MainWindow::checkPassword()
{
    QString password1 = passwordInput1->text();
    QString password2 = passwordInput2->text();

    if (password1.isEmpty() || password2.isEmpty()) {
        statusLabel->setText("Заполните оба поля");
        return;
    }

    if (!checkPasswordComplexity(password1)) {
        return;
    }

    if (password1 != password2) {
        statusLabel->setText("Пароли не совпадают");
        return;
    }

    savedPassword = password1;
    statusLabel->setStyleSheet("color: green;");
    statusLabel->setText("Пароль принят!");

    QTimer::singleShot(1000, this, &MainWindow::showAnimationScreen);
}

void MainWindow::showAnimationScreen()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    helloLabel = new QLabel("Hello", centralWidget);

    QFont font("Arial", 36, QFont::Bold);
    helloLabel->setFont(font);
    helloLabel->setAlignment(Qt::AlignCenter);

    QPalette palette = helloLabel->palette();
    palette.setColor(QPalette::WindowText, Qt::black);
    helloLabel->setPalette(palette);

    xPos = 0;
    yPos = 0;
    helloLabel->move(xPos, yPos);

    step = 0;
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &MainWindow::moveHello);
    animationTimer->start(50);
}

void MainWindow::moveHello()
{
    xPos += 15;
    yPos += 15;

    int windowWidth = centralWidget()->width();
    int windowHeight = centralWidget()->height();

    QFontMetrics fm(helloLabel->font());
    int labelWidth = fm.horizontalAdvance("Hello");
    int labelHeight = fm.height();

    if (xPos + labelWidth >= windowWidth || yPos + labelHeight >= windowHeight) {
        animationTimer->stop();

        QMessageBox::information(this, "Добро пожаловать!",
                                 "Добро пожаловать в систему!");

        QTimer::singleShot(2000, this, &QWidget::close);
        return;
    }

    helloLabel->move(xPos, yPos);
}