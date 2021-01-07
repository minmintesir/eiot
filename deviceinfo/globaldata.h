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

#ifndef GLOBALDATA_H
#define GLOBALDATA_H

#include <mutex>
#include <map>
#include <QtCore/QObject>
#include <QtCore/QTranslator>
#include "sqlworker/sqlworker.h"
#include <tools/iniparse.h>
#include <deviceinfo/version.h>
#include "mqttservice.h"

class MqttServiceGlobal : public QObject
{
public:
    MqttServiceGlobal();
    ~MqttServiceGlobal();
    MqttService *mqttservice;
    QMqttClient *mqtt_client;
    void mqttserviceglobal_init();
    void mqttserviceglobal_subscribeTopic();
    void mqttserviceglobal_publishMessage(QString _topicname,QByteArray _message);


private:


};

#endif //GLOBALDATA_H
