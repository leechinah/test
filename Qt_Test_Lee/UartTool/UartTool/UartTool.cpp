#include "UartTool.h"
#include <windows.h>

#include <QtSerialPort/QSerialport>
#include <QtSerialPort/QSerialportinfo>
#include <QTimer>
#include <QDebug>
#include <QFile>
#include <QFileDialog>


UartTool::UartTool(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

    foreach(const QSerialPortInfo &Info, QSerialPortInfo::availablePorts())//读取串口信息
    {
        qDebug() << "portName    :" << Info.portName();//调试时可以看的串口信息
        qDebug() << "Description   :" << Info.description();
        qDebug() << "Manufacturer:" << Info.manufacturer();

        QSerialPort serial;
        serial.setPort(Info);

        if (serial.open(QIODevice::ReadWrite))//如果串口是可以读写方式打开的
        {
            ui.comboBox_portName->addItem(Info.portName());//在comboBox那添加串口号
            serial.close();//然后自动关闭等待人为开启（通过那个打开串口的PushButton）
        }
    }

    pSerialPort = nullptr;
    connect(ui.pushButton_comopen, SIGNAL(clicked()), this, SLOT(slot_pushButton_open_clicked()));
    connect(ui.pushButton_send , SIGNAL(clicked()), this, SLOT(slot_pushButton_send_clicked()));
    connect(ui.pushButton_OpenBin, SIGNAL(clicked()), this, SLOT(slot_pushButton_OpenBin_clicked()));
    connect(ui.pushButton_ClearBin, SIGNAL(clicked()), this, SLOT(slot_pushButton_ClearBin_clicked()));

}


void UartTool::slot_pushButton_open_clicked()
{
    //对串口进行一些初始化
    if (pSerialPort == nullptr)
    {
        pSerialPort = new QSerialPort(this);
        pSerialPort->setPortName(ui.comboBox_portName->currentText());
        pSerialPort->open(QIODevice::ReadWrite);
        qDebug() << ui.comboBox_portName->currentText();
        pSerialPort->setBaudRate(ui.comboBox_baudRate->currentText().toInt());//波特率
        pSerialPort->setDataBits(QSerialPort::Data8);//数据字节，8字节
        pSerialPort->setParity(QSerialPort::NoParity);//校验，无
        pSerialPort->setFlowControl(QSerialPort::NoFlowControl);//数据流控制,无
        pSerialPort->setStopBits(QSerialPort::OneStop);//一位停止位

        //timer = new QTimer(this);
        //connect(timer, SIGNAL(timeout()), this, SLOT(slot_readCom()));
        //timer->start(1);//每ms读一次

        //这是串口信息判断，如果有这个信号发出来，调用getdata接收数据。
        //这里需要注意的是，readyread信号发出，不代表你可以一次性把你所发的数据都接收过来。
        //例如：你发一个字符串“123456789”,它可能会发出两个readyread信号，调用两次getdata方法来获取数据
        connect(pSerialPort, SIGNAL(readyRead()), this, SLOT(slot_getdata()));
    }
}

/////////////////////////////////////////////////
/// \brief MainWindow::ConvertHexChar
/// \param ch
/// \return
///
char UartTool::ConvertHexChar(char ch)
{
    if ((ch >= '0') && (ch <= '9'))
        return ch - 0x30;
    else if ((ch >= 'A') && (ch <= 'F'))
        return ch - 'A' + 10;
    else if ((ch >= 'a') && (ch <= 'f'))
        return ch - 'a' + 10;
    else return ch - ch;//不在0-f范围内的会发送成0
}

/////////////////////////////////////////////////
/// \brief MainWindow::StringToHex
/// \param str
/// \param senddata
///
void UartTool::StringToHex(QString str, QByteArray &senddata) //字符串转换为十六进制数据0-F
{
    int hexdata, lowhexdata;
    int hexdatalen = 0;
    int len = str.length();
    senddata.resize(len / 2);
    char lstr, hstr;

    for (int i = 0; i < len;)
    {
        //char lstr,
        hstr = str[i].toLatin1();
        if (hstr == ' ')
        {
            i++;
            continue;
        }
        i++;
        if (i >= len)
            break;
        lstr = str[i].toLatin1();
        hexdata = ConvertHexChar(hstr);
        lowhexdata = ConvertHexChar(lstr);
        if ((hexdata == 16) || (lowhexdata == 16))
            break;
        else
            hexdata = hexdata * 16 + lowhexdata;
        i++;
        senddata[hexdatalen] = (char)hexdata;
        hexdatalen++;
    }
    senddata.resize(hexdatalen);
}

