/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.10.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QLabel *label;
    QLabel *titleLabel;
    QLineEdit *inputLineEdit;
    QLabel *digitLabel;
    QLabel *russianLabel;
    QLabel *russianDigitLabel;
    QLabel *digitalDateLabel;
    QLabel *russianDateLabel;
    QLabel *russianDigitalDateLabel;
    QListWidget *popupList;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        label->setGeometry(QRect(20, 60, 241, 41));
        titleLabel = new QLabel(centralwidget);
        titleLabel->setObjectName("titleLabel");
        titleLabel->setGeometry(QRect(20, 10, 281, 41));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        titleLabel->setFont(font);
        inputLineEdit = new QLineEdit(centralwidget);
        inputLineEdit->setObjectName("inputLineEdit");
        inputLineEdit->setGeometry(QRect(240, 70, 281, 61));
        digitLabel = new QLabel(centralwidget);
        digitLabel->setObjectName("digitLabel");
        digitLabel->setGeometry(QRect(30, 180, 121, 41));
        russianLabel = new QLabel(centralwidget);
        russianLabel->setObjectName("russianLabel");
        russianLabel->setGeometry(QRect(30, 230, 121, 41));
        russianDigitLabel = new QLabel(centralwidget);
        russianDigitLabel->setObjectName("russianDigitLabel");
        russianDigitLabel->setGeometry(QRect(30, 280, 151, 41));
        digitalDateLabel = new QLabel(centralwidget);
        digitalDateLabel->setObjectName("digitalDateLabel");
        digitalDateLabel->setGeometry(QRect(160, 190, 181, 31));
        russianDateLabel = new QLabel(centralwidget);
        russianDateLabel->setObjectName("russianDateLabel");
        russianDateLabel->setGeometry(QRect(160, 230, 381, 31));
        russianDigitalDateLabel = new QLabel(centralwidget);
        russianDigitalDateLabel->setObjectName("russianDigitalDateLabel");
        russianDigitalDateLabel->setGeometry(QRect(30, 330, 331, 31));
        popupList = new QListWidget(centralwidget);
        popupList->setObjectName("popupList");
        popupList->setGeometry(QRect(20, 370, 461, 192));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 25));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\320\222\320\262\320\265\320\264\320\270\321\202\320\265 \320\264\320\260\321\202\321\203 \320\275\320\260 \320\260\320\275\320\263\320\273\320\270\320\271\321\201\320\272\320\276\320\274:", nullptr));
        titleLabel->setText(QCoreApplication::translate("MainWindow", "\320\237\321\200\320\265\320\276\320\261\321\200\320\260\320\267\320\276\320\262\320\260\321\202\320\265\320\273\321\214 \320\264\320\260\321\202 ", nullptr));
        digitLabel->setText(QCoreApplication::translate("MainWindow", "\320\246\320\270\321\204\321\200\320\276\320\262\320\276\320\271:", nullptr));
        russianLabel->setText(QCoreApplication::translate("MainWindow", "\320\240\321\203\321\201\321\201\320\272\320\270\320\271:", nullptr));
        russianDigitLabel->setText(QCoreApplication::translate("MainWindow", "\320\240\321\203\321\201\321\201\320\272\320\270\320\271 \321\206\320\270\321\204\321\200\320\276\320\262\320\276\320\271:", nullptr));
        digitalDateLabel->setText(QString());
        russianDateLabel->setText(QString());
        russianDigitalDateLabel->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
