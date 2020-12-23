#ifndef VALVESERVICE_H
#define VALVESERVICE_H
//
#include <QObject>
#include <qextserialport/src/qextserialport.h>

struct modbus_information{
    unsigned int modbus_addr;  /* Modbus 地址*/
    BaudRateType baudrate_type;//波特率
    DataBitsType databits_type;//数据位
    ParityType parity_type;//校验位
    StopBitsType stopbits_type;//停止位
    FlowType flowcontrol_type;//数据流控制
    long timeout_millisec;          //读和写超时设置为n毫秒
};

struct valve_information {
    /* 01 03 00 65 00 32 D4 00 查询寄存器地址0x65(101 地址为3)，50个保持寄存器状态值
     * **************************反馈数据解析***********************************
     * 01 03 64
     * 00 00 00 20 00 38
     * 37 24 50 00 12 51 19 00
     * 00 03 00 02
     * 00 18
     * 20 19 00 12 00 25 00 01 00 37 00 46
     * 00 00 00 32 00 32 00 84
     * 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
     * 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
     * 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
     * 00 00 00 64 00 00 00 00 00 00 00 00 F6 BA
    *************************************************************************/
    /* 适配器分配地址 即仪表编号*/
    unsigned int mbus_addr;
    /* 01协议类型 * 00-国产（CJ／T188-2004）* 01-进口一（一级地址）（EN 1434-3） * 02-进口四（二级地址）（EN 1434-3） */
    unsigned int protocol_type;
    /* 02仪表类型* 10-水表 * 20-热量表  * 30-燃气表  * 40-电表 */
    unsigned int instrument_type;
    /* 03仪表厂家 */
    unsigned int instrument_manufacturers;
    /* 04-07仪表M-bus地址 * 00-国产表：A0-A6七个字节，每个字节为2位BCD码格式，低地址在后，高地址在前  * 01-进口表一：输入表地址后两位 * 02-进口表四：输入完整的表地址*/
    unsigned int instrument_addr;
    /* 08仪表波特率* 00--300  01--600  02--1200  03--2400  04--4800  05--9600  06--19200  07--38400 */
    unsigned int instrument_baudrate;
    /* 09仪表校验位 * 00--无校验  * 01--奇校验  * 02--偶校验 */
    unsigned int instrument_parity;
    /* 10序列号 */
    unsigned int serial_number ;
    /* 17状态ST || Bit0Bit1 00 阀开  01 阀关  11 异常 || Bit2 0 解锁 1 锁定 || Bit3 0 停止 1 动作 || Bit4 0 LED使能 1 LED禁用 */
    unsigned int current_status;
    /* 18当前阀门开度 */
    unsigned int current_angle;
    /* 19设定开度 */
    unsigned int setangle_r;
    /* 20工作模式 */
    unsigned int work_mode;
    /* 46设定阀门开度 */
    unsigned int setangle_w;
};

class ValveService : public QObject
{
    Q_OBJECT
public:
    explicit ValveService(QObject *parent = nullptr);

signals:

public slots:
};

#endif // VALVESERVICE_H