void UartTool::slot_pushButton_send_clicked()
{
    QString str = ui.lineEdit_write->text();
    QByteArray senddata;
    StringToHex(str, senddata); //将str字符串转换为16进制的形式
    if (pSerialPort != nullptr)
    {
        pSerialPort->write(senddata);
    }
}

void UartTool::slot_getdata()
{
    requestData = pSerialPort->readAll();//用requestData存储从串口那读取的数据

    if (requestData != QByteArray(NULL))//如果读取到有数据，就显示出来。
    {
        //ui.textEdit_read->append(requestData);
        if (requestData.length())
        {
            ui.textEdit_read->insertPlainText("\nRX : ");
            QDataStream out(&requestData, QIODevice::ReadWrite);    //将字节数组读入
            while (!out.atEnd())
            {
                qint8 outChar = 0;
                out >> outChar;   //每字节填充一次，直到结束
                //十六进制的转换
                QString str = QString("%1").arg(outChar & 0xFF, 2, 16, QLatin1Char('0'));
                ui.textEdit_read->insertPlainText(str.toUpper());//大写
                ui.textEdit_read->insertPlainText(" ");//每发送两个字符后添加一个空格
                ui.textEdit_read->moveCursor(QTextCursor::End);
            }
        }

        qint16 len = 0;
        if (UpDataPackNow <= UpDataPackAll)
        {
            if (UpDataPackNow * 900 <= UpDataSizeAll)
            {
                len = 900;
            }
            else
            {
                len = UpDataSizeAll - (UpDataPackNow - 1) * 900;
            }
            SendData[0] = 0x1E;
            SendData[1] = 0x03;
            SendData[2] = 0x06;
            SendData[3] = 0;
            SendData[4] = 0;
            SendData[5] = 0;
            SendData[6] = 0;
            SendData[7] = (unsigned char)(len + 5);
            SendData[8] = (unsigned char)((len + 5) >> 8);
            SendData[9] = BOOT_OR_APP;
            SendData[10] = (unsigned char)(UpDataPackNow);
            SendData[11] = (unsigned char)(UpDataPackNow >> 8);
            SendData[12] = (unsigned char)(UpDataPackAll);
            SendData[13] = (unsigned char)(UpDataPackAll >> 8);
            int i;
            for (i = 0; i < len; i++)
            {
                SendData[14 + i] = arry[(UpDataPackNow - 1) * 900 + i];
            }
            SendData[14 + i] = 0x2E;
            UpDataPackNow++;
            pSerialPort->write(SendData);
            SendData.clear();
        }
        else
        {
            arry.clear();
        }
    }
    requestData.clear();    //清除缓冲区
}


void UartTool::slot_readCom()
{
    requestData = pSerialPort->readAll();//用requestData存储从串口那读取的数据
    
    if (requestData != QByteArray(NULL))//如果读取到有数据，就显示出来。
    {
        //ui.textEdit_read->append(requestData);
        if (requestData.length())
        {
                ui.textEdit_read->insertPlainText("\nRX : ");
                QDataStream out(&requestData, QIODevice::ReadWrite);    //将字节数组读入
                while (!out.atEnd())
                {
                    qint8 outChar = 0;
                    out >> outChar;   //每字节填充一次，直到结束
                    //十六进制的转换
                    QString str = QString("%1").arg(outChar & 0xFF, 2, 16, QLatin1Char('0'));
                    ui.textEdit_read->insertPlainText(str.toUpper());//大写
                    ui.textEdit_read->insertPlainText(" ");//每发送两个字符后添加一个空格
                    ui.textEdit_read->moveCursor(QTextCursor::End);
                }
        }

        qint16 len = 0;
        if (UpDataPackNow <= UpDataPackAll)
        {
            if (UpDataPackNow * 900 <= UpDataSizeAll)
            {
                len = 900;
            }
            else
            {
                len = UpDataSizeAll - (UpDataPackNow - 1) * 900;
            }
            SendData[0] = 0x1E;
            SendData[1] = 0x03;
            SendData[2] = 0x06;
            SendData[3] = 0;
            SendData[4] = 0;
            SendData[5] = 0;
            SendData[6] = 0;
            SendData[7] = (unsigned char)(len+5);
            SendData[8] = (unsigned char)((len+5) >> 8);
            SendData[9] = BOOT_OR_APP;
            SendData[10] = (unsigned char)(UpDataPackNow);
            SendData[11] = (unsigned char)(UpDataPackNow >> 8);
            SendData[12] = (unsigned char)(UpDataPackAll);
            SendData[13] = (unsigned char)(UpDataPackAll >> 8);
            int i;
            for (i = 0; i < len; i++)
            {
                SendData[14 + i] = arry[(UpDataPackNow - 1) * 900 + i];
            }
            SendData[14 + i] = 0x2E;
            UpDataPackNow++;
            pSerialPort->write(SendData);
            SendData.clear();
        }
        else
        {
            arry.clear();
        }
    }
    requestData.clear();    //清除缓冲区
}



