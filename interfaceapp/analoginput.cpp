#include "analoginput.h"

AnalogInput::AnalogInput(QObject *parent) : QObject(parent)
{
    //模拟量輸入初始化,获取文件描述符
    m_fd = analoginput_init();
    if(m_fd < 0){
        ERROR_LOG("Device file open failed");
    }
    //用来监听系统文件操作，将操作转换为Qt事件进入系统的消息循环队列。并调用预先设置的事件接受函数，处理事件
    m_notifier = new QSocketNotifier(m_fd, QSocketNotifier::Read, this);
    connect (m_notifier, SIGNAL(activated(int)), this, SLOT(analoginput_read()));
}

//模拟量輸入读取
int AnalogInput::analoginput_read()
{
    unsigned int res = 0;
    int data[2];

    res = read(m_fd, data, sizeof(data));
    if( (res > 0) && (res <= sizeof(data)) ){
        //data[0]:chanel    data[1]:status
        printf("analoginput value = %X gpio val = %x\r\n", data[0],data[1]);
        switch(data[0]){
            case ANALOGINPUT_CH1:{

            }break;
            case ANALOGINPUT_CH2:{

            }break;
            case ANALOGINPUT_CH3:{

            }break;
            case ANALOGINPUT_CH4:{

            }break;
            case ANALOGINPUT_CH5:{

            }break;
            case ANALOGINPUT_CH6:{

            }break;
            case ANALOGINPUT_CH7:{

            }break;
            case ANALOGINPUT_CH8:{

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

int AnalogInput::get_analoginput_ch1()
{
     return analoginput_write(ANALOGINPUT_CH1);
}

//模拟量輸入控制操作
int AnalogInput::analoginput_write(int channel)
{
    int tmp[1];
    int res;
    tmp[0] = channel;

    res = write(m_fd, tmp, sizeof(tmp));
    if(res < 0){

        close(m_fd);
    }
    return res;
}
//模拟量輸入初始化
int AnalogInput::analoginput_init()
{
    int fd = 0;
    fd = ::open(IMX6_ANALOGINPUT, O_RDWR|O_NONBLOCK);
        if(fd < 0){

        }
    return fd;
}
AnalogInput::~AnalogInput()
{
    close(m_fd);
}
