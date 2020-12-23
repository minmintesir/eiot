#ifndef ANALOGINPUT_H
#define ANALOGINPUT_H

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#include <QObject>
#include <QTimer>
#include <QThread>
#include <QSocketNotifier>
#include "tools/log.h"

#define IMX6_ANALOGINPUT     "/dev/ms1112"

//第一路模拟量输入
#define ANALOGINPUT_CH1       0
//第二路模拟量输入
#define ANALOGINPUT_CH2       1
//第三路模拟量输入
#define ANALOGINPUT_CH3       2
//第四路模拟量输入
#define ANALOGINPUT_CH4       3
//第五路模拟量输入
#define ANALOGINPUT_CH5       4
//第六路模拟量输入
#define ANALOGINPUT_CH6       5
//第七路模拟量输入
#define ANALOGINPUT_CH7       6
//第八路模拟量输入
#define ANALOGINPUT_CH8       7


class AnalogInput : public QObject
{
    Q_OBJECT
public:
    explicit AnalogInput(QObject *parent = nullptr);
    int get_analoginput_ch1();
    ~AnalogInput();

private:
    int analoginput_init();
    int analoginput_write(int channel);
    int analoginput_read();
    //文件描述符
    int m_fd;
    //监听套接字
    QSocketNotifier *m_notifier;
signals:

};

#endif // ANALOGINPUT_H
