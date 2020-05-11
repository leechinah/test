/********************************************************************************
** Form generated from reading UI file 'UartTool.ui'
**
** Created by: Qt User Interface Compiler version 5.6.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UARTTOOL_H
#define UI_UARTTOOL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_UartToolClass
{
public:
    QWidget *centralWidget;
    QComboBox *comboBox_portName;
    QComboBox *comboBox_baudRate;
    QLabel *label;
    QLabel *label_2;
    QPushButton *pushButton_comopen;
    QPushButton *pushButton_send;
    QTextBrowser *textEdit_read;
    QLineEdit *lineEdit_write;
    QPushButton *pushButton_OpenBin;
    QPushButton *pushButton_ClearBin;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *UartToolClass)
    {
        if (UartToolClass->objectName().isEmpty())
            UartToolClass->setObjectName(QStringLiteral("UartToolClass"));
        UartToolClass->resize(598, 521);
        centralWidget = new QWidget(UartToolClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        comboBox_portName = new QComboBox(centralWidget);
        comboBox_portName->setObjectName(QStringLiteral("comboBox_portName"));
        comboBox_portName->setGeometry(QRect(90, 50, 69, 22));
        comboBox_baudRate = new QComboBox(centralWidget);
        comboBox_baudRate->setObjectName(QStringLiteral("comboBox_baudRate"));
        comboBox_baudRate->setGeometry(QRect(90, 100, 69, 22));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(30, 50, 54, 12));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(30, 100, 54, 12));
        pushButton_comopen = new QPushButton(centralWidget);
        pushButton_comopen->setObjectName(QStringLiteral("pushButton_comopen"));
        pushButton_comopen->setGeometry(QRect(70, 150, 75, 23));
        pushButton_send = new QPushButton(centralWidget);
        pushButton_send->setObjectName(QStringLiteral("pushButton_send"));
        pushButton_send->setGeometry(QRect(70, 420, 75, 23));
        textEdit_read = new QTextBrowser(centralWidget);
        textEdit_read->setObjectName(QStringLiteral("textEdit_read"));
        textEdit_read->setGeometry(QRect(190, 20, 361, 381));
        lineEdit_write = new QLineEdit(centralWidget);
        lineEdit_write->setObjectName(QStringLiteral("lineEdit_write"));
        lineEdit_write->setGeometry(QRect(190, 420, 361, 31));
        pushButton_OpenBin = new QPushButton(centralWidget);
        pushButton_OpenBin->setObjectName(QStringLiteral("pushButton_OpenBin"));
        pushButton_OpenBin->setGeometry(QRect(70, 200, 75, 23));
        pushButton_ClearBin = new QPushButton(centralWidget);
        pushButton_ClearBin->setObjectName(QStringLiteral("pushButton_ClearBin"));
        pushButton_ClearBin->setGeometry(QRect(70, 240, 75, 23));
        UartToolClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(UartToolClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 598, 23));
        UartToolClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(UartToolClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        UartToolClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(UartToolClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        UartToolClass->setStatusBar(statusBar);

        retranslateUi(UartToolClass);

        QMetaObject::connectSlotsByName(UartToolClass);
    } // setupUi

    void retranslateUi(QMainWindow *UartToolClass)
    {
        UartToolClass->setWindowTitle(QApplication::translate("UartToolClass", "UartTool", 0));
        comboBox_baudRate->clear();
        comboBox_baudRate->insertItems(0, QStringList()
         << QApplication::translate("UartToolClass", "230400", 0)
         << QApplication::translate("UartToolClass", "9600", 0)
         << QApplication::translate("UartToolClass", "19200", 0)
         << QApplication::translate("UartToolClass", "38400", 0)
         << QApplication::translate("UartToolClass", "115200", 0)
        );
        label->setText(QApplication::translate("UartToolClass", "Uart", 0));
        label_2->setText(QApplication::translate("UartToolClass", "Rate", 0));
        pushButton_comopen->setText(QApplication::translate("UartToolClass", "Open", 0));
        pushButton_send->setText(QApplication::translate("UartToolClass", "Send", 0));
        lineEdit_write->setText(QApplication::translate("UartToolClass", "1E 01 00 00 00 00 00 01 00 00 0F 2E", 0));
        pushButton_OpenBin->setText(QApplication::translate("UartToolClass", "File", 0));
        pushButton_ClearBin->setText(QApplication::translate("UartToolClass", "Clear", 0));
    } // retranslateUi

};

namespace Ui {
    class UartToolClass: public Ui_UartToolClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UARTTOOL_H
