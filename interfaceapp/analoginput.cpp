#include "analoginput.h"
#include <QtMqtt/qmqttclient.h>
#include "mqttservice.h"
#include "mainservice.h"
#include "globaldata.h"
#include <string.h>


AnalogInput::~AnalogInput()
{
    int fd;
    close(m_fd);
    qDebug()<<"AnalogInput, file closed!\r\n";
}

int AnalogInput::analoginputstruct_init()
{
    int res;

    return res;
}
//模拟量輸入初始化
int AnalogInput::analoginput_init()
{
    int res =0;
    //MqttService new mqttservice;
    getanaloginputrunstate = GET_ANALOG_INPUT_RUN_STATE_INIT;
    analogsendpackage.flag =0;
    analoginputstruct_init();
    //用来监听系统文件操作，将操作转换为Qt事件进入系统的消息循环队列。并调用预先设置的事件接受函数，处理事件
    //m_notifier = new QSocketNotifier(m_fd, QSocketNotifier::Read, this);
    //connect (m_notifier, SIGNAL(activated(int)), this, SLOT(analoginput_read()));

    return res;
}
int AnalogInput::analoginput_get_valuemode(unsigned int *valuemode)
{
    int res = 0;
        *valuemode = 1;
    return res;
}

int AnalogInput::analoginput_get_valueoutput(unsigned int *valueoutput)
{
    int res = 0;
        *valueoutput =0;
    return res;
}

int AnalogInput::analoginput_creat_package()
{
    int res = 0;
    unsigned int valueoutput;


    return res;
}

//AnalogInput::AnalogInput(QObject *parent) : QObject(parent)

AnalogInput::AnalogInput()
{
    int fd;

    fd = ::open(IMX6_ANALOGINPUT, O_RDWR|O_NONBLOCK);
    if(fd < 0){
        qDebug()<<"AnalogInput,open  file fail!\r\n";
    }

    m_fd = fd;

   qDebug()<<"AnalogInput: start!\r\n";
}

int AnalogInput::analoginput_run()
{
    static int whichadcch = 0;
    int res = 0;

    GET_ANALOG_INPUT_RUN_STATE state = getanaloginputrunstate;
    printf("AnalogInput: the state change ,is %d \r\n",state);
    switch (state) {
        case GET_ANALOG_INPUT_RUN_STATE_INIT:
            {
                //模拟量輸入初始化,获取文件描述符
                /*
                m_fd = analoginput_init();
                if(m_fd < 0){
                    ERROR_LOG("Device file open failed");
                }
                else
                {
                    printf("analoginput_run: %d\r\n",m_fd);
                }
                */
                getanaloginputrunstate = GET_ANALOG_INPUT_RUN_STATE_GET_DATA;


                break;
            }

    case GET_ANALOG_INPUT_RUN_STATE_SEND_CMD:
            {
                if(analogsendpackage.flag == 0)
                {
                    res = analoginput_write(whichadcch);
                    if(res<0)
                        {
                            printf("AnalogInput: write fail\r\n");
                        }
                    else
                        {
                            //getanaloginputrunstate = GET_ANALOG_INPUT_RUN_STATE_GET_DATA;
                            //printf("AnalogInput: getanaloginputrunstate = GET_ANALOG_INPUT_RUN_STATE_GET_DATA\r\n");
                        }
                }
                else
                {
                    res = -1;
                    printf("AnalogInput: analogsendpackage.flag fail\r\n");
                }

                break;
            }

    case GET_ANALOG_INPUT_RUN_STATE_GET_DATA:
            {

                //res = get_analoginput_ch1();
                res = analoginput_read();

                if(res>0)
                    {
                        //getanaloginputrunstate = GET_ANALOG_INPUT_RUN_STATE_CREAT_PACKAGE;
                       // printf("AnalogInput: getanaloginputrunstate = GET_ANALOG_INPUT_RUN_STATE_GET_DATA\r\n");
                    }
                else
                    {
                        //printf("AnalogInput: getanaloginputrunstate = GET_ANALOG_INPUT_RUN_STATE_GET_DATA no data\r\n");
                    }
                break;
            }

    case GET_ANALOG_INPUT_RUN_STATE_CREAT_PACKAGE:
            {
                analoginput_creat_package();

                whichadcch++;
                if(whichadcch == 8)
                    {
                        whichadcch =0;
                    }

                if(res == 0)
                    {
                        getanaloginputrunstate = GET_ANALOG_INPUT_RUN_STATE_SEND_CMD;

                    }

                break;
            }

    default:
            {
                ERROR_LOG("The state is GET_ANALOG_INPUT_RUN_STATE_MAX , cannt process the palce\r\n");
                break;
            }

    }

    return res;
}

//模拟量輸入读取
int AnalogInput::analoginput_read()
{
    int res =0;
    int data[16]={0};

    if(analogsendpackage.flag == 0)
        {
            res = ::read(m_fd, data, sizeof(data));
            printf("AnalogInput::analoginput_read   res=%d m_fd=%d \r\n",res,m_fd);
            if(res == 0){
                //data[i]:chanel    data[i+1]:status
                printf("analoginput: whichadcch = %d value = %d whichadcch = %d value = %d\r\n", data[0],data[1], data[2],data[3]);
                printf("analoginput: whichadcch = %d value = %d whichadcch = %d value = %d\r\n", data[4],data[5], data[6],data[7]);
                printf("analoginput: whichadcch = %d value = %d whichadcch = %d value = %d\r\n", data[8],data[9], data[10],data[11]);
                printf("analoginput: whichadcch = %d value = %d whichadcch = %d value = %d\r\n", data[12],data[13], data[14],data[15]);

                analogsendpackage.analogchanalog[0].whichch = data[0];
                analogsendpackage.analogchanalog[0].adcvalue = data[1];

                analogsendpackage.analogchanalog[1].whichch = data[2];
                analogsendpackage.analogchanalog[1].adcvalue = data[3];

                analogsendpackage.analogchanalog[2].whichch = data[4];
                analogsendpackage.analogchanalog[2].adcvalue = data[5];

                analogsendpackage.analogchanalog[3].whichch = data[6];
                analogsendpackage.analogchanalog[3].adcvalue = data[7];

                analogsendpackage.analogchanalog[4].whichch = data[8];
                analogsendpackage.analogchanalog[4].adcvalue = data[9];

                analogsendpackage.analogchanalog[5].whichch = data[10];
                analogsendpackage.analogchanalog[5].adcvalue = data[11];

                analogsendpackage.analogchanalog[6].whichch =data[12];
                analogsendpackage.analogchanalog[6].adcvalue = data[13];

                analogsendpackage.analogchanalog[7].whichch = data[14];
                analogsendpackage.analogchanalog[7].adcvalue = data[15];

                analogsendpackage.flag = 1;
            }
        }
    else{
        res = -1;
    }
    return res;
}

int AnalogInput::get_analoginput_ch1()
{
    int res = 0;

     return res;
}

ANALOG_SEND_PACKAGE  AnalogInput::analoginput_get_analogdata()
{
    ANALOG_SEND_PACKAGE analogSendPackage;

    analogSendPackage = analogsendpackage;

    analogsendpackage.flag=0;

    return analogSendPackage;
}
//模拟量輸入控制操作
int AnalogInput::analoginput_write(int channel)
{
    int tmp[1];
    int res;
    tmp[0] = channel;

    res = ::write(m_fd, tmp, sizeof(tmp));
    printf("analoginput_write: %d \r\n",res);
    if(res < 0){
        close(m_fd);
    }
    return res;
}

