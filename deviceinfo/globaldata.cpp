/******************************************************************************************
*-----First Created. time: 16-November-2020	4:35  P.m.
* @File name    :globaldata.cpp
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
#include "globaldata.h"
#include <QString>
#include <QDateTime>

MqttServiceGlobal::MqttServiceGlobal()
{

}

MqttServiceGlobal::~MqttServiceGlobal()
{

}

void MqttServiceGlobal::mqttserviceglobal_init()
{
    mqttservice = new MqttService;
    mqtt_client = mqttservice->mqttservice_init(QString("standard"));

    //mqttservice->mqttservice_subscribeTopic(mqtt_client);
}

void MqttServiceGlobal::mqttserviceglobal_subscribeTopic()
{
    mqttservice->mqttservice_subscribeTopic(mqtt_client);
}

void MqttServiceGlobal::mqttserviceglobal_publishMessage(QString _topicname,QByteArray _message)
{
    mqttservice->mqttservice_publishMessage(_topicname,_message);
}



