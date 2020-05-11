#pragma once

#include <QtWidgets/QMainWindow>
#include <QtSerialPort/QSerialport>
#include <QtSerialPort/QSerialportinfo>
#include <QTimer>
#include "ui_UartTool.h"

#define BOOT_OR_APP     0x01    //0x01-App 0x00-Boot

class UartTool : public QMainWindow
{
	Q_OBJECT

public:
	UartTool(QWidget *parent = Q_NULLPTR);

    char ConvertHexChar(char ch);
    void StringToHex(QString str, QByteArray &senddata);
    
    quint16 UpDataPackNow;
    quint16 UpDataPackAll;
    quint32 UpDataSizeAll;
    QByteArray SendData;
    QByteArray arry;

private slots:
    void slot_pushButton_open_clicked();
    void slot_pushButton_send_clicked();
    void slot_readCom(); //用于读取数据
    void slot_pushButton_OpenBin_clicked();
    void slot_pushButton_ClearBin_clicked();
    void slot_getdata();

private:
	Ui::UartToolClass ui;
    QSerialPort *pSerialPort;   //(实例化一个指向串口的指针，可以用于访问串口)
	QByteArray requestData;     //（用于存储从串口那读取的数据）
    QTimer *timer;//（用于计时）
};
