#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QCoreApplication>
#include <QDir>
#include <cmath>

NameInputDialog::NameInputDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Введите вашу фамилию");
    setFixedSize(300, 150);
    setModal(true);

    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *label = new QLabel("Введите фамилию ", this);
    label->setAlignment(Qt::AlignCenter);
    label->setFont(QFont("Arial", 12));
    layout->addWidget(label);

    nameLineEdit = new QLineEdit(this);
    nameLineEdit->setFont(QFont("Arial", 14));
    layout->addWidget(nameLineEdit);

    okButton = new QPushButton("Начать игру", this);
    okButton->setFont(QFont("Arial", 12, QFont::Bold));
    okButton->setStyleSheet("background-color: #4CAF50; color: white; border-radius: 8px; padding: 8px;");
    layout->addWidget(okButton);

    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(nameLineEdit, &QLineEdit::returnPressed, this, &QDialog::accept);
}

QString NameInputDialog::getEnteredName() const
{
    return nameLineEdit->text().trimmed();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , countTimer(nullptr)
    , crownLabel(nullptr)
    , longestWordLabel(nullptr)
    , centerWordLabel(nullptr)
    , upsSound(nullptr)
    , upsAudioOutput(nullptr)
    , victoryMusic(nullptr)
    , musicAudioOutput(nullptr)
{
    ui->setupUi(this);

    setWindowTitle("Считалка");
    setFixedSize(1400, 900);

    NameInputDialog nameDialog(this);
    if (nameDialog.exec() == QDialog::Accepted) {
        targetWinnerName = nameDialog.getEnteredName();
        if (targetWinnerName.isEmpty()) {
            targetWinnerName = "Пользователь";
        }
    } else {
        targetWinnerName = "Пользователь";
    }

    longestWordLabel = new QLabel(this);
    longestWordLabel->setAlignment(Qt::AlignRight | Qt::AlignTop);
    longestWordLabel->setFont(QFont("Arial", 16, QFont::Bold));
    longestWordLabel->setStyleSheet("background-color: transparent; color: #FFD700;");
    longestWordLabel->setGeometry(1050, 20, 300, 40);
    longestWordLabel->hide();

    centerWordLabel = new QLabel(this);
    centerWordLabel->setAlignment(Qt::AlignCenter);
    centerWordLabel->setFont(QFont("Arial", 14, QFont::Bold));
    centerWordLabel->setStyleSheet("background-color: transparent; color: #00FF00;");
    centerWordLabel->setFixedSize(200, 40);
    centerWordLabel->move(width()/2 - 100, height()/2 - 20);
    centerWordLabel->hide();

    startButton = new QPushButton("Начать игру", this);
    startButton->setGeometry(600, 800, 200, 50);
    startButton->setFont(QFont("Arial", 14, QFont::Bold));
    startButton->setStyleSheet("background-color: #4CAF50; color: white; border-radius: 10px;");
    connect(startButton, &QPushButton::clicked, this, &MainWindow::startGame);

    loadSounds();
    loadPhotos();
    loadRhymes();
}

MainWindow::~MainWindow()
{
    delete ui;
    if (countTimer) delete countTimer;
    if (upsSound) delete upsSound;
    if (upsAudioOutput) delete upsAudioOutput;
    if (victoryMusic) delete victoryMusic;
    if (musicAudioOutput) delete musicAudioOutput;
    if (centerWordLabel) delete centerWordLabel;
}

void MainWindow::loadSounds()
{
    QString resourcesPath = QCoreApplication::applicationDirPath() + "/resources/";

    upsSound = new QMediaPlayer(this);
    upsAudioOutput = new QAudioOutput(this);
    upsSound->setAudioOutput(upsAudioOutput);
    upsAudioOutput->setVolume(1.0);

    QString upsPath = resourcesPath + "sounds/ups.mp3";
    if (QFile::exists(upsPath)) {
        upsSound->setSource(QUrl::fromLocalFile(upsPath));
    }

    victoryMusic = new QMediaPlayer(this);
    musicAudioOutput = new QAudioOutput(this);
    victoryMusic->setAudioOutput(musicAudioOutput);
    musicAudioOutput->setVolume(1.0);

    QString musicPath = resourcesPath + "sounds/Stairway_to_Heaven.mp3";
    if (QFile::exists(musicPath)) {
        victoryMusic->setSource(QUrl::fromLocalFile(musicPath));
    }
}

void MainWindow::loadPhotos()
{
    QString resourcesPath = QCoreApplication::applicationDirPath() + "/resources/";
    QString photosPath = resourcesPath + "photos/";

    int w = 80, h = 106;

    QDir dir(photosPath);
    QStringList filters;
    filters << "*.jpg" << "*.png" << "*.jpeg";
    dir.setNameFilters(filters);
    QFileInfoList fileList = dir.entryInfoList();

    if (fileList.isEmpty()) {
        return;
    }

    int studentId = 1;
    for (const QFileInfo &fileInfo : fileList) {
        Student s;
        s.id = studentId;
        s.photoFileName = fileInfo.fileName();
        s.active = true;

        QString baseName = fileInfo.baseName();
        s.name = baseName;

        QStringList femaleNames = {"Андрейковец", "Галуза", "Сахарова", "Гордевич", "Корж", "Сеноженская", "Чопко", "Милякова"};
        s.isFemale = femaleNames.contains(baseName);

        QLabel *photo = new QLabel(this);
        QPixmap pix(fileInfo.absoluteFilePath());

        if (!pix.isNull()) {
            photo->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            photo->setScaledContents(true);
        } else {
            photo->setFixedSize(w, h);
            photo->setStyleSheet(QString("background-color: lightblue; border: 2px solid #333; border-radius: 10px; font-size: 20px;"));
            photo->setAlignment(Qt::AlignCenter);
            photo->setText(baseName);
        }

        photo->setFixedSize(w, h);
        photo->setStyleSheet(photo->styleSheet() + " border-radius: 10px;");
        photo->show();

        QLabel *word = new QLabel(this);
        word->setFixedSize(w + 30, 45);
        word->setAlignment(Qt::AlignCenter);
        word->setFont(QFont("Arial", 10, QFont::Bold));
        word->setWordWrap(true);
        word->setStyleSheet("background-color: transparent; color: #00FF00; font-weight: bold; font-size: 11px;");
        word->setText("");
        word->hide();

        s.photoLabel = photo;
        s.wordLabel = word;
        students.append(s);
        studentId++;
    }

    arrangeInCircle();
}

void MainWindow::arrangeInCircle()
{
    if (students.isEmpty()) return;

    int cx = 700, cy = 420, r = 340;
    int w = 80, h = 106;
    int count = students.size();

    for (int i = 0; i < count; i++) {
        double angle = i * 2 * M_PI / count - M_PI / 2;
        int x = cx + r * cos(angle) - w/2;
        int y = cy + r * sin(angle) - h/2;

        students[i].photoLabel->move(x, y);
        students[i].wordLabel->move(x - 15, y + h + 5);
    }
}

void MainWindow::loadRhymes()
{
    QString rhymesPath = QCoreApplication::applicationDirPath() + "/resources/counting_rhymes.txt";
    QFile file(rhymesPath);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            if (!line.isEmpty()) {
                rhymes.append(line);
            }
        }
        file.close();
    }

    if (rhymes.isEmpty()) {
        rhymes << "Раз два три четыре пять шесть семь восемь девять десять вышел месяц на гору"
               << "Шла машина темным лесом за каким-то интересом инте инте интерес выходи на букву с"
               << "На золотом крыльце сидели царь царевич король королевич сапожник портной кто ты такой";
    }
}

