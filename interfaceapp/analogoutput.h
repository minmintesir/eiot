#ifndef ANALOGOUTPUT_H
#define ANALOGOUTPUT_H

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#include <QObject>
#include <QTimer>
#include <QThread>

#define IMX6_ANALOGOUTPUT     "/dev/ad5667r"
//第一路模拟量输出
#define ANALOGOUTPUT_CH1       0
//第二路模拟量输出
#define ANALOGOUTPUT_CH2       1

typedef  enum _ANALOG_OUTPUT_CHANNEL
{
    ANALOG_OUT_CHANNEL_CH1 = 0,
    ANALOG_OUT_CHANNEL_CH2,
    ANALOG_OUT_CHANNEL_MAX
}ANALOG_OUTPUT_CHANNEL;

typedef enum _ANALOG_OUTPUT_TYPE
{
    ANALOG_OUTPUT_TYPE_LOW_GATEVALVE = 0,
    ANALOG_OUTPUT_TYPE_HIGH_GATEVALVE,
    ANALOG_OUTPUT_TYPE_MAX
}ANALOG_OUTPUT_TYPE;

typedef struct _ANALOG_OUTPUT_DISPOSE_STRUCT
{
    ANALOG_OUTPUT_CHANNEL analogoutchannel;
    ANALOG_OUTPUT_TYPE analogoutputtype;
}ANALOG_OUTPUT_DISPOSE_STRUCT;

typedef struct _ANALOG_OUTPUT_SINGALCH_STRUCT
{
    int flag;
    unsigned short  dacvalue;
    unsigned char   angle;
}ANALOG_OUTPUT_SINGALCH_STRUCT;

typedef  struct _ANALOG_OUTPUT_REV_STRUCT
{
    int flag;
    ANALOG_OUTPUT_SINGALCH_STRUCT analogoutputsingalch[2];
}ANALOG_OUTPUT_REV_STRUCT;

class AnalogOutput : public QObject
{
    Q_OBJECT
public:
    explicit AnalogOutput(QObject *parent = nullptr);
    int analogoutput1_change(unsigned char status);
    int analogoutput_run(void);
    int analogoutput_send_from_dac(ANALOG_OUTPUT_REV_STRUCT analogrev);
    int analogoutput_angle_to_analog(ANALOG_OUTPUT_REV_STRUCT analogrev,unsigned short *dacvalue);
    int analogoutput_init();

private:
    int analogoutput_write(unsigned short channel,unsigned short quantity);

    int m_fd;
    int gateminvalue;
    int gatemaxvalue;
    int dacmodel;
    int dacmodeh;
    int dacvall;
    int dacvalh;


signals:

};

#endif // ANALOGOUTPUT_H
