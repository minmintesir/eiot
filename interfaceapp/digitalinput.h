#ifndef DIGITALINPUT_H
#define DIGITALINPUT_H

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
/*
#define IMX6_DIGITALINPUT       "/dev/imx6uirq"

//第一路开关量输入
#define DIGITALINPUT_CH1        0
//第二路开关量输入
#define DIGITALINPUT_CH2        1
//第三路开关量输入
#define DIGITALINPUT_CH3        2
//第四路开关量输入
#define DIGITALINPUT_CH4        3

class DigitalInput : public QObject
{
    Q_OBJECT
public:
    explicit DigitalInput(QObject *parent = nullptr);
    ~DigitalInput();

protected:
    void run();

private:
    int digitalinput_init();
    int digitalinput_read();
    //文件描述符
    int m_fd;
    //监听套接字
    QSocketNotifier *m_notifier;
signals:

};
*/
#endif // DIGITALINPUT_H
