#include "digitaloutput.h"
/******************************************************************************************
* @Function         :main(int argc, char *argv[])
*
* @Purpose          :主函数
* @Description      :
*********************设置支持中文显示，设置字体
*********************设置程序的工作目录
*********************
* @Grammar&Demand   :
*********************<QTextCodec>
*********************<QDir>
* @Param            :
* @Retval           :
* @Others           :
* @e.g.             :
******************************************************************************************/
DigitalOutput::DigitalOutput(QObject *parent) : QObject(parent)
{

}

/*电源控制*/
int DigitalOutput::digitaloutput1_ep12_change(int status){
    return digitaloutput_write(EXITPOWER1_12_GROUP,EXITPOWER1_12_PIN,status);
}
int DigitalOutput::digitaloutput2_ep12_change(int status){
    return digitaloutput_write(EXITPOWER2_12_GROUP,EXITPOWER2_12_PIN,status);
}
int DigitalOutput::digitaloutput3_ep12_change(int status){
    return digitaloutput_write(EXITPOWER3_12_GROUP,EXITPOWER3_12_PIN,status);
}
int DigitalOutput::digitaloutput4_ep5v_change(int status){
    return digitaloutput_write(EXITPOWER4_5V_GROUP,EXITPOWER4_5V_PIN,status);
}
int DigitalOutput::digitaloutput5_ep9v_change(int status){
    return digitaloutput_write(EXITPOWER5_9V_GROUP,EXITPOWER5_9V_PIN,status);
}

/*继电器控制*/
int DigitalOutput::relayctr1_IO_change(int status){
    return digitaloutput_write(RELAYCTR1_IO_GROUP,RELAYCTR1_IO_PIN,status);
}
int DigitalOutput::relayctr2_IO_change(int status){
    return digitaloutput_write(RELAYCTR2_IO_GROUP,RELAYCTR2_IO_PIN,status);
}
int DigitalOutput::relayctr3_IO_change(int status){
    return digitaloutput_write(RELAYCTR3_IO_GROUP,RELAYCTR3_IO_PIN,status);
}
int DigitalOutput::relayctr4_IO_change(int status){
    return digitaloutput_write(RELAYCTR4_IO_GROUP,RELAYCTR4_IO_PIN,status);
}

/*wifi电源控制*/
int DigitalOutput::wifipower_change(int status){
    return digitaloutput_write(WIFIPOWER_GROUP,WIFIPOWER_PIN,status);
}

/*LED控制*/
int DigitalOutput::LED1_change(int status){
    return digitaloutput_write(LED1_GROUP,LED1_PIN,status);
}
int DigitalOutput::LED2_change(int status){
    return digitaloutput_write(LED2_GROUP,LED2_PIN,status);
}
int DigitalOutput::LED3_change(int status){
    return digitaloutput_write(LED3_GROUP,LED3_PIN,status);
}
int DigitalOutput::LED6_change(int status){
    return digitaloutput_write(LED6_GROUP,LED6_PIN,status);
}

//開關量輸出控制操作
int DigitalOutput::digitaloutput_write(unsigned char group, unsigned char pin,unsigned char status)
{
    int tmp[3];
    int res;
    tmp[0] = group;
    tmp[1] = pin;
    tmp[2] = status;
    res = write(m_fd, tmp, 3);
    if(res < 0){
        printf("iic pcal6416 Control Failed!\r\n");
        close(m_fd);
    }
    return res;
}
//開關量輸出初始化
int DigitalOutput::digitaloutput_init()
{
    int fd = 0;
    fd = ::open(IMX6_DIGITALOUTPUT, O_RDWR|O_NONBLOCK);
        if(fd < 0){

        }
    return fd;
}


