#ifndef ADAPTERMASTER_H
#define ADAPTERMASTER_H

#include <QObject>
#include <QMutex>
#include <QStringList>
#include <QDebug>
#include <QTimer>
#include <QElapsedTimer>
#include <QThread>
#include "qextserialport/src/qextserialport.h"
#include "tools/log.h"
#include "tools/iniparse.h"
#include "deviceinfo/version.h"
#include "tools/common.h"


//UART2  :ttymxc1   RS485
//UART3  :ttymxc2   RS485
//UART6  :ttymxc5   RS232
#define IMX6_ADAPTERMASTER    "/dev/ttymxc1"

#define ADAPTERMASTER_GETDATA_MS      150


class AdapterMaster : public QThread
{
    Q_OBJECT
public:
    AdapterMaster(QObject *parent = nullptr);
    ~AdapterMaster();
    void run();
    //Modbus 地址  功能码  仪表编号  寄存器地址46  角度100
    //set_value_angle(0x1,0x6,0x3,0x2E,0x64);
    int set_value_angle(unsigned char modbus_addr,unsigned char function_code,
                                       unsigned char mbus_addr,unsigned char w_reg,
                                       unsigned char value);
    int set_value_angle_dec(unsigned char modbus_addr,unsigned char function_code,
                                       unsigned char mbus_addr,unsigned char w_reg,
                                       unsigned char value);
    //Modbus 地址 功能码  仪表编号  寄存器地址1  寄存器总数50
    //read_subdevice_reg(0x1,0x3,0x1,0x1,0x32)
    int read_subdevice_reg(unsigned  char modbus_addr,unsigned char function_code,
                                       unsigned char mbus_addr,unsigned char w_reg,
                                       unsigned char reg_num);
private:
    //互斥锁
    bool m_isCanRun;
    QMutex m_lock;
    //接收数据定时
    QTimer *timer;
    //数据接收原始字节数组
    QByteArray baRcvData;
    int m_fd;

    int adaptermaster_init();
    int adaptermaster_write(QextSerialPort *tty_port,QByteArray data);//return write whether successed

    void stopImmediately();



    QextSerialPort *adapter_port;
    void String2Hex(QString str, QByteArray &senddata) ;

    char ConvertHexChar(char ch) ;
signals:
     void tts_message(unsigned int num,QString str);
     //获取到机具识别号，获取阿里云秘钥
     void get_aliyun_key();
     void sensor_capturedata(QString str);
     void sensor_data_analysis(QByteArray str);


private slots:
     void read_from_com();
     void on_timeout();
     void on_sensor_data_analysis(QByteArray str);

};

#endif // ADAPTERMASTER_H
