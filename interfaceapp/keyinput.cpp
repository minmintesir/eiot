//此版本先不处理按键输入的问题
#include "keyinput.h"
/*
KeyInput::KeyInput(QObject *parent) : QObject(parent)
{
    //按键輸入初始化,获取文件描述符
    m_fd = keyinput_init();
    if(m_fd < 0){
        ERROR_LOG("Device file open failed");
    }
    //用来监听系统文件操作，将操作转换为Qt事件进入系统的消息循环队列。并调用预先设置的事件接受函数，处理事件
    m_notifier = new QSocketNotifier(m_fd, QSocketNotifier::Read, this);
    connect (m_notifier, SIGNAL(activated(int)), this, SLOT(keyinput_read()));
}

//按键輸入读取
int KeyInput::keyinput_read()
{
    unsigned int res = 0;
    int data[2];

    res = read(m_fd, data, sizeof(data));

    if( (res > 0) && (res <= sizeof(data)) ){
        //data[0]:chanel    data[1]:status
        printf("DigitalInput value = %X gpio val = %x\r\n", data[0],data[1]);
        switch(data[0]){
            case KEYINPUT_CH1:{

            }break;
            case KEYINPUT_CH2:{

            }break;
            default:{

            } break;
        }
    }
    else{
        printf("KeyInput Error. read key error. res=%d.\n", res);
    }
    return res;
}

//按键輸入初始化
int KeyInput::keyinput_init()
{
    return ::open(IMX6_KEYINPUT, O_RDWR|O_NONBLOCK);
}

KeyInput::~KeyInput()
{
    close(m_fd);
}
*/