static unsigned char gu8CrcTable[256] =
{
    0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83,
    0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,
    0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e,
    0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,
    0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0,
    0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62,
    0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d,
    0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,
    0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5,
    0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,
    0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58,
    0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
    0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6,
    0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24,
    0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b,
    0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,
    0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f,
    0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,
    0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92,
    0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50,
    0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c,
    0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee,
    0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1,
    0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,
    0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49,
    0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,
    0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4,
    0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16,
    0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a,
    0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,
    0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7,
    0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35,
};

char DrvGetCrc8(unsigned char* pData, quint16 u16Length)
{
    unsigned char u8CrcResult = 0;
    unsigned char temp = 0;

    for (; u16Length > 0; u16Length--)
    {
        temp = u8CrcResult ^ (*pData);
        u8CrcResult = gu8CrcTable[temp];
        pData++;
    }

    return u8CrcResult;
}

const quint8 auchCRCHi[] =
{
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
    0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
    0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
    0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
    0x40
};

const quint8 auchCRCLo[] =
{
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
    0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
    0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
    0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
    0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
    0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
    0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
    0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
    0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
    0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
    0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
    0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
    0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
    0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
    0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
    0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
    0x40
};

quint16   N_CRC16(quint8 *updata, quint32 len, quint16 Crc16)
{
    quint8 uchCRCHi;
    quint8 uchCRCLo;

    quint16  uindex;

    uchCRCLo = Crc16;
    uchCRCHi = Crc16 >> 8;
    while (len--)
    {
        uindex = uchCRCHi^*updata++;
        uchCRCHi = uchCRCLo^auchCRCHi[uindex];
        uchCRCLo = auchCRCLo[uindex];
    }
    return (uchCRCHi << 8 | uchCRCLo);
}

void UartTool::slot_pushButton_OpenBin_clicked()//按钮响应
{
    QString fileName = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("bin file"), qApp->applicationDirPath(),
        QString::fromLocal8Bit("bin File(*.bin)"));//新建文件打开窗口
    if (fileName.isEmpty())//如果未选择文件便确认，即返回
        return;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        ui.textEdit_read->setText(file.errorString());//文件打开错误显示错误信息

    arry = file.readAll();//读取文件
    ui.textEdit_read->append(QString::fromLocal8Bit("打开文件："));
    ui.textEdit_read->append(fileName);
    file.close();
    int length = arry.size();//计算长度
    quint16 tempcrc = N_CRC16((unsigned char*)arry.data(), length, 0xffff);
    qDebug() << "bin size :" << length;
    qDebug() << "crc :" << tempcrc;

    //QFile file2("C:/Users/Administrator/Desktop/test.c");
    //file2.open(QIODevice::WriteOnly);
    ////QTextStream out(&file2);
    ////out << arry;  //文本流,arry会强制转换为text
    //file2.write(arry);
    //file2.close();

    //1E 06 01 00 00 00 00 07 01 sizeof(arry) sizeof(arry)/4096 crc 2E
    UpDataSizeAll = arry.size();
    if (UpDataSizeAll % 900)
    {
        UpDataPackAll = arry.size() / 900 + 1;
    }
    else
    {
        UpDataPackAll = arry.size() / 900;
    }
    
    UpDataPackNow = 1;

    SendData[0] = 0x1E;
    SendData[1] = 0x01;
    SendData[2] = 0x06;
    SendData[3] = 0;
    SendData[4] = 0;
    SendData[5] = 0;
    SendData[6] = 0;
    SendData[7] = 0x07;
    SendData[8] = 0x00;
    SendData[9] = BOOT_OR_APP;
    SendData[10] = (unsigned char)(UpDataSizeAll);
    SendData[11] = (unsigned char)(UpDataSizeAll >> 8);
    SendData[12] = (unsigned char)(UpDataSizeAll >> 16);
    SendData[13] = (unsigned char)(UpDataSizeAll >> 24);
    SendData[14] = (unsigned char)(UpDataPackAll);
    SendData[15] = (unsigned char)(UpDataPackAll >> 8);
    unsigned char* strdata = (unsigned char *)SendData.data();
    SendData[16] = DrvGetCrc8(&strdata[1], 15);
    SendData[17] = 0x2E;
    if (pSerialPort != nullptr)
    {
        pSerialPort->write(SendData);
    }
    SendData.clear();
    
}


void UartTool::slot_pushButton_ClearBin_clicked()
{
    UpDataSizeAll = 0;
    UpDataPackNow = 1;
    UpDataPackAll = 0;
    arry.clear();
    SendData.clear();

    ui.textEdit_read->clear();
}