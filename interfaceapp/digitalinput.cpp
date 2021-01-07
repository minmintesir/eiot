//此版本先不处理开关量输入的问题
#include "digitalinput.h"
/*
DigitalInput::DigitalInput(QObject *parent) : QObject(parent)
{
    //開關量輸入初始化,获取文件描述符
    m_fd = digitalinput_init();
    if(m_fd < 0){
        printf(" QT open IMX6_DIGITALINPUT failed");
    }
    //用来监听系统文件操作，将操作转换为Qt事件进入系统的消息循环队列。并调用预先设置的事件接受函数，处理事件
    m_notifier = new QSocketNotifier(m_fd, QSocketNotifier::Read, this);
    connect (m_notifier, SIGNAL(activated(int)), this, SLOT(digitalinput_read()));
}

//開關量輸入读取
int DigitalInput::digitalinput_read()
{
    unsigned int res = 0;
    int data[2];

    res = read(m_fd, data, sizeof(data));
    if( (res > 0) && (res <= sizeof(data)) ){
        //data[0]:chanel    data[1]:status
        printf("DigitalInput value = %X gpio val = %x\r\n", data[0],data[1]);
        switch(data[0]){
            case DIGITALINPUT_CH1:{

            }break;
            case DIGITALINPUT_CH2:{

            }break;
            case DIGITALINPUT_CH3:{

            }break;
            case DIGITALINPUT_CH4:{

            }break;
            default:{

            } break;
        }
    }
    else{
        printf("DigitalInput Error. read DO device error. res=%d.\n", res);
    }
    return res;
}

//開關量輸入初始化
int DigitalInput::digitalinput_init()
{
    int fd = 0;
    fd = ::open(IMX6_DIGITALINPUT, O_RDWR|O_NONBLOCK);
        if(fd < 0){
            //log 日志
        }
    return fd;
}

DigitalInput::~DigitalInput()
{
    close(m_fd);
}
*/
