#include "adaptermaster.h"

void AdapterMaster::run()
{
    //标记可以运行
    m_isCanRun = true;
    qDebug() <<"AdapterMaster:run";
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    {
        QMutexLocker locker(&m_lock);
        //在每次循环判断是否可以运行，如果不行就退出循环
        if(!m_isCanRun){
            qDebug()<<"m_isCanRun";
            return;
        }
    }
}


int AdapterMaster::read_subdevice_reg(unsigned  char modbus_addr,unsigned char function_code,
                                   unsigned char mbus_addr,unsigned char w_reg,
                                   unsigned char reg_num)
{
    QByteArray senddata;
    unsigned int temp;
    //modbus地址
    senddata[0]=(modbus_addr);
    //功能码
    senddata[1]=(function_code);
    //寄存器地址（由适配器制定的仪表编号生成，仪表编号与M-bus地址对应）
    senddata[2]=(((mbus_addr-1)*0x32+w_reg)>>8);  //高八位
    senddata[3]=(((mbus_addr-1)*0x32+w_reg)&0xFF);//低八位
    //开度值
    senddata[4]=(reg_num>>8); //高八位
    senddata[5]=(reg_num&0xFF);//低八位
    //校验值
    temp = crc16table(senddata);
    senddata[6] = temp>>8; //高八位
    senddata[7] = temp&0xFF;//低八位
    qDebug()<<senddata;

    //发送获取机具号指令
    return adaptermaster_write(adapter_port,senddata);

//    //短暂的延时
//    QElapsedTimer t;
//    t.start();
//    while(t.elapsed()<20);
}

//每块仪表自动占据 50 个寄存器，依据“仪表信息设置”中的顺序，自动安排寄存器。
//第一块 1~~50；第二块 51~~100；第三块101~~150；第 N 块（N-1） *50+1~~N*50
/*
 * 01 06 00 92 00 00 28 27  设置Mbus地址为3的调节阀角度为0
 * 01 06 00 92 00 50 28 1B  设置Mbus地址为3的调节阀角度为80
 * 01 06 00 92 00 64 29 CC  设置Mbus地址为3的调节阀角度为100
 * 485协议：
 * Modbus地址     功能码     Mbus寄存器地址   角度值     16CRC校验（低字节在前）
 *
*/
int AdapterMaster::set_value_angle(unsigned  char modbus_addr,unsigned char function_code,
                                   unsigned char mbus_addr,unsigned char w_reg,
                                   unsigned char value)
{
    QByteArray senddata;
    unsigned int temp;
    //modbus地址
    senddata[0]=(modbus_addr);
    //功能码
    senddata[1]=(function_code);
    //寄存器地址（由适配器制定的仪表编号生成，仪表编号与M-bus地址对应）
    senddata[2]=(((mbus_addr-1)*0x32+w_reg)>>8);  //高八位
    senddata[3]=(((mbus_addr-1)*0x32+w_reg)&0xFF);//低八位
    //开度值
    senddata[4]=(value>>8); //高八位
    senddata[5]=(value&0xFF);//低八位
    //校验值
    temp = crc16table(senddata);
    senddata[6] = temp>>8; //高八位
    senddata[7] = temp&0xFF;//低八位
    qDebug()<<senddata;

    //发送获取机具号指令
    return adaptermaster_write(adapter_port,senddata);

//    //短暂的延时
//    QElapsedTimer t;
//    t.start();
//    while(t.elapsed()<20);
}



void AdapterMaster::on_sensor_data_analysis(QByteArray str)
{
    QString OriginalData = QString(str.toHex().toUpper());


    qDebug()<<"AdapterMaster OriginalData"<<OriginalData;

    qDebug()<<"OriginalData"<<OriginalData;


    QString CaptureData;

    //010364
    //0002 0020 0002   【1】协议类型 【2】仪表类型 【3】仪表厂商
    //5277349000000000 【4-7】仪表地址
    //0003 0002        【8】波特率 【9】校验位
    //000041C0 000041B8 00003F80 00000000 0000  【10-11】供温 【12-13】回温 【14-15】温差 【16-17】瞬时流量 【18】单位
    //00000000 0000 00004320 0000 00000000 0000 【19-20】瞬时热量 【21】单位 【22-23】累计流量 【24】单位 【25-26】累计热量 【27】单位
    //00000000 0000 0000000 0000 【28-29】累计冷量 【30】单位 【31-32】结算日热量 【33】单位
    //0010 00000000 0000 00000000 0000  【34】errorcode  【35-36】累积水量 【37】单位  【38-39】累积气量 【40】单位
    //000043A0 0003 0000   【41-42】累计工作时间  【43】单位  【44】状态字
    //00000000005A0000005A0000E3CC 备用

    IniParse ini(ADAPTER_CONFIG);

    QString frameheader;
    if(OriginalData.startsWith("010364")){

    }
    //010364
    //0000 0020 0038  【1】协议类型 【2】仪表类型 【3】仪表厂商
    //3724500012511900 【4-7】仪表地址
    //0003 0002 【 8】波特率 【9】校验位
    //0029 2019 0012 0025 0002 0037 0034  【10】序列号 【11】年【12】月【13】日【14】时【15】分【16】秒
    //0000 0032 0032 0084  【17】状态 【18】阀门开度 【19】设定开度 【20】工作模式
    //21-45备用
    //0032 【46】设定阀门开度  （0x06）
    //00000000000000009F1E
    if(OriginalData.startsWith("550A")){

    }
}



