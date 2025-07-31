/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *inputLineEdit;
    QPushButton *selectAiButton;
    QPushButton *convertAndParseButton;
    QGraphicsView *graphicsView;
    QPushButton *saveAsButton;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(600, 550);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        label = new QLabel(centralwidget);
        label->setObjectName("label");

        horizontalLayout->addWidget(label);

        inputLineEdit = new QLineEdit(centralwidget);
        inputLineEdit->setObjectName("inputLineEdit");

        horizontalLayout->addWidget(inputLineEdit);

        selectAiButton = new QPushButton(centralwidget);
        selectAiButton->setObjectName("selectAiButton");

        horizontalLayout->addWidget(selectAiButton);


        verticalLayout->addLayout(horizontalLayout);

        convertAndParseButton = new QPushButton(centralwidget);
        convertAndParseButton->setObjectName("convertAndParseButton");
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        convertAndParseButton->setFont(font);

        verticalLayout->addWidget(convertAndParseButton);

        graphicsView = new QGraphicsView(centralwidget);
        graphicsView->setObjectName("graphicsView");

        verticalLayout->addWidget(graphicsView);

        saveAsButton = new QPushButton(centralwidget);
        saveAsButton->setObjectName("saveAsButton");

        verticalLayout->addWidget(saveAsButton);

        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "AI to QPainterPath Converter", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "Input AI File:", nullptr));
        selectAiButton->setText(QCoreApplication::translate("MainWindow", "Browse...", nullptr));
        convertAndParseButton->setText(QCoreApplication::translate("MainWindow", "Convert AI and Parse to QPainterPath", nullptr));
        saveAsButton->setText(QCoreApplication::translate("MainWindow", "Save As...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
