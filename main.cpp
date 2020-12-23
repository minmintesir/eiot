/******************************************************************************************
*-----First Created. time: 16-November-2020	4:35  P.m.
* @File name    :main.cpp
* @Platform     :imx6ull * openwrt19.07.2 * linux 4.1.15 * Qt5.5.1 * arm-openwrt-linux-gnueabi-
* @Author       :MacianYuan
* @E-mail       :macian@foxmail.com
* @Project      :边缘网关控制器（楼宇供暖控制系统）
*******************************************************************************************
* @Description  :
* @Attention    :
* @Log          :
*****************16-November-2020(MacianYuan):
*****************
* @Copyright© @MacianYuan Anhui Xinjie Technology Co.,Ltd.  2010-2020  All rights reserved.
******************************************************************************************/

#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QDir>
#include "tools/log.h"
#include "tools/iniparse.h"
#include "deviceinfo/version.h"
#include "qextserialport/src/qextserialport.h"
#include "adaptermaster.h"
#include "mainservice.h"

#include <QCoreApplication>
#include <QNetworkInterface>
#include <QProcess>

void eiot_config_init()
{
    IniParse ini(EIOT_CONFIG);
    //modbus 主站设置
    ini.Set("ModbusMaster.SlaveAddress",01);
    ini.Set("ModbusMaster.BaudRateType",BAUD9600);
    ini.Set("ModbusMaster.DataBitsType",DATA_8);
    ini.Set("ModbusMaster.ParityType",PAR_NONE);
    ini.Set("ModbusMaster.StopBitsType",STOP_1);
    ini.Set("ModbusMaster.FlowType",FLOW_OFF);
    ini.Set("ModbusMaster.TimeOutMS",10);

    ini.SaveFile();

}
void adapter_config_init()
{
    IniParse ini(ADAPTER_CONFIG);
    //设备1
    ini.Set("ModbusMaster.SlaveAddress",01);
    ini.Set("ModbusMaster.BaudRateType",BAUD9600);
    ini.Set("ModbusMaster.DataBitsType",DATA_8);
    ini.Set("ModbusMaster.ParityType",PAR_NONE);
    ini.Set("ModbusMaster.StopBitsType",STOP_1);
    ini.Set("ModbusMaster.FlowType",FLOW_OFF);
    ini.Set("ModbusMaster.TimeOutMS",10);

    ini.SaveFile();

}

void server_config_init()
{
    IniParse ini(SERVER_CONFIG);
    //设备1
    ini.Set("Standard.hostname",std::string("mqtt.ctwing.cn"));
    ini.Set("Standard.port",1883);
    ini.Set("Standard.clientid",std::string("15017060sss117434325"));
    ini.Set("Standard.username",std::string("Macian"));
    ini.Set("Standard.password",std::string("n-cfsSNXqB00N9ZykRevj6EDDYgGILsbW2n1fejSy8c"));
    ini.Set("Standard.sub_qos",0);
    ini.Set("Standard.pub_qos",0);
    ini.Set("Standard.retain",0);
    ini.Set("Standard.topic_str",std::string("ValueModeHigh,ValueModeLow,UserInformation,CommandMessage"));
    ini.SaveFile();

}


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

int main(int argc, char *argv[])
{

    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    LoggerControl::getInstance().init(EIOT_LOG, 5 * 1024 * 1024, 5);
    QApplication a(argc, argv);

    /* 设置编码格式为utf-8 默认的情况下 无法识别中文路径 所以这里必须要设置为utf-8 */
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    QFont font;
    font.setPixelSize(20);
    font.setFamily("fangzheng");
    QApplication::setFont(font);


    DEBUG_LOG("System start");
    if (!QFile(EIOT_CONFIG).exists()) {
        eiot_config_init();
    }
    if (!QFile(SERVER_CONFIG).exists()) {
        server_config_init();
    }

    if (!QFile(EIOT_CONFIG).exists()) {
        eiot_config_init();
    }

    /* 设置程序的工作目录为可执行文件所在目录 */
    QDir::setCurrent(QCoreApplication::applicationDirPath());
    AdapterMaster adapter_master;
    //adapter_master.set_value_angle_dec(1,6,3,46,50);
    //adapter_master.set_value_angle(0x1,0x6,0x3,0x2E,0x32);
    adapter_master.read_subdevice_reg(0x1,0x3,0x3,0x1,0x32);


    //获取4G IMEI 号作为设备号，保存到配置文件中
//    IniParse ini(EIOT_CONFIG);
//    ini.Set("Device.DeviceNumber",netInterface.hardwareAddress());
//    ini.SaveFile();


    MainWindow w;
    w.show();

    //主服务运行
    MainService *mainservice = new MainService;
    mainservice->init();

    DEBUG_LOG("System start");
    return a.exec();
}