QString MainWindow::findLongestWord(const QString &rhyme)
{
    QStringList words = rhyme.split(" ");
    QString longest;
    for (const QString &word : words) {
        if (word.length() > longest.length()) {
            longest = word;
        }
    }
    return longest;
}

void MainWindow::startGame()
{
    if (students.isEmpty()) {
        QMessageBox::critical(this, "Ошибка", "Нет фотографий студентов!");
        return;
    }

    int targetIndex = -1;
    for (int i = 0; i < students.size(); i++) {
        if (students[i].name == targetWinnerName) {
            targetIndex = i;
            break;
        }
    }

    if (targetIndex == -1) {
        QMessageBox::warning(this, "Предупреждение",
                             QString("Студент с фамилией '%1' не найден в списке.\n"
                                     "Победитель будет выбран случайно.")
                                 .arg(targetWinnerName));
    }

    startButton->hide();
    resetGame();

    int idx = QRandomGenerator::global()->bounded(rhymes.size());
    QString rhyme = rhymes[idx];
    currentWords = rhyme.split(" ");
    currentWordIndex = 0;

    QString longestWord = findLongestWord(rhyme);
    longestWordLabel->setText(longestWord);
    longestWordLabel->show();

    chooseRandomStart();

    students[currentStudentIndex].wordLabel->setText(currentWords[0]);
    students[currentStudentIndex].wordLabel->show();

    centerWordLabel->setText(currentWords[0]);
    centerWordLabel->show();
    QTimer::singleShot(600, this, [this]() {
        if (centerWordLabel) centerWordLabel->hide();
    });

    currentWordIndex++;

    highlightCurrentStudent();

    countTimer = new QTimer(this);
    connect(countTimer, &QTimer::timeout, this, &MainWindow::nextWord);
    countTimer->start(700);
}

