#include "mqttservice.h"

MqttService::MqttService(QObject *parent) : QObject(parent){}

QMqttClient* MqttService::mqttservice_init(QString str)
{
    m_client = new QMqttClient(this);

    connect(m_client, &QMqttClient::messageReceived, this,&MqttService::on_subscribeParse);
    connect(m_client, &QMqttClient::disconnected, this, &MqttService::on_brokerDisconnected);
    connect(m_client, &QMqttClient::pingResponseReceived, this, &MqttService::on_pingResponse);

    //从配置文件中读取服務器配置并初始化
    IniParse ini(SERVER_CONFIG);

    if( str.compare("aliyun") == 0){
        service_name = "aliyun";
        _hostname   = ini.GetString("Aliyun.hostname").c_str();
        _port       = ini.GetLong("Aliyun.port");
        _clientid   = ini.GetString("Aliyun.clientid").c_str();
        _username   = ini.GetString("Aliyun.username").c_str();
        _password   = ini.GetString("Aliyun.password").c_str();
        _sub_qos    = ini.GetLong("Aliyun.sub_qos");
        _pub_qos    = ini.GetLong("Aliyun.pub_qos");
        _retain     = ini.GetLong("Aliyun.retain");
        _topic_str  = ini.GetString("Aliyun.topic_str").c_str();

    }else{
        service_name = "standard";
        _hostname   = ini.GetString("Standard.hostname").c_str();
        _port       = ini.GetLong("Standard.port");
        _clientid   = ini.GetString("Standard.clientid").c_str();
        _username   = ini.GetString("Standard.username").c_str();
        _password   = ini.GetString("Standard.password").c_str();
        _sub_qos    = ini.GetLong("Standard.sub_qos");
        _pub_qos    = ini.GetLong("Standard.pub_qos");
        _retain     = ini.GetLong("Standard.retain");
        _topic_str  = ini.GetString("Standard.topic_str").c_str();

    }

    m_client->setHostname(_hostname);
    m_client->setPort(_port);
    m_client->setClientId(_clientid);
    m_client->setUsername(_username);
    m_client->setPassword(_password);

    m_client->connectToHost();
    INFO_LOG("---connectToHost success!---");

    return m_client;
}



