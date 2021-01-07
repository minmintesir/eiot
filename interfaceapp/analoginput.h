#ifndef ANALOGINPUT_H
#define ANALOGINPUT_H

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#include <QObject>
#include <QtCore/QDebug>
#include <QTimer>
#include <QThread>
#include <QSocketNotifier>
#include "tools/log.h"
#include "tools/macian_json.h"

#define IMX6_ANALOGINPUT     "/dev/ms1112"
#define QUEUECOUNTS    64
#define ANALOGINPUT 8
#define ANALOGCONTACT 2
#define ANALOGOUTPUT 25

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

typedef enum _GET_ANALOG_INPUT_RUN_STATE{
    GET_ANALOG_INPUT_RUN_STATE_INIT = 0,
    GET_ANALOG_INPUT_RUN_STATE_SEND_CMD,
    GET_ANALOG_INPUT_RUN_STATE_GET_DATA,
    GET_ANALOG_INPUT_RUN_STATE_CREAT_PACKAGE,
    GET_ANALOG_INPUT_RUN_STATE_MAX
}GET_ANALOG_INPUT_RUN_STATE;

typedef  enum _ANALOG_INPUT_CHANNEL
{
    ANALOG_INPUT_CH0 =0,
    ANALOG_INPUT_CH1,
    ANALOG_INPUT_CH2,
    ANALOG_INPUT_CH3,
    ANALOG_INPUT_CH4,
    ANALOG_INPUT_CH5,
    ANALOG_INPUT_CH6,
    ANALOG_INPUT_CH7,
    ANALOG_INPUT_MAX
}ANALOG_INPUT_CHANNEL;

typedef enum _ANALOG_INPUT_CHANNEL_FLAG
{
    ANALOG_INPUT_CHANNEL_FLAG_RESET,
    ANALOG_INPUT_CHANNEL_FLAG_SET,
    ANALOG_INPUT_CHANNEL_FLAG_MAX
}ANALOG_INPUT_CHANNEL_FLAG;

typedef enum _ANALOG_INPUT_TYPE
{
    ANALOG_INPUT_TYPE_LOW_PRESS = 0,
    ANALOG_INPUT_TYPE_HIGH_PRESS,
    ANALOG_INPUT_TYPE_LOW_IN_TEMP,
    ANALOG_INPUT_TYPE_LOW_OUT_TEMP,
    ANALOG_INPUT_TYPE_HIGH_IN_TEMP,
    ANALOG_INPUT_TYPE_HIGH_OUT_TEMP,
    ANALOG_INPUT_TYPE_LOW_GATEVALVE,
    ANALOG_INPUT_TYPE_HIGH_GATEVALVE,
    ANALOG_INPUT_TYPE_MAX
}ANALOG_INPUT_TYPE;

typedef enum _MQTT_PUBLIST_MESSAGE_TOPIC_GROUP
{
    MQTT_PUBLIST_MESSAGE_TOPIC_INLETPRESSLOW = 0,
    MQTT_PUBLIST_MESSAGE_TOPIC_INLETPRESSHIGH,
    MQTT_PUBLIST_MESSAGE_TOPIC_INLETTEMPLOW,
    MQTT_PUBLIST_MESSAGE_TOPIC_OUTLETTEMPLOW,
    MQTT_PUBLIST_MESSAGE_TOPIC_INLETTEMPHIGH,
    MQTT_PUBLIST_MESSAGE_TOPIC_OUTLETTEMPHIGH,
    MQTT_PUBLIST_MESSAGE_TOPIC_TOTALVALVELOW,
    MQTT_PUBLIST_MESSAGE_TOPIC_TOTALVALVEHIGH,
    MQTT_PUBLIST_MESSAGE_TOPIC_MAX
}MQTT_PUBLIST_MESSAGE_TOPIC_GROUP;

typedef struct _ANALOG_INPUT_STRUCT
{
    ANALOG_INPUT_CHANNEL analoginputchannel;
    ANALOG_INPUT_CHANNEL_FLAG analoginputchannelflag;
    ANALOG_INPUT_TYPE analoginputtype;
    MQTT_PUBLIST_MESSAGE_TOPIC_GROUP ainpubmestopic;
}ANALOG_INPUT_STRUCT;

typedef struct _ANALOG_INPUT_DATA_STRUCT
{
    ANALOG_INPUT_TYPE analoginputtype;
    int analogvalue;
}ANALOG_INPUT_DATA_STRUCT;

typedef struct _ANALOG_CH_ANALOG_STRUCT
{
    int whichch;
    int adcvalue;
}ANALOG_CH_ANALOG_STRUCT;

typedef struct _ANALOG_SEND_PACKAGE
{
    int flag;
    ANALOG_CH_ANALOG_STRUCT analogchanalog[ANALOGINPUT];
}ANALOG_SEND_PACKAGE;


class AnalogInput : public QObject
{
    Q_OBJECT
public:
    AnalogInput();
    ~AnalogInput();

    int analoginput_init();
    int analoginput_run();
    int get_analoginput_ch1();

    ANALOG_SEND_PACKAGE analoginput_get_analogdata();

private:
    int analoginputstruct_init();
    int analoginput_write(int channel);
    int analoginput_copytopic(unsigned char whichch);
    int analoginput_read();
    int analoginput_get_valuemode(unsigned int *valuemode);
    int analoginput_get_valueoutput(unsigned int *valueoutput);
    int analoginput_creat_package();
    //文件描述符
    int m_fd;
    int valuemode;
    int valueoutput;
    int currentchannel;
    ANALOG_INPUT_STRUCT  analoginputstrcut[ANALOGINPUT];
    GET_ANALOG_INPUT_RUN_STATE getanaloginputrunstate;
    ANALOG_SEND_PACKAGE analogsendpackage;
    //监听套接字
    QSocketNotifier *m_notifier;
signals:

};

#endif // ANALOGINPUT_H
