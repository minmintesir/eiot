#include "digitaloutput.h"
#include "globaldata.h"
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
    int fd = 0;
    fd = ::open(IMX6_DIGITALOUTPUT, O_RDWR|O_NONBLOCK);
        if(fd < 0){
            printf("DigitalOutput : open IMX6_DIGITALOUTPUT file fail!\r\n");
        }
    m_fd = fd;

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
void DigitalOutput::io_get_status(void)
{
    int i = 0;
    int temp[9] ;
    IniParse ini(EIOT_CONFIG);
    temp[0] = ini.GetLong("ExitPower.Exit1Power12");
    temp[1] = ini.GetLong("ExitPower.Exit2Power12");
    temp[2] = ini.GetLong("ExitPower.Exit3Power12");
    temp[3] = ini.GetLong("ExitPower.Exit4Power5");
    temp[4] = ini.GetLong("ExitPower.Exit5Power9");
    temp[5] = ini.GetLong("OutputRelay.Output1Relay");
    temp[6] = ini.GetLong("OutputRelay.Output1Relay");
    temp[7] = ini.GetLong("OutputRelay.Output1Relay");
    temp[8] = ini.GetLong("OutputRelay.Output1Relay");

    for(;i<OUTPUTCOUNTS;i++)
        {
            switch (i) {
                case 0:
                    {
                        digitalstate[i].outputpin = DIGITAL_OUTPUT_PIN_PCALPIN0;
                        digitalstate[i].outputgroup = DIGITAL_OUTPUT_GROUP_PCALPORT0;
                        if( temp[i] == 0)
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_CLR;
                        else
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_SET;
                    }

                case 1:
                    {
                        digitalstate[i].outputpin = DIGITAL_OUTPUT_PIN_PCALPIN1;
                        digitalstate[i].outputgroup = DIGITAL_OUTPUT_GROUP_PCALPORT0;
                        if( temp[i] == 0)
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_CLR;
                        else
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_SET;
                    }

                case 2:
                    {
                        digitalstate[i].outputpin = DIGITAL_OUTPUT_PIN_PCALPIN2;
                        digitalstate[i].outputgroup = DIGITAL_OUTPUT_GROUP_PCALPORT0;
                        if( temp[i] == 0)
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_CLR;
                        else
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_SET;
                    }

                case 3:
                    {
                        digitalstate[i].outputpin = DIGITAL_OUTPUT_PIN_PCALPIN3;
                        digitalstate[i].outputgroup = DIGITAL_OUTPUT_GROUP_PCALPORT0;
                        if( temp[i] == 0)
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_CLR;
                        else
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_SET;
                    }

                case 4:
                    {
                        digitalstate[i].outputpin = DIGITAL_OUTPUT_PIN_PCALPIN4;
                        digitalstate[i].outputgroup = DIGITAL_OUTPUT_GROUP_PCALPORT0;
                        if( temp[i] == 0)
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_CLR;
                        else
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_SET;
                    }

                case 5:
                    {
                        digitalstate[i].outputpin = DIGITAL_OUTPUT_PIN_PCALPIN5;
                        digitalstate[i].outputgroup = DIGITAL_OUTPUT_GROUP_PCALPORT0;
                        if( temp[i] == 0)
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_CLR;
                        else
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_SET;
                    }

                case 6:
                    {
                        digitalstate[i].outputpin = DIGITAL_OUTPUT_PIN_PCALPIN6;
                        digitalstate[i].outputgroup = DIGITAL_OUTPUT_GROUP_PCALPORT0;
                        if( temp[i] == 0)
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_CLR;
                        else
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_SET;
                    }

                case 7:
                    {
                        digitalstate[i].outputpin = DIGITAL_OUTPUT_PIN_PCALPIN7;
                        digitalstate[i].outputgroup = DIGITAL_OUTPUT_GROUP_PCALPORT0;
                        if( temp[i] == 0)
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_CLR;
                        else
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_SET;

                    }
                    break;
                case 8:
                    {
                        digitalstate[i].outputpin = DIGITAL_OUTPUT_PIN_PCALPIN7;
                        digitalstate[i].outputgroup = DIGITAL_OUTPUT_GROUP_PCALPORT1;
                        if( temp[i] == 0)
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_CLR;
                        else
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_SET;
                    }
                    break;
                }

            if(digitalstate[i].outputstate == DIGITAL_OUTPUT_PIN_STATE_SET)
                curiostatus |= 1<<i;
        }

}

