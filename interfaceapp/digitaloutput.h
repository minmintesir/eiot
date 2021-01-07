#ifndef DIGITALOUTPUT_H
#define DIGITALOUTPUT_H

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#include <QObject>
#include <QTimer>
#include <QThread>

#define IMX6_DIGITALOUTPUT     "/dev/PCAL6416A"
#define OUTPUTCOUNTS 9

#define DIGITALOUTPUT_ON       1
#define DIGITALOUTPUT_OFF      0
//PCAL6416A 硬件配置  IIC2 地址 0x20
//P0_0   12V第一路控制
#define EXITPOWER1_12_GROUP    0
#define EXITPOWER1_12_PIN      0
//P0_1   12V第二路控制
#define EXITPOWER2_12_GROUP    0
#define EXITPOWER2_12_PIN      1
//P0_2   12V第三路控制
#define EXITPOWER3_12_GROUP    0
#define EXITPOWER3_12_PIN      2
//P0_3   5V控制
#define EXITPOWER4_5V_GROUP    0
#define EXITPOWER4_5V_PIN      3
//P0_4   继电器第一路控制
#define RELAYCTR1_IO_GROUP     0
#define RELAYCTR1_IO_PIN       4
//P0_5   继电器第二路控制
#define RELAYCTR2_IO_GROUP     0
#define RELAYCTR2_IO_PIN       5
//P0_6   继电器第三路控制
#define RELAYCTR3_IO_GROUP     0
#define RELAYCTR3_IO_PIN       6
//P0_7   继电器第四路控制
#define RELAYCTR4_IO_GROUP     0
#define RELAYCTR4_IO_PIN       7

//P1_7   9V控制
#define EXITPOWER5_9V_GROUP    1
#define EXITPOWER5_9V_PIN      7
//P1_6   wifi电源控制
#define WIFIPOWER_GROUP        1
#define WIFIPOWER_PIN          6
//     o  o
//     o  o
//     o  o
//P1_5   LED3控制
#define LED3_GROUP             1
#define LED3_PIN               5
//P1_4   LED2控制
#define LED2_GROUP             1
#define LED2_PIN               4
//P1_3   LED1控制
#define LED1_GROUP             1
#define LED1_PIN               3
//P1_2   LED6控制
#define LED6_GROUP             1
#define LED6_PIN               2
//P1_1   LED5控制
#define LED5_GROUP             1
#define LED5_PIN               1
//P1_0   LED4控制   未使用
#define LED4_GROUP             1
#define LED4_PIN               0
//
#define SET_SYS_RUN_LED               LED1_change(1)
#define CLR_SYS_RUN_LED               LED1_change(0)
#define SET_4G_WLAN_MODULE_RUN_LED    LED2_change(1)
#define CLR_4G_WLAN_MODULE_RUN_LED    LED2_change(0)
#define SET_WIFI_MODULE_RUN_LED       LED3_change(1)
#define CLR_WIFI_MODULE_RUN_LED       LED3_change(0)
#define SET_MQTT_MODULE_RUN_LED       LED6_change(1)
#define CLR_MQTT_MODULE_RUN_LED       LED6_change(0)

typedef  enum _DIGITAL_OUTPUT_GROUP
{
    DIGITAL_OUTPUT_GROUP_PCALPORT0 = 0,
    DIGITAL_OUTPUT_GROUP_PCALPORT1,
    DIGITAL_OUTPUT_GROUP_PCALPORTMAX
}DIGITAL_OUTPUT_GROUP;

typedef  enum _DIGITAL_OUTPUT_PIN
{
    DIGITAL_OUTPUT_PIN_PCALPIN0 = 0,
    DIGITAL_OUTPUT_PIN_PCALPIN1,
    DIGITAL_OUTPUT_PIN_PCALPIN2,
    DIGITAL_OUTPUT_PIN_PCALPIN3,
    DIGITAL_OUTPUT_PIN_PCALPIN4,
    DIGITAL_OUTPUT_PIN_PCALPIN5,
    DIGITAL_OUTPUT_PIN_PCALPIN6,
    DIGITAL_OUTPUT_PIN_PCALPIN7,
    DIGITAL_OUTPUT_PIN_MAX
}DIGITAL_OUTPUT_PIN;

typedef enum _DIGITAL_OUTPUT_PIN_STATE
{
    DIGITAL_OUTPUT_PIN_STATE_CLR = 0,
    DIGITAL_OUTPUT_PIN_STATE_SET,
    DIGITAL_OUTPUT_PIN_STATE_MAX
}DIGITAL_OUTPUT_PIN_STATE;

typedef struct _DIGITAL_OUTPUT_STATE_STRUCT
{
    DIGITAL_OUTPUT_GROUP        outputgroup;
    DIGITAL_OUTPUT_PIN          outputpin;
    DIGITAL_OUTPUT_PIN_STATE    outputstate;
}DIGITAL_OUTPUT_STATE_STRUCT;

typedef  struct _DIGITAL_IOCH_OUTPUT_STATE_STRUCT
{
    int iochflag;
    QString topicname;
    DIGITAL_OUTPUT_PIN_STATE outputstate;
}DIGITAL_IOCH_OUTPUT_STATE_STRUCT;

typedef  struct _DIGITAL_SEND_OUTPUT_STATE_STRUCT
{
    int flag;
    DIGITAL_IOCH_OUTPUT_STATE_STRUCT digitaloutputstate[OUTPUTCOUNTS];
}DIGITAL_SEND_OUTPUT_STATE_STRUCT;

class DigitalOutput : public QObject
{
     Q_OBJECT
public:
    explicit DigitalOutput(QObject *parent = nullptr);
    int digitaloutput_init();
    int digitaloutput1_ep12_change(int status);
    int digitaloutput2_ep12_change(int status);
    int digitaloutput3_ep12_change(int status);
    int digitaloutput4_ep5v_change(int status);
    int digitaloutput5_ep9v_change(int status);
    int relayctr1_IO_change(int status);
    int relayctr2_IO_change(int status);
    int relayctr3_IO_change(int status);
    int relayctr4_IO_change(int status);
    int wifipower_change(int status);
    int LED1_change(int status);
    int LED2_change(int status);
    int LED3_change(int status);
    int LED6_change(int status);
    int io_set_status();
    void io_get_status(void);
    DIGITAL_SEND_OUTPUT_STATE_STRUCT digitalsendoutstate;
private:
   //初s始化
   //int digitaloutput_init();
   //文件描述符
   int m_fd;
   unsigned short  curiostatus;
   unsigned short  preiostatus;
   DIGITAL_OUTPUT_STATE_STRUCT digitalstate[OUTPUTCOUNTS];
   int digitaloutput_write(unsigned char group, unsigned char pin,unsigned char status);
signals:

};

#endif // DIGITALOUTPUT_H
