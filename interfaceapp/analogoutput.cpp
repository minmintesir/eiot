#include "analogoutput.h"

AnalogOutput::AnalogOutput(QObject *parent) : QObject(parent)
{

}

int AnalogOutput::analogoutput1_change(unsigned char status)
{
    int ret = analogoutput_write(1,status);
    return ret;
}

//模拟量輸出控制操作
int AnalogOutput::analogoutput_write(unsigned char channel,unsigned char quantity)
{
    int tmp[2];
    int res;
    tmp[0] = channel;
    tmp[1] = quantity; //0-65536
    res = write(m_fd, tmp, sizeof(tmp));
    if(res < 0){
        printf("iic pcal6416 Control Failed!\r\n");
        close(m_fd);
    }
    return res;
}
//模拟量輸出初始化
int AnalogOutput::analogoutput_init()
{
    int fd = 0;
    fd = ::open(IMX6_ANALOGOUTPUT, O_RDWR|O_NONBLOCK);
        if(fd < 0){

        }
    return fd;
}