int DigitalOutput::io_set_status()
{
    int res = 0;

    unsigned char count = 0;
    unsigned short varcuriostatus,varpreiostatus;

    io_get_status();
    varcuriostatus = curiostatus & 0x0001;
    varpreiostatus = preiostatus & 0x0001;
    for(;count < 9;count++)
    {
        varcuriostatus = varcuriostatus & 0x0001;
        varpreiostatus = varpreiostatus & 0x0001;
        if(varcuriostatus != varpreiostatus )
            {
                if(varcuriostatus == 0)
                   digitaloutput_write(digitalstate[count].outputgroup, digitalstate[count].outputpin,digitalstate[count].outputstate);
                else
                   digitaloutput_write(digitalstate[count].outputgroup, digitalstate[count].outputpin,digitalstate[count].outputstate);
            }
        varcuriostatus = (curiostatus>>count) & 0x0001;
        varpreiostatus = (preiostatus>>count) & 0x0001;
    }
    preiostatus = curiostatus;

    return res;
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
    int res = 0;
    int i = 0;
    int temp[9] ;
    IniParse ini(EIOT_CONFIG);
    temp[0] = ini.GetLong("ExitPower.Exit1Power12");
    temp[1] = ini.GetLong("ExitPower.Exit2Power12");
    temp[2] = ini.GetLong("ExitPower.Exit3Power12");
    temp[3] = ini.GetLong("ExitPower.Exit4Power5");
    temp[4] = ini.GetLong("ExitPower.Exit5Power9");
    temp[5] = ini.GetLong("OutputRelay.Output1Relay");
    temp[6] = ini.GetLong("OutputRelay.Output2Relay");
    temp[7] = ini.GetLong("OutputRelay.Output3Relay");
    temp[8] = ini.GetLong("OutputRelay.Output4Relay");

    curiostatus = 0;
    preiostatus = 0;
    for(;i<OUTPUTCOUNTS;i++)
        {
            switch (i) {
                case 0:
                    {
                        digitalstate[i].outputpin = DIGITAL_OUTPUT_PIN_PCALPIN0;
                        digitalstate[i].outputgroup = DIGITAL_OUTPUT_GROUP_PCALPORT0;
                        if( temp[i] == 0)
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_CLR;
                        else
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_SET;

                        digitalsendoutstate.digitaloutputstate[i].iochflag = 1;
                        digitalsendoutstate.digitaloutputstate[i].topicname = "Exit1Power12";
                        digitalsendoutstate.digitaloutputstate[i].outputstate = digitalstate[i].outputstate;
                    }

                case 1:
                    {
                        digitalstate[i].outputpin = DIGITAL_OUTPUT_PIN_PCALPIN1;
                        digitalstate[i].outputgroup = DIGITAL_OUTPUT_GROUP_PCALPORT0;
                        if( temp[i] == 0)
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_CLR;
                        else
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_SET;

                        digitalsendoutstate.digitaloutputstate[i].iochflag = 1;
                        digitalsendoutstate.digitaloutputstate[i].topicname = "Exit2Power12";
                        digitalsendoutstate.digitaloutputstate[i].outputstate = digitalstate[i].outputstate;
                    }

                case 2:
                    {
                        digitalstate[i].outputpin = DIGITAL_OUTPUT_PIN_PCALPIN2;
                        digitalstate[i].outputgroup = DIGITAL_OUTPUT_GROUP_PCALPORT0;
                        if( temp[i] == 0)
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_CLR;
                        else
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_SET;

                        digitalsendoutstate.digitaloutputstate[i].iochflag = 1;
                        digitalsendoutstate.digitaloutputstate[i].topicname = "Exit3Power12";
                        digitalsendoutstate.digitaloutputstate[i].outputstate = digitalstate[i].outputstate;
                    }

                case 3:
                    {
                        digitalstate[i].outputpin = DIGITAL_OUTPUT_PIN_PCALPIN3;
                        digitalstate[i].outputgroup = DIGITAL_OUTPUT_GROUP_PCALPORT0;
                        if( temp[i] == 0)
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_CLR;
                        else
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_SET;

                        digitalsendoutstate.digitaloutputstate[i].iochflag = 1;
                        digitalsendoutstate.digitaloutputstate[i].topicname = "Exit4Power5";
                        digitalsendoutstate.digitaloutputstate[i].outputstate = digitalstate[i].outputstate;
                    }

                case 4:
                    {
                        digitalstate[i].outputpin = DIGITAL_OUTPUT_PIN_PCALPIN4;
                        digitalstate[i].outputgroup = DIGITAL_OUTPUT_GROUP_PCALPORT0;
                        if( temp[i] == 0)
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_CLR;
                        else
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_SET;

                        digitalsendoutstate.digitaloutputstate[i].iochflag = 1;
                        digitalsendoutstate.digitaloutputstate[i].topicname = "Exit5Power9";
                        digitalsendoutstate.digitaloutputstate[i].outputstate = digitalstate[i].outputstate;
                    }

                case 5:
                    {
                        digitalstate[i].outputpin = DIGITAL_OUTPUT_PIN_PCALPIN5;
                        digitalstate[i].outputgroup = DIGITAL_OUTPUT_GROUP_PCALPORT0;
                        if( temp[i] == 0)
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_CLR;
                        else
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_SET;

                        digitalsendoutstate.digitaloutputstate[i].iochflag = 1;
                        digitalsendoutstate.digitaloutputstate[i].topicname = "Output1Relay";
                        digitalsendoutstate.digitaloutputstate[i].outputstate = digitalstate[i].outputstate;
                    }

                case 6:
                    {
                        digitalstate[i].outputpin = DIGITAL_OUTPUT_PIN_PCALPIN6;
                        digitalstate[i].outputgroup = DIGITAL_OUTPUT_GROUP_PCALPORT0;
                        if( temp[i] == 0)
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_CLR;
                        else
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_SET;

                        digitalsendoutstate.digitaloutputstate[i].iochflag = 1;
                        digitalsendoutstate.digitaloutputstate[i].topicname = "Output2Relay";
                        digitalsendoutstate.digitaloutputstate[i].outputstate = digitalstate[i].outputstate;
                    }

                case 7:
                    {
                        digitalstate[i].outputpin = DIGITAL_OUTPUT_PIN_PCALPIN7;
                        digitalstate[i].outputgroup = DIGITAL_OUTPUT_GROUP_PCALPORT0;
                        if( temp[i] == 0)
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_CLR;
                        else
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_SET;

                        digitalsendoutstate.digitaloutputstate[i].iochflag = 1;
                        digitalsendoutstate.digitaloutputstate[i].topicname = "Output3Relay";
                        digitalsendoutstate.digitaloutputstate[i].outputstate = digitalstate[i].outputstate;

                    }
                    break;
                case 8:
                    {
                        digitalstate[i].outputpin = DIGITAL_OUTPUT_PIN_PCALPIN7;
                        digitalstate[i].outputgroup = DIGITAL_OUTPUT_GROUP_PCALPORT1;
                        if( temp[i] == 0)
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_CLR;
                        else
                            digitalstate[i].outputstate = DIGITAL_OUTPUT_PIN_STATE_SET;

                        digitalsendoutstate.digitaloutputstate[i].iochflag = 1;
                        digitalsendoutstate.digitaloutputstate[i].topicname = "Output4Relay";
                        digitalsendoutstate.digitaloutputstate[i].outputstate = digitalstate[i].outputstate;
                    }
                    break;
                }

            if(digitalstate[i].outputstate == DIGITAL_OUTPUT_PIN_STATE_SET)
                curiostatus |= 1<<i;
        }
        digitalsendoutstate.flag =1;
        preiostatus = curiostatus;

    return res;
}


