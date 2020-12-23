/******************************************************************************************
*-----First Created. time: 16-November-2020	4:35  P.m.
* @File name    :main.cpp
* @Platform     :imx6ull * openwrt19.07.2 * linux 4.1.15 * Qt5.5.1 * arm-openwrt-linux-gnueabi-
* @Author       :MacianYuan
* @E-mail       :macian@foxmail.com
* @Project      :边缘网关控制器（楼宇供暖控制系统）
*******************************************************************************************
* @Description  :
* @Attention    :
* @Log          :
*****************16-November-2020(MacianYuan):
*****************
* @Copyright© @MacianYuan Anhui Xinjie Technology Co.,Ltd.  2010-2020  All rights reserved.
******************************************************************************************/

#include <QtCore/QByteArray>
#include <QtCore/QTime>
#include <QtSql/QtSql>
#include "common.h"

// int转bytes，低字节在前
QByteArray intToBytes(int num, int size) {
    QByteArray byteArray;

    if (size <= 0) {
        size = sizeof(num);
    }

    for (int i = 0; i < size; i++) {
        byteArray.append((uchar) (num >> (i * 8) & 0xff));
    }

    return byteArray;
}

// bytes转int，低字节在前
int bytesToInt(QByteArray byteArray, int size) {
    if (size <= 0)
        return 0;

    if (size > byteArray.size())
        size = byteArray.size();

    int num = 0;
    for (int i = 0; i < size; i++)
        num |= (byteArray[i] << (i * 8));

    return num;
}

// int转bytes, 高字节在前
QByteArray intToBytes2(int num, int size) {
    QByteArray byteArray;

    if (size <= 0)
        size = sizeof(num);

    for (int i = size - 1; i >= 0; i--)
        byteArray.append((uchar) (num >> (i * 8) & 0xff));

    return byteArray;
}

// bytes转int，高字节在前
int bytesToInt2(QByteArray byteArray, int size) {
    if (size <= 0)
        return 0;
    if (size > byteArray.size())
        size = byteArray.size();

    int num = 0;
    for (int i = 0; i < size; i++)
        num |= (byteArray[size - 1 - i] << (i * 8));

    return num;
}

void msleep(uint msecs) {
    QTime dieTime = QTime::currentTime().addMSecs(msecs);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

char ConvertHexChar(char ch) {
    if ((ch >= '0') && (ch <= '9')) {
        return ch - 0x30;
    } else if ((ch >= 'A') && (ch <= 'F')) {
        return ch - 'A' + 10;
    } else if ((ch >= 'a') && (ch <= 'f')) {
        return ch - 'a' + 10;
    }
    return (-1);
}


unsigned int crc16table(QByteArray bytearray_buf)
{
    unsigned char ucCRCHi = 0xFF;
    unsigned char ucCRCLo = 0xFF;
    unsigned int usIndex;
    //unsigned int len = bytearray_buf.size();

    for( int i = 0;i < bytearray_buf.size();i++){
        usIndex = ucCRCHi ^ bytearray_buf.at(i);
        ucCRCHi = ucCRCLo ^ auchCRCHi[usIndex];
        ucCRCLo = auchCRCLo[usIndex];
    }
    return ((unsigned int)ucCRCHi << 8 | ucCRCLo);
}