void MainWindow::chooseRandomStart()
{
    QVector<int> active;
    for (int i = 0; i < students.size(); i++) {
        if (students[i].active) active.append(i);
    }
    if (!active.isEmpty()) {
        currentStudentIndex = active[QRandomGenerator::global()->bounded(active.size())];
    }
}

void MainWindow::highlightCurrentStudent()
{
    for (int i = 0; i < students.size(); i++) {
        if (students[i].active && students[i].photoLabel) {
            QString style = students[i].photoLabel->styleSheet();
            style.replace("border: 3px solid red;", "");
            style.replace("border: 3px solid red", "");
            students[i].photoLabel->setStyleSheet(style);
        }
    }

    if (currentStudentIndex >= 0 && currentStudentIndex < students.size() && students[currentStudentIndex].active) {
        QString currentStyle = students[currentStudentIndex].photoLabel->styleSheet();
        students[currentStudentIndex].photoLabel->setStyleSheet(currentStyle + "border: 3px solid red;");
    }
}

void MainWindow::nextWord()
{
    if (currentWordIndex >= currentWords.size()) {
        eliminateStudent();
        return;
    }

    int next = (currentStudentIndex + 1) % students.size();
    while (!students[next].active) {
        next = (next + 1) % students.size();
    }
    currentStudentIndex = next;

    QString currentWord = currentWords[currentWordIndex];

    students[currentStudentIndex].wordLabel->setText(currentWord);
    students[currentStudentIndex].wordLabel->show();

    centerWordLabel->setText(currentWord);
    centerWordLabel->show();
    QTimer::singleShot(600, this, [this]() {
        if (centerWordLabel) centerWordLabel->hide();
    });

    currentWordIndex++;

    highlightCurrentStudent();
}