/******************************************************************************************
* @Function         : adaptermaster_init
*
* @Purpose          : 初始化
* @Description      :
********************* 串口驱动 485
********************* 硬件端口设备描述符  /dev/ttymxc1    波特率9600
********************* End
* @Grammar&Demand   :
********************* <QThread>
********************* 利用线程msleep秒睡眠
********************* End
* @Param            : None
* @Retval           : Bool  true 成功  false失败
* @Others           :
* @E.g.             : None
* @Log          :
*****************
*****************
******************************************************************************************/
int AdapterMaster::adaptermaster_init()
{

    //串口初始化
    adapter_port = new QextSerialPort(IMX6_ADAPTERMASTER,QextSerialPort::EventDriven);
    //注意：得要先打开串口，然后再设置串口的参数，不然设置无效！！！
    m_fd = adapter_port->open(QIODevice::ReadWrite);

    //定义串口对象，并传递参数，在构造函数里对其进行初始化
    if(m_fd){
        //Device_public::sensor_port->setDtr(true);
        //Device_public::sensor_port->setRts(false);
        //从配置文件中读取串口配置并初始化
        IniParse ini(EIOT_CONFIG);
        //设置波特率
        adapter_port->setBaudRate((BaudRateType)ini.GetLong("ModbusMaster.BaudRateType"));
        //设置数据位
        adapter_port->setDataBits((DataBitsType)ini.GetLong("ModbusMaster.DataBitsType"));
        //设置奇偶校验
        adapter_port->setParity((ParityType)ini.GetLong("ModbusMaster.ParityType"));
        //设置停止位
        adapter_port->setStopBits((StopBitsType)ini.GetLong("ModbusMaster.StopBitsType"));
        //设置数据流控制
        adapter_port->setFlowControl((FlowType)ini.GetLong("ModbusMaster.FlowType"));
        //设置延时
        adapter_port->setTimeout((long)ini.GetLong("ModbusMaster.TimeOutMS"));
        DEBUG_LOG("adapter_port IMX6_ADAPTERMASTER success! ");
    }else{
        ERROR_LOG("open IMX6_ADAPTERMASTER failed");
    }
    //有数据读取到，进入数据分析槽函数，分析出数据
    connect(adapter_port, SIGNAL(readyRead()), this,SLOT(read_from_com()));

    //定时接收100ms的数据
    timer = new QTimer(this);
    timer->setInterval(ADAPTERMASTER_GETDATA_MS);
    //在有数据到达时开启计时
    connect(timer, SIGNAL(timeout()), this,SLOT(on_timeout()));

    //连接分析函数
    connect(this,SIGNAL(sensor_data_analysis(QByteArray)),this,SLOT(on_sensor_data_analysis(QByteArray)));

    return m_fd;
}
void AdapterMaster::stopImmediately()
{
    QMutexLocker locker(&m_lock);
    m_isCanRun = false;
}

//读数据 对数据进行解析，定时100ms的数据
void AdapterMaster::read_from_com()
{
    //开始接受下面100ms的数据
    timer->start();
    baRcvData.append(adapter_port->readAll());

}
void AdapterMaster::on_timeout()
{
    timer->stop();
    //qDebug()<<"baRcvData"<<baRcvData;
    if ( baRcvData.length() != 0 ){
        emit sensor_data_analysis(baRcvData);
        baRcvData.clear();
    }
}

int AdapterMaster::adaptermaster_write(QextSerialPort *tty_port, QByteArray data)
{
    if(tty_port == 0  || !tty_port->isOpen()){
        qDebug() << "SensorAnalysis write_com Open Failed!";
        return -1;
    }
    if(tty_port->write(data) <= 0){
        tty_port->close();
        return -1;
    }
    return 0;
}
AdapterMaster::AdapterMaster(QObject *parent) : QThread(parent) {
    adaptermaster_init();
}

AdapterMaster::~AdapterMaster() {

}
