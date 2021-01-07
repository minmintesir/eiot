#ifndef KEYINPUT_H
#define KEYINPUT_H

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
/*
#define IMX6_KEYINPUT       "/dev/input/event1"

//
//tv_sec:事件发生的时间 秒
//tv_usec:事件发生的时间 微秒
//dev：需要上报的 input_dev。
//type: 上报的事件类型，比如 EV_KEY。
// *********************重要****************************
//code： 事件码，也就是我们注册的按键值，比如 KEY_0、 KEY_1 等等。
//value：事件值，比如 1 表示按键按下， 0表示按键松开。
//
// *****************input_event 类型********************
// * 编号 *    * tv_sec *      * tv_usec *    * type *    * code *      * value *
//0000000     0c41 0000       d7cd 000c       0001        000b        0001 0000  按下
//0000010     0c41 0000       d7cd 000c       0000        0000        0000 0000
//0000020     0c42 0000       54bb 0000       0001        000b        0000 0000  松开
//0000030     0c42 0000       54bb 0000       0000        0000        0000 0000
//
struct keyinput_timeval {
    long tv_sec;    // 秒
    long tv_usec;   // 微秒
};

struct keyinput_event {
    struct keyinput_timeval time;
    unsigned int type;  // 事件类型
    unsigned int code;  // 事件码
    int value;         // 事件值
};

//Linux中输入设备的事件类型
#define EV_SYN 0x00 //同步事件
#define EV_KEY 0x01 //按键事件,如KEY_VOLU ME DOWN
#define EV_REL 0x02 //相对坐标,如鼠标上报的坐标
#define EV_ABS 0x03 //绝对坐标,如触摸屏上报的坐标

//第一个按键输入
#define KEYINPUT_CH1        0
//第二个按键输入
#define KEYINPUT_CH2        1

class KeyInput : public QObject
{
    Q_OBJECT
public:
    explicit KeyInput(QObject *parent = nullptr);
    ~KeyInput();

protected:

private:
    int keyinput_init();
    int keyinput_read();
    //文件描述符
    int m_fd;
    //监听套接字
    QSocketNotifier *m_notifier;
signals:

};
*/
#endif // KEYINPUT_H