//发布消息
void MqttService::mqttservice_publishMessage(QString _topicname,QByteArray _message)
{
    if ( m_client->publish( QMqttTopicName(_topicname),_message,_pub_qos,_retain ) == -1){
        ERROR_LOG("Could not publish message");
    }
}
//订阅topic 获取到消息分析
void MqttService::on_subscribeParse(const QByteArray &message, const QMqttTopicName &topic)
{
    cJSON* root = NULL;
    cJSON* root_payload = NULL;
    cJSON* itemName = NULL;
    const QString content = QDateTime::currentDateTime().toString();
    //BASE64 解码
    //QByteArray payload_bytearry_decode = message.fromBase64(message);
    QByteArray payload_bytearry_decode = message;
    qDebug()<<"on_mqttservice_subscribeparse"<<payload_bytearry_decode;

    //從配置文件中獲取 DeviceNumber
    IniParse ini(EIOT_CONFIG);
    QString _devicenumber = ini.GetString("Device.DeviceNumber").c_str();
    qDebug()<<"DeviceName"<<_devicenumber;

    //解析用户信息
    if( topic.name().compare("UserInformation") == 0 )
    {
        //从缓冲区中解析json结构
        root = cJSON_Parse(payload_bytearry_decode.data());
        //qDebug()<<"payload_bytearry_decode"<<payload_bytearry_decode.data();
        // {"taskId":15,"payload":{"ValveSerialNum":2,"ValveAddress":3,"UserID":"1","MeterSerialNum":4,"MeterAddress":5,"EdgeCtrID":"12345678"}}
        //从缓冲区中解析 payload
        root_payload = cJSON_GetObjectItem(root,"payload");

        if(root_payload != NULL){
            //从json包中获取 EdgeCtrID
            QString _edgectrid = cJSON_GetObjectItem(root_payload, "EdgeCtrID")->valuestring;
            //qDebug()<<"EdgeCtrID"<<_edgectrid;

            //比对deviceNumber是否匹配  再从json包中获取用户信息
            if( _edgectrid.compare(_devicenumber) == 0){
                itemName = cJSON_GetObjectItem(root_payload, "UserID");

                //判断用户名是否有意义
                if( itemName->valuestring != NULL ){
                    //UserID
                    QString _userid = itemName->valuestring;
                    //MeterAddress
                    int _meteraddress = cJSON_GetObjectItem(root_payload, "MeterAddress")->valueint;
                    //MeterSerialNum
                    int _meterserialnum = cJSON_GetObjectItem(root_payload, "MeterSerialNum")->valueint;
                    //ValveAddress
                    int _valveaddress = cJSON_GetObjectItem(root_payload, "ValveAddress")->valueint;
                    //ValveSerialNum
                    int _valveserialnum = cJSON_GetObjectItem(root_payload, "ValveSerialNum")->valueint;

                    qDebug()<<"_meteraddress"<<_meteraddress;
                    qDebug()<<"_meterserialnum"<<_meterserialnum;
                    qDebug()<<"_valveaddress"<<_valveaddress;
                    qDebug()<<"_valveserialnum"<<_valveserialnum;
                    //数据库保存用户信息

                }
            }
            cJSON_Delete(root);
        }
    }
    //解析面板命令信息
    else if(topic.name().compare("CommandMessage")==0) {
        //从缓冲区中解析json结构
        root = cJSON_Parse(payload_bytearry_decode.data());    
        //{"taskId":13,"payload":{"UserID":"1","Temperature":1,"EdgeCtrID":"1"}}
        //从缓冲区中解析 payload
        root_payload = cJSON_GetObjectItem(root,"payload");
        if(root_payload != NULL){
            //从json包中获取 EdgeCtrID
            QString _edgectrid = cJSON_GetObjectItem(root_payload, "EdgeCtrID")->valuestring;

            //比对deviceNumber是否匹配  再从json包中获取用户信息
            if( _edgectrid.compare(_devicenumber) == 0){
                itemName = cJSON_GetObjectItem(root_payload, "UserID");

                //判断用户名是否有意义
                if( itemName->valuestring != NULL ){
                    //UserID
                    QString _userid = itemName->valuestring;
                    //Temperature
                    int _temperature = cJSON_GetObjectItem(root_payload, "Temperature")->valueint;

                    //从数据库中查找阀上级适配器Modebus地址 和在适配器中的序号以计算出寄存器地址
                    //MeterAddress
                    int _meteraddress = 0;
                    //MeterSerialNum
                    int _meterserialnum = 1;
                    //ValveAddress
                    int _valveaddress = 0;
                    //ValveSerialNum
                    int _valveserialnum = 2;

                    qDebug()<<"_temperature"<<_temperature;
                    qDebug()<<"_meteraddress"<<_meteraddress;
                    qDebug()<<"_meterserialnum"<<_meterserialnum;
                    qDebug()<<"_valveaddress"<<_valveaddress;
                    qDebug()<<"_valveserialnum"<<_valveserialnum;

                    //数据库保存命令信息

                }
            }
            cJSON_Delete(root);
        }
    }
    //解析高区总阀配置信息和命令信息
    else if(topic.name().compare("ValueModeHigh")==0) {
        //从缓冲区中解析json结构
        root = cJSON_Parse(payload_bytearry_decode.data());
        //从缓冲区中解析 payload
        root_payload = cJSON_GetObjectItem(root,"payload");

        if(root_payload != NULL){
            //从json包中获取 EdgeCtrID
            QString _edgectrid = cJSON_GetObjectItem(root_payload, "EdgeCtrID")->valuestring;

            //比对deviceNumber是否匹配  再从json包中获取用户信息
            if( _edgectrid.compare(_devicenumber) == 0){
                itemName = cJSON_GetObjectItem(root_payload, "ValueMode");

                //判断模式是否有意义
                if( itemName->valueint < 2 ){
                    //高区总阀控制模式保存配置文件中
                    int _valuemode = itemName->valueint;
                    IniParse ini(EIOT_CONFIG);
                    ini.Set("Device.ValueModeHigh",_valuemode);
                    ini.SaveFile();

                    //ValueOutput
                    int _valueoutput = cJSON_GetObjectItem(root_payload, "ValueOutput")->valueint;

                    switch (_valuemode){
                        //自动模式
                        case 0:{
                            //触发自动控制阀门算法 信号  并一直持续执行该算法

                        }break;
                        //手动模式
                        case 1:{
                            //触发手动控制阀门算法 信号 传参 ValueOutput  单次执行此槽函数

                        }break;
                        default:{
                            ERROR_LOG("---ValueModeHigh Could not parse normally!---");
                        }break;
                    }


                    //从数据库中查找阀上级适配器Modebus地址 和在适配器中的序号以计算出寄存器地址
                    //MeterAddress
                    int _meteraddress = 0;
                    //MeterSerialNum
                    int _meterserialnum = 0;
                    //ValveAddress
                    int _valveaddress = 0;
                    //ValveSerialNum
                    int _valveserialnum = 0;

                    qDebug()<<"_valueoutput"<<_valueoutput;
                    qDebug()<<"_meteraddress"<<_meteraddress;
                    qDebug()<<"_meterserialnum"<<_meterserialnum;
                    qDebug()<<"_valveaddress"<<_valveaddress;
                    qDebug()<<"_valveserialnum"<<_valveserialnum;

                    //数据库保存命令信息

                }
            }
            cJSON_Delete(root);
        }
    }
    //解析低区总阀配置信息和命令信息
    else if(topic.name().compare("ValueModeLow")==0) {
        qDebug()<<"ValueModeLow";
        //从缓冲区中解析json结构
        root = cJSON_Parse(payload_bytearry_decode.data());
        //从缓冲区中解析 payload
        root_payload = cJSON_GetObjectItem(root,"payload");

        if(root_payload != NULL){
            //从json包中获取 EdgeCtrID
            QString _edgectrid = cJSON_GetObjectItem(root_payload, "EdgeCtrID")->valuestring;

            //比对deviceNumber是否匹配  再从json包中获取用户信息
            if( _edgectrid.compare(_devicenumber) == 0){
                itemName = cJSON_GetObjectItem(root_payload, "ValueMode");

                //判断模式是否有意义
                if( itemName->valueint < 2 ){
                    //低区总阀控制模式保存配置文件中
                    int _valuemode = itemName->valueint;
                    IniParse ini(EIOT_CONFIG);
                    ini.Set("Device.ValueModeLow",_valuemode);
                    ini.SaveFile();

                    //ValueOutput
                    int _valueoutput = cJSON_GetObjectItem(root_payload, "ValueOutput")->valueint;

                    switch (_valuemode){
                        //自动模式
                        case 0:{
                            //触发自动控制阀门算法 信号  并一直持续执行该算法

                        }break;
                        //手动模式
                        case 1:{
                            //触发手动控制阀门算法 信号 传参 ValueOutput  单次执行此槽函数

                        }break;
                        default:{
                            ERROR_LOG("---ValueModeHigh Could not parse normally!---");
                        }break;
                    }

                    //从数据库中查找阀上级适配器Modebus地址 和在适配器中的序号以计算出寄存器地址
                    //MeterAddress
                    int _meteraddress = 1;
                    //MeterSerialNum
                    int _meterserialnum = 1;
                    //ValveAddress
                    int _valveaddress = 1;
                    //ValveSerialNum
                    int _valveserialnum = 1;

                    qDebug()<<"_valueoutput"<<_valueoutput;
                    qDebug()<<"_meteraddress"<<_meteraddress;
                    qDebug()<<"_meterserialnum"<<_meterserialnum;
                    qDebug()<<"_valveaddress"<<_valveaddress;
                    qDebug()<<"_valveserialnum"<<_valveserialnum;

                    //数据库保存命令信息

                }
            }
            cJSON_Delete(root);
        }
    }else{
        //未知Topic
        WARN_LOG("---Resolve the unknown topic!---");
         cJSON_Delete(root);
    }
}
//订阅消息
void MqttService::mqttservice_subscribeTopic(QMqttClient *mqtt_client)
{
    QStringList topic_list = _topic_str.split(",");
    if( topic_list.count() > 0 ){
        foreach ( QString topic_name, topic_list ) {
            if ( !( mqtt_client->subscribe( QMqttTopicFilter(topic_name),_sub_qos) ) ) {
                ERROR_LOG("Could not subscribe. Is there a valid connection?");
            }
        }
    }
}
//重连服务器
void MqttService::mqttservice_reconnected(QString str)
{
    mqttservice_init(str);
}

//心跳请求
void MqttService::mqttservice_requestPing(QMqttClient *mqtt_client)
{
    mqtt_client->requestPing();
}

//心跳响应
void MqttService::on_pingResponse()
{
    //qDebug()<<"MQTT on_mqttPing_request";
}

//断线处理
void MqttService::on_brokerDisconnected()
{
     DEBUG_LOG("broker disconnected");
     //重连
     mqttservice_reconnected(service_name);
}


MqttService::~MqttService()
{

}


//void MqttService::on_mqttConnect_clicked()
//{
//    if (m_client->state() == QMqttClient::Disconnected) {
//        m_client->connectToHost();
//        qDebug()<<"MQTT connectToHost";
//    } else {
//        m_client->disconnectFromHost();
//        sleep(1);
//        m_client->connectToHost();
//    }
//}

//void MqttService::updateLogStateChange()
//{
//    const QString content = QDateTime::currentDateTime().toString()
//                    + QLatin1String(": State Change")
//                    + QString::number(m_client->state())
//                    + QLatin1Char('\n');
//    ui->editLog->insertPlainText(content);
//}
