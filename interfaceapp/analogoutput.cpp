#include "analogoutput.h"
#include "mqttservice.h"

AnalogOutput::AnalogOutput(QObject *parent) : QObject(parent)
{
    int fd = 0;
    fd = ::open(IMX6_ANALOGOUTPUT, O_RDWR|O_NONBLOCK);
        if(fd < 0){

        }
    m_fd = fd;
}
//模拟量輸出初始化
int AnalogOutput::analogoutput_init()
{
    int res =0;

    //從配置文件中獲取 DeviceNumber
    IniParse ini(EIOT_CONFIG);
    unsigned short gateminvalue = ini.GetLong("Device.GateMinValue");
    unsigned short gatemaxvalue = ini.GetLong("Device.GateMaxValue");

    unsigned short dacmodel = ini.GetLong("Device.ValueModeHigh");
    unsigned short dacmodeh = ini.GetLong("Device.ValueModeLow");
    if(dacmodel ==1)
    {
        int dacvalue;
        dacvall = ini.GetLong("Device.ValueLowOutput");
        dacvalue =(unsigned short) (((dacvall -gateminvalue)*(65535-13107))/(gatemaxvalue - gateminvalue) + 13107);
        analogoutput_write(0,dacvalue);
    }
    else
    {
        analogoutput_write(0,0);
    }

    if(dacmodeh ==1)
    {
        int dacvalue;
        dacvalh = ini.GetLong("Device.ValueHighOutput");
        dacvalue =(unsigned short) (((dacvall -gateminvalue)*(65535-13107))/(gatemaxvalue - gateminvalue) + 13107);
        analogoutput_write(1,dacvalue);
    }
    else
    {
        analogoutput_write(0,0);
    }

    QString _devicenumber = ini.GetString("Device.DeviceNumber").c_str();

    printf("analogoutput_init:gateminvalue:%d,gatemaxvalue:%d",gateminvalue,gatemaxvalue);
    return res;
}

//模拟量輸出控制操作
int AnalogOutput::analogoutput_write(unsigned short channel,unsigned short quantity)
{
    unsigned short tmp[2];
    int res;
    tmp[0] = channel;
    tmp[1] = quantity; //0-65536
    res = write(m_fd, tmp, sizeof(tmp));
    if(res < 0){
        printf("iic pcal6416 Control Failed!\r\n");
        close(m_fd);
    }
    printf("analogoutput_write:ch:%d,qu:%d \r\n",tmp[0],tmp[1]);
    return res;
}
int AnalogOutput::analogoutput_send_from_dac(ANALOG_OUTPUT_REV_STRUCT analogrev)
{
    int res =0;
    unsigned short dacvalue[2];

    if(analogrev.flag == 1)
    {
        analogoutput_angle_to_analog(analogrev,dacvalue);
        if(analogrev.analogoutputsingalch[0].flag ==1)
        {
            analogoutput_write(0,dacvalue[0]);
            analogrev.analogoutputsingalch[0].flag = 0;
            printf("analogoutput_send_from_dac:0flag :%d 0dacv:%d \r\n",analogrev.analogoutputsingalch[0].flag,dacvalue[0]);
        }

        if(analogrev.analogoutputsingalch[1].flag ==1)
        {
            analogoutput_write(1,dacvalue[1]);
            analogrev.analogoutputsingalch[1].flag = 0;
            printf("analogoutput_send_from_dac:1flag :%d 1dacv:%d \r\n",analogrev.analogoutputsingalch[1].flag,dacvalue[1]);
        }
        analogrev.flag =0;
        clearAnalogDataFlag();
    }
    else
        res = -1;
    return  res;
}

int AnalogOutput::analogoutput_run()
{
    int res =0;
    ANALOG_OUTPUT_REV_STRUCT analogrev;

    analogrev = getAnalogDataFromMqtt();
    res = analogoutput_send_from_dac(analogrev);

    return res;
}

int AnalogOutput::analogoutput1_change(unsigned char status)
{
    int ret = analogoutput_write(1,status);
    return ret;
}

int AnalogOutput::analogoutput_angle_to_analog(ANALOG_OUTPUT_REV_STRUCT analogrev,unsigned short *dacvalue)
{
    int res =0;
    printf("analogoutput_angle_to_analog min:%d max:%d \r\n",gateminvalue,gatemaxvalue);
    if(analogrev.analogoutputsingalch[0].flag ==1)
        {
            analogrev.analogoutputsingalch[0].dacvalue =(unsigned short) (((analogrev.analogoutputsingalch[0].angle -gateminvalue)*(65535-13107))/(gatemaxvalue - gateminvalue) + 13107);
            *dacvalue = analogrev.analogoutputsingalch[0].dacvalue;
            printf("analogoutput_angle_to_analog 0:%d \r\n",analogrev.analogoutputsingalch[0].dacvalue);
        }
    if(analogrev.analogoutputsingalch[1].flag ==1)
        {
            analogrev.analogoutputsingalch[1].dacvalue = (unsigned short)(((analogrev.analogoutputsingalch[1].angle -gateminvalue)*(65535-13107))/(gatemaxvalue - gateminvalue) + 13107);
            *(dacvalue +1) = analogrev.analogoutputsingalch[1].dacvalue;
            printf("analogoutput_angle_to_analog 0:%d \r\n",analogrev.analogoutputsingalch[1].dacvalue);
        }
    return res;
}