void MainWindow::animateToCorner(QLabel *label)
{
    if (!label) return;

    QPropertyAnimation *animation = new QPropertyAnimation(label, "geometry");
    animation->setDuration(800);
    animation->setStartValue(label->geometry());
    animation->setEndValue(QRect(1200, 20, label->width(), label->height()));
    animation->start(QAbstractAnimation::DeleteWhenStopped);

    QPropertyAnimation *opacityAnimation = new QPropertyAnimation(label, "windowOpacity");
    opacityAnimation->setDuration(800);
    opacityAnimation->setStartValue(1.0);
    opacityAnimation->setEndValue(0.0);
    opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::ensureTargetWins()
{
    QVector<int> activeIndices;
    for (int i = 0; i < students.size(); i++) {
        if (students[i].active) {
            activeIndices.append(i);
        }
    }

    if (activeIndices.size() != 2) return;

    int idx1 = activeIndices[0];
    int idx2 = activeIndices[1];

    bool isTarget1 = (students[idx1].name == targetWinnerName);
    bool isTarget2 = (students[idx2].name == targetWinnerName);

    if (!isTarget1 && !isTarget2) {
        return;
    }

    if (isTarget1) {
        currentStudentIndex = idx2;
    } else if (isTarget2) {
        currentStudentIndex = idx1;
    }
}

void MainWindow::eliminateStudent()
{
    if (upsSound && upsSound->source().isValid()) {
        upsSound->play();
    }

    int activeCount = 0;
    for (const Student &s : students) {
        if (s.active) activeCount++;
    }

    if (activeCount == 2) {
        ensureTargetWins();
    }

    int nextStudentIndex = (currentStudentIndex + 1) % students.size();
    while (!students[nextStudentIndex].active) {
        nextStudentIndex = (nextStudentIndex + 1) % students.size();
    }
    QString nextStudentName = students[nextStudentIndex].name;

    QLabel *photoToAnimate = students[currentStudentIndex].photoLabel;
    QLabel *wordToAnimate = students[currentStudentIndex].wordLabel;

    animateToCorner(photoToAnimate);
    animateToCorner(wordToAnimate);

    students[currentStudentIndex].active = false;

    QVector<Student> newStudents;
    for (int i = 0; i < students.size(); i++) {
        if (students[i].active) {
            newStudents.append(students[i]);
        } else {
            QTimer::singleShot(800, [photoToAnimate, wordToAnimate]() {
                photoToAnimate->hide();
                wordToAnimate->hide();
            });
        }
    }

    students = newStudents;

    for (int i = 0; i < students.size(); i++) {
        students[i].id = i + 1;
    }

    int newNextIndex = -1;
    for (int i = 0; i < students.size(); i++) {
        if (students[i].name == nextStudentName) {
            newNextIndex = i;
            break;
        }
    }

    if (newNextIndex != -1) {
        currentStudentIndex = newNextIndex;
    } else {
        currentStudentIndex = 0;
    }

    arrangeInCircle();

    int newActiveCount = students.size();

    if (newActiveCount == 1) {
        if (countTimer) {
            countTimer->stop();
        }
        showWinner(0);
        return;
    }

    if (countTimer) {
        countTimer->stop();
        delete countTimer;
        countTimer = nullptr;
    }

    QTimer::singleShot(800, this, &MainWindow::startNewRound);
}

void MainWindow::startNewRound()
{
    int idx = QRandomGenerator::global()->bounded(rhymes.size());
    QString rhyme = rhymes[idx];
    currentWords = rhyme.split(" ");
    currentWordIndex = 0;

    QString longestWord = findLongestWord(rhyme);
    longestWordLabel->setText(longestWord);

    if (currentStudentIndex >= students.size()) {
        currentStudentIndex = 0;
    }

    if (!students.isEmpty() && students[currentStudentIndex].active) {
        QString firstWord = currentWords[0];

        students[currentStudentIndex].wordLabel->setText(firstWord);
        students[currentStudentIndex].wordLabel->show();

        centerWordLabel->setText(firstWord);
        centerWordLabel->show();
        QTimer::singleShot(600, this, [this]() {
            if (centerWordLabel) centerWordLabel->hide();
        });

        currentWordIndex++;
        highlightCurrentStudent();
    }

    countTimer = new QTimer(this);
    connect(countTimer, &QTimer::timeout, this, &MainWindow::nextWord);
    countTimer->start(700);
}

void MainWindow::showWinner(int winnerIndex)
{
    if (students.isEmpty()) return;

    students[winnerIndex].wordLabel->hide();

    QString resourcesPath = QCoreApplication::applicationDirPath() + "/resources/";
    QString crownPath = resourcesPath + "crown.png";

    crownLabel = new QLabel(this);
    QPixmap crown(crownPath);

    if (!crown.isNull()) {
        crownLabel->setPixmap(crown.scaled(70, 70, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        crownLabel->setStyleSheet("background-color: transparent;");
    } else {
        crownLabel->setText("🏆");
        crownLabel->setFont(QFont("Arial", 48));
        crownLabel->setStyleSheet("background-color: transparent;");
    }

    crownLabel->adjustSize();
    crownLabel->move(students[winnerIndex].photoLabel->x() + 5,
                     students[winnerIndex].photoLabel->y() - 55);
    crownLabel->show();

    if (victoryMusic && victoryMusic->source().isValid()) {
        victoryMusic->play();
    }

    QString result;
    if (students[winnerIndex].isFemale) {
        result = QString("%1 ПОБЕДИЛА! 🎉")
                     .arg(students[winnerIndex].name);
    } else {
        result = QString("%1 ПОБЕДИЛ! 🎉")
                     .arg(students[winnerIndex].name);
    }

    if (students[winnerIndex].name == targetWinnerName) {
        result += "\n\nВы успешно победили! Поздравляем! 🥳";
    }

    QMessageBox::information(this, "ПОБЕДИТЕЛЬ!", result);

    longestWordLabel->hide();
    startButton->show();
    startButton->setText("Новая игра");
}

void MainWindow::resetGame()
{
    if (countTimer) {
        countTimer->stop();
        delete countTimer;
        countTimer = nullptr;
    }

    for (int i = 0; i < students.size(); i++) {
        delete students[i].photoLabel;
        delete students[i].wordLabel;
    }
    students.clear();

    loadPhotos();

    if (victoryMusic && victoryMusic->playbackState() == QMediaPlayer::PlayingState) {
        victoryMusic->stop();
    }

    if (crownLabel) {
        crownLabel->hide();
        delete crownLabel;
        crownLabel = nullptr;
    }

    longestWordLabel->hide();
    if (centerWordLabel) centerWordLabel->hide();
}