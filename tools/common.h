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

#ifndef COMMON_H
#define COMMON_H

#include <QByteArray>

// int转bytes，低字节在前，bytes长度为size
QByteArray intToBytes(int num, int size = 4);
// bytes转int，低字节在前，bytes长度为size
int bytesToInt(QByteArray byteArray, int size = 4);
// int转bytes，高字节在前，bytes长度为size
QByteArray intToBytes2(int num, int size = 4);
// bytes转int，高字节在前，bytes长度为size
int bytesToInt2(QByteArray byteArray, int size = 4);
// 毫秒级延时
void msleep(uint msecs);
// 16禁止转换成数字
char ConvertHexChar(char ch);

#endif // COMMON_H
