#include "mqttservice.h"
#include "globaldata.h"

ANALOG_OUTPUT_REV_STRUCT anlaogoutput;

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

int  MqttService::creat_publishdatagroub(const QByteArray &message)
{
    int res;

     /*   TOPIC:
      *   /a1LOqD9O9II/20200701001/user/get_mode
      */

     //QByteArray pub_topic_char("/");
     //pub_topic_char.append(Device_public::ProductKey);
     //pub_topic_char.append("/");
     //pub_topic_char.append(Device_public::device_number);
     //pub_topic_char.append("/user/get_mode");

     /*  JSON 包:
      *  {
      *      "deviceNumber": "20200701001",
      *      "eventDate":    "2019-03-20 18:13:33"
      *  }
      *
      */

     //  创建JSON 包
     cJSON *event_data = cJSON_CreateObject();

     //  获取当前设备机具号
     //QByteArray device_name = Device_public::device_number.toLatin1();

     //  获取当前设备系统时间
     //QByteArray time_bytearray = Device_public::timeString.toString("yyyy-MM-dd hh:mm:ss").toLatin1();
     //qDebug()<<"time_bytearray: "<<time_bytearray.data();

     //cJSON_AddStringToObject(event_data, "deviceNumber",device_name.data());
     //cJSON_AddStringToObject(event_data, "eventDate", time_bytearray.data());
     //  字节数组转化
     //QByteArray pub_payload_string(cJSON_Print(event_data));

     //qDebug()<< pub_payload_string.data();
     //qDebug()<< pub_payload_string.toBase64().data();

     //cJSON_Delete(event_data);
     //res =   aiot_mqtt_pub(Device_public::mqtt_handle, pub_topic_char.data(), (uint8_t *)pub_payload_string.toBase64().data(),pub_payload_string.toBase64().size(), 0);
     if (res < 0) {
          printf("aiot_mqtt_sub failed, res: -0x%04X\n", -res);
     }

}

//发布消息
void MqttService::mqttservice_publishMessage(QString _topicname,QByteArray &_message)
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
        qDebug()<<"DeviceName11";
        root = cJSON_Parse(payload_bytearry_decode.data());
        //qDebug()<<"payload_bytearry_decode"<<payload_bytearry_decode.data();
        // {"taskId":15,"payload":{"ValveSerialNum":2,"ValveAddress":3,"UserID":"1","MeterSerialNum":4,"MeterAddress":5,"EdgeCtrID":"12345678"}}
        //从缓冲区中解析 payload
        root_payload = cJSON_GetObjectItem(root,"payload");
        qDebug()<<"DeviceName22";
        if(root_payload != NULL){
            //从json包中获取 EdgeCtrID
            QString _edgectrid = cJSON_GetObjectItem(root_payload, "EdgeCtrID")->valuestring;
            //qDebug()<<"EdgeCtrID"<<_edgectrid;
            qDebug()<<"DeviceName33";
            //比对deviceNumber是否匹配  再从json包中获取用户信息
            if( _edgectrid.compare(_devicenumber) == 0){
                itemName = cJSON_GetObjectItem(root_payload, "RoomID");

                //判断用户名是否有意义
                if( itemName->valuestring != NULL ){
                    //UserID
                    QString _userid = itemName->valuestring;
                    QString date = cJSON_GetObjectItem(root_payload, "EventDate")->valuestring;
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
                    qDebug()<<"date"<<date;
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
                itemName = cJSON_GetObjectItem(root_payload, "RoomID");

                //判断用户名是否有意义
                if( itemName->valuestring != NULL ){
                    //UserID
                    QString _userid = itemName->valuestring;
                    //Temperature
                    int _temperature = cJSON_GetObjectItem(root_payload, "Temperature")->valueint;
                    QString date = cJSON_GetObjectItem(root_payload, "EventDate")->valuestring;
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
                    qDebug()<<"date"<<date;

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
                            ini.Set("Device.ValueHighOutput",_valueoutput);
                            ini.SaveFile();

                            anlaogoutput.analogoutputsingalch[0].angle = _valueoutput;
                            anlaogoutput.analogoutputsingalch[0].flag =1;
                            anlaogoutput.flag =1;
                            printf("rec mode %d \r\n",_valuemode);
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
                            ini.Set("Device.ValueLowOutput",_valueoutput);
                            ini.SaveFile();
                            anlaogoutput.analogoutputsingalch[1].angle = _valueoutput;
                            anlaogoutput.analogoutputsingalch[1].flag =1;
                            anlaogoutput.flag =1;
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

int MqttService::topictonum(QString str)
{
    int reseult =0;;

    if(str.compare("InletPressLow")==0)
        reseult=1;
    if(str.compare("InletPressHigh")==0)
        reseult=2;
    if(str.compare("InletTempLow")==0)
        reseult=3;
    if(str.compare("InletTempLowO")==0)
        reseult=4;
    if(str.compare("InletTempHigh")==0)
        reseult=5;
    if(str.compare("InletTempHighO")==0)
        reseult=6;
    if(str.compare("TotalValveLow")==0)
        reseult=7;
    if(str.compare("TotalValveHigh")==0)
        reseult=8;

    return reseult;
}

int  MqttService::mqttservice_senddata(ANALOG_SEND_PACKAGE analogsendpackage)
{
    int res = 0;
    int i = 0;

    if(analogsendpackage.flag == 1)
        {
            //从配置文件中读取服務器配置并初始化

            IniParse ini(EIOT_CONFIG);
            QString _devicenumber ="12345678"; //ini.GetString("Device.DeviceNumber").c_str();
             /*
                QString anlogtopic[ANALOGINPUT]=
                {
                    "InletPressLow",
                    "InletPressHigh",
                    "InletTempLow",
                    "InletTempLowO",
                    "InletTempHigh",
                    "InletTempHighO",
                    "TotalValveLow",
                    "TotalValveHigh"
                };
            */
            for(;i<8;i++)
                {
                    if(i == 0)
                        {
                            QString _topic = ini.GetString("Device.AnalogTopic1").c_str();
                            int minvalue = ini.GetLong("Device.Topic1MinValue");
                            int maxvalue = ini.GetLong("Device.Topic1MaxValue");
                            int valueformadc;
                            qDebug()<<"mqttservice_senddata:"<<_topic;
                            //QByteArray _topic(_itopic);
                            /*  JSON 包:
                             * {
                             *      "EdgeCtrID": "",
                             *      "Pressure": -2147483648
                             *      "EventDate":
                             * }
                             *
                             */

                            //  创建JSON 包
                            cJSON *event_data = cJSON_CreateObject();

                            //  获取当前设备系统时间
                            QDateTime current_date_time = QDateTime::currentDateTime();
                            QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm::ss.zzz");
                            QByteArray time_bytearray =  current_date.toLatin1();
                            valueformadc = (int)((maxvalue-minvalue)*(analogsendpackage.analogchanalog[0].adcvalue-13107)/(65535-13107)+minvalue);
                            QString analogdata = QString("%1").arg(valueformadc);
                            if(_topic.compare("TotalValveLow")==0)
                                {
                                    int valueout = 0;
                                    int mode = 0;

                                    mode = ini.GetLong("Device.ValueModeLow");
                                    if(mode ==0)
                                        {
                                            QString analogoutput = QString("%1").arg(valueout);
                                            cJSON_AddStringToObject(event_data, "ValueOutput", analogoutput.toUtf8().data());
                                        }
                                    if(mode ==1)
                                        {
                                            valueout = ini.GetLong("Device.ValueLowOutput");
                                            QString analogoutput = QString("%1").arg(valueout);
                                            cJSON_AddStringToObject(event_data, "ValueOutput", analogoutput.toUtf8().data());
                                        }

                                }
                            else if(_topic.compare("TotalValveHigh")==0)
                            {
                                int valueout = 0;
                                int mode = 0;

                                mode = ini.GetLong("Device.ValueModeHigh");
                                if(mode ==0)
                                    {
                                        QString analogoutput = QString("%1").arg(valueout);
                                        cJSON_AddStringToObject(event_data, "ValueOutputH", analogoutput.toUtf8().data());
                                    }
                                if(mode ==1)
                                    {
                                        valueout = ini.GetLong("Device.ValueHighOutput");
                                        QString analogoutput = QString("%1").arg(valueout);
                                        cJSON_AddStringToObject(event_data, "ValueOutputH", analogoutput.toUtf8().data());
                                    }
                            }

                            cJSON_AddStringToObject(event_data, "EdgeCtrID",_devicenumber.toLatin1().data());
                            int _topicnum = topictonum(_topic);
                            switch(_topicnum) {
                                    case 3:
                                        {
                                            cJSON_AddStringToObject(event_data, "InletFlowTemp", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:InletFlowTemp"<<analogdata;
                                        }
                                        break;
                                    case 5:
                                        {
                                            cJSON_AddStringToObject(event_data, "InletFlowTempH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:InletFlowTempH"<<analogdata;
                                        }
                                        break;

                                    case 4:
                                        {
                                            cJSON_AddStringToObject(event_data, "OutletFlowTemp", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:OutletFlowTemp"<<analogdata;
                                        }
                                        break;
                                    case 6:
                                        {
                                            cJSON_AddStringToObject(event_data, "OutletFlowTempH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:OutletFlowTempH"<<analogdata;
                                        }
                                        break;
                                    case 1:
                                        {
                                            cJSON_AddStringToObject(event_data, "Pressure", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:Pressure"<<analogdata;
                                        }
                                        break;
                                    case 2:
                                        {
                                            cJSON_AddStringToObject(event_data, "PressureH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:PressureH"<<analogdata;
                                        }
                                        break;
                                    case 7:
                                        {
                                            cJSON_AddStringToObject(event_data, "ValueFeedback", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:ValueFeedback"<<analogdata;
                                        }
                                        break;
                                    case 8:
                                        {
                                            cJSON_AddStringToObject(event_data, "ValueFeedbackH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:ValueFeedbackH"<<analogdata;
                                        }
                                        break;

                            }

                            cJSON_AddStringToObject(event_data, "EventDate", time_bytearray.data());//设备版本号"01"
                            //  字节数组转化
                            QByteArray _message(cJSON_Print(event_data));
                            cJSON_Delete(event_data);
                            printf("mqttservice_senddata: ch_%d,val_%d ,minvalue_%d ,maxvalue_%d\r\n",i,valueformadc,minvalue,maxvalue);
                            MqttService::mqttservice_publishMessage(_topic,_message);
                            //i++;
                        }
                    else if(i == 1)
                        {
                        QString _topic = ini.GetString("Device.AnalogTopic2").c_str();
                        int minvalue = ini.GetLong("Device.Topic2MinValue");
                        int maxvalue = ini.GetLong("Device.Topic2MaxValue");
                        int valueformadc;
                        //QByteArray _topic("_itopic.()");
                            /*  JSON 包:
                             * {
                             *      "EdgeCtrID": "",
                             *      "Pressure": -2147483648
                             *      "EventDate":
                             * }
                             *
                             */
                            //  创建JSON 包
                            cJSON *event_data = cJSON_CreateObject();

                            //  获取当前设备系统时间
                            QDateTime current_date_time = QDateTime::currentDateTime();
                            QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm::ss.zzz");
                            QByteArray time_bytearray =  current_date.toLatin1();
                            valueformadc = (int)((maxvalue-minvalue)*(analogsendpackage.analogchanalog[1].adcvalue-13107)/(65535-13107)+minvalue);
                            QString analogdata = QString("%1").arg(valueformadc);
                            if(_topic.compare("TotalValveLow")==0)
                                {
                                    int valueout = 0;
                                    int mode = 0;

                                    mode = ini.GetLong("Device.ValueModeLow");
                                    if(mode ==0)
                                        {
                                            QString analogoutput = QString("%1").arg(valueout);
                                            cJSON_AddStringToObject(event_data, "ValueOutput", analogoutput.toUtf8().data());
                                        }
                                    if(mode ==1)
                                        {
                                            valueout = ini.GetLong("Device.ValueLowOutput");
                                            QString analogoutput = QString("%1").arg(valueout);
                                            cJSON_AddStringToObject(event_data, "ValueOutput", analogoutput.toUtf8().data());
                                        }

                                }
                            else if(_topic.compare("TotalValveHigh")==0)
                            {
                                int valueout = 0;
                                int mode = 0;

                                mode = ini.GetLong("Device.ValueModeHigh");
                                if(mode ==0)
                                    {
                                        QString analogoutput = QString("%1").arg(valueout);
                                        cJSON_AddStringToObject(event_data, "ValueOutputH", analogoutput.toUtf8().data());
                                    }
                                if(mode ==1)
                                    {
                                        valueout = ini.GetLong("Device.ValueHighOutput");
                                        QString analogoutput = QString("%1").arg(valueout);
                                        cJSON_AddStringToObject(event_data, "ValueOutputH", analogoutput.toUtf8().data());
                                    }
                            }

                            cJSON_AddStringToObject(event_data, "EdgeCtrID",_devicenumber.toLatin1().data());
                            int _topicnum = topictonum(_topic);
                            switch(_topicnum) {
                                    case 3:
                                        {
                                            cJSON_AddStringToObject(event_data, "InletFlowTemp", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:InletFlowTemp"<<analogdata;
                                        }
                                        break;
                                    case 5:
                                        {
                                            cJSON_AddStringToObject(event_data, "InletFlowTempH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:InletFlowTempH"<<analogdata;
                                        }
                                        break;

                                    case 4:
                                        {
                                            cJSON_AddStringToObject(event_data, "OutletFlowTemp", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:OutletFlowTemp"<<analogdata;
                                        }
                                        break;
                                    case 6:
                                        {
                                            cJSON_AddStringToObject(event_data, "OutletFlowTempH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:OutletFlowTempH"<<analogdata;
                                        }
                                        break;
                                    case 1:
                                        {
                                            cJSON_AddStringToObject(event_data, "Pressure", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:Pressure"<<analogdata;
                                        }
                                        break;
                                    case 2:
                                        {
                                            cJSON_AddStringToObject(event_data, "PressureH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:PressureH"<<analogdata;
                                        }
                                        break;
                                    case 7:
                                        {
                                            cJSON_AddStringToObject(event_data, "ValueFeedback", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:ValueFeedback"<<analogdata;
                                        }
                                        break;
                                    case 8:
                                        {
                                            cJSON_AddStringToObject(event_data, "ValueFeedbackH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:ValueFeedbackH"<<analogdata;
                                        }
                                        break;

                            }

                            cJSON_AddStringToObject(event_data, "EventDate", time_bytearray.data());//设备版本号"01"
                            //  字节数组转化
                            QByteArray _message(cJSON_Print(event_data));
                            cJSON_Delete(event_data);
                            printf("mqttservice_senddata: ch_%d,val_%d ,minvalue_%d ,maxvalue_%d\r\n",i,valueformadc,minvalue,maxvalue);
                            MqttService::mqttservice_publishMessage(_topic,_message);
                            //i++;
                        }
                    else if(i == 2)
                        {
                            QString _topic = ini.GetString("Device.AnalogTopic3").c_str();
                            int minvalue = ini.GetLong("Device.Topic1MinValue");
                            int maxvalue = ini.GetLong("Device.Topic1MaxValue");
                            int valueformadc;
                            //QByteArray _topic("_itopic.()");
                            /*  JSON 包:
                             * {
                             *      "EdgeCtrID": "",
                             *      "Pressure": -2147483648
                             *      "EventDate":
                             * }
                             *
                             */
                            //  创建JSON 包
                            cJSON *event_data = cJSON_CreateObject();

                            //  获取当前设备系统时间
                            QDateTime current_date_time = QDateTime::currentDateTime();
                            QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm::ss.zzz");
                            QByteArray time_bytearray =  current_date.toLatin1();
                            valueformadc = (int)((maxvalue-minvalue)*(analogsendpackage.analogchanalog[2].adcvalue-13107)/(65535-13107)+minvalue);
                            QString analogdata = QString("%1").arg(valueformadc);
                            if(_topic.compare("TotalValveLow")==0)
                                {
                                    int valueout = 0;
                                    int mode = 0;

                                    mode = ini.GetLong("Device.ValueModeLow");
                                    if(mode ==0)
                                        {
                                            QString analogoutput = QString("%1").arg(valueout);
                                            cJSON_AddStringToObject(event_data, "ValueOutput", analogoutput.toUtf8().data());
                                        }
                                    if(mode ==1)
                                        {
                                            valueout = ini.GetLong("Device.ValueLowOutput");
                                            QString analogoutput = QString("%1").arg(valueout);
                                            cJSON_AddStringToObject(event_data, "ValueOutput", analogoutput.toUtf8().data());
                                        }

                                }
                            else if(_topic.compare("TotalValveHigh")==0)
                            {
                                int valueout = 0;
                                int mode = 0;

                                mode = ini.GetLong("Device.ValueModeHigh");
                                if(mode ==0)
                                    {
                                        QString analogoutput = QString("%1").arg(valueout);
                                        cJSON_AddStringToObject(event_data, "ValueOutputH", analogoutput.toUtf8().data());
                                    }
                                if(mode ==1)
                                    {
                                        valueout = ini.GetLong("Device.ValueHighOutput");
                                        QString analogoutput = QString("%1").arg(valueout);
                                        cJSON_AddStringToObject(event_data, "ValueOutputH", analogoutput.toUtf8().data());
                                    }
                            }

                            cJSON_AddStringToObject(event_data, "EdgeCtrID",_devicenumber.toLatin1().data());
                            int _topicnum = topictonum(_topic);
                            switch(_topicnum) {
                                    case 3:
                                        {
                                            cJSON_AddStringToObject(event_data, "InletFlowTemp", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:InletFlowTemp"<<analogdata;
                                        }
                                        break;
                                    case 5:
                                        {
                                            cJSON_AddStringToObject(event_data, "InletFlowTempH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:InletFlowTempH"<<analogdata;
                                        }
                                        break;

                                    case 4:
                                        {
                                            cJSON_AddStringToObject(event_data, "OutletFlowTemp", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:OutletFlowTemp"<<analogdata;
                                        }
                                        break;
                                    case 6:
                                        {
                                            cJSON_AddStringToObject(event_data, "OutletFlowTempH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:OutletFlowTempH"<<analogdata;
                                        }
                                        break;
                                    case 1:
                                        {
                                            cJSON_AddStringToObject(event_data, "Pressure", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:Pressure"<<analogdata;
                                        }
                                        break;
                                    case 2:
                                        {
                                            cJSON_AddStringToObject(event_data, "PressureH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:PressureH"<<analogdata;
                                        }
                                        break;
                                    case 7:
                                        {
                                            cJSON_AddStringToObject(event_data, "ValueFeedback", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:ValueFeedback"<<analogdata;
                                        }
                                        break;
                                    case 8:
                                        {
                                            cJSON_AddStringToObject(event_data, "ValueFeedbackH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:ValueFeedbackH"<<analogdata;
                                        }
                                        break;

                            }

                            cJSON_AddStringToObject(event_data, "EventDate", time_bytearray.data());//设备版本号"01"
                            //  字节数组转化
                            QByteArray _message(cJSON_Print(event_data));
                            cJSON_Delete(event_data);
                            printf("mqttservice_senddata: ch_%d,val_%d ,minvalue_%d ,maxvalue_%d\r\n",i,valueformadc,minvalue,maxvalue);
                            MqttService::mqttservice_publishMessage(_topic,_message);
                            //i++;
                        }
                    else if(i == 3)
                        {
                            QString _topic = ini.GetString("Device.AnalogTopic4").c_str();
                            int minvalue = ini.GetLong("Device.Topic4MinValue");
                            int maxvalue = ini.GetLong("Device.Topic4MaxValue");
                            int valueformadc;
                            //QByteArray _topic("_itopic.()");
                            /*  JSON 包:
                             * {
                             *      "EdgeCtrID": "",
                             *      "Pressure": -2147483648
                             *      "EventDate":
                             * }
                             *
                             */
                            //  创建JSON 包
                            cJSON *event_data = cJSON_CreateObject();

                            //  获取当前设备系统时间
                            QDateTime current_date_time = QDateTime::currentDateTime();
                            QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm::ss.zzz");
                            QByteArray time_bytearray =  current_date.toLatin1();
                            valueformadc = (int)((maxvalue-minvalue)*(analogsendpackage.analogchanalog[3].adcvalue-13107)/(65535-13107)+minvalue);
                            QString analogdata = QString("%1").arg(valueformadc);
                            if(_topic.compare("TotalValveLow")==0)
                                {
                                    int valueout = 0;
                                    int mode = 0;

                                    mode = ini.GetLong("Device.ValueModeLow");
                                    if(mode ==0)
                                        {
                                            QString analogoutput = QString("%1").arg(valueout);
                                            cJSON_AddStringToObject(event_data, "ValueOutput", analogoutput.toUtf8().data());
                                        }
                                    if(mode ==1)
                                        {
                                            valueout = ini.GetLong("Device.ValueLowOutput");
                                            QString analogoutput = QString("%1").arg(valueout);
                                            cJSON_AddStringToObject(event_data, "ValueOutput", analogoutput.toUtf8().data());
                                        }

                                }
                            else if(_topic.compare("TotalValveHigh")==0)
                            {
                                int valueout = 0;
                                int mode = 0;

                                mode = ini.GetLong("Device.ValueModeHigh");
                                if(mode ==0)
                                    {
                                        QString analogoutput = QString("%1").arg(valueout);
                                        cJSON_AddStringToObject(event_data, "ValueOutputH", analogoutput.toUtf8().data());
                                    }
                                if(mode ==1)
                                    {
                                        valueout = ini.GetLong("Device.ValueHighOutput");
                                        QString analogoutput = QString("%1").arg(valueout);
                                        cJSON_AddStringToObject(event_data, "ValueOutputH", analogoutput.toUtf8().data());
                                    }
                            }

                            cJSON_AddStringToObject(event_data, "EdgeCtrID",_devicenumber.toLatin1().data());
                            int _topicnum = topictonum(_topic);
                            switch(_topicnum) {
                                    case 3:
                                        {
                                            cJSON_AddStringToObject(event_data, "InletFlowTemp", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:InletFlowTemp"<<analogdata;
                                        }
                                        break;
                                    case 5:
                                        {
                                            cJSON_AddStringToObject(event_data, "InletFlowTempH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:InletFlowTempH"<<analogdata;
                                        }
                                        break;

                                    case 4:
                                        {
                                            cJSON_AddStringToObject(event_data, "OutletFlowTemp", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:OutletFlowTemp"<<analogdata;
                                        }
                                        break;
                                    case 6:
                                        {
                                            cJSON_AddStringToObject(event_data, "OutletFlowTempH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:OutletFlowTempH"<<analogdata;
                                        }
                                        break;
                                    case 1:
                                        {
                                            cJSON_AddStringToObject(event_data, "Pressure", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:Pressure"<<analogdata;
                                        }
                                        break;
                                    case 2:
                                        {
                                            cJSON_AddStringToObject(event_data, "PressureH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:PressureH"<<analogdata;
                                        }
                                        break;
                                    case 7:
                                        {
                                            cJSON_AddStringToObject(event_data, "ValueFeedback", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:ValueFeedback"<<analogdata;
                                        }
                                        break;
                                    case 8:
                                        {
                                            cJSON_AddStringToObject(event_data, "ValueFeedbackH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:ValueFeedbackH"<<analogdata;
                                        }
                                        break;

                            }

                            cJSON_AddStringToObject(event_data, "EventDate", time_bytearray.data());//设备版本号"01"
                            //  字节数组转化
                            QByteArray _message(cJSON_Print(event_data));
                            cJSON_Delete(event_data);
                            printf("mqttservice_senddata: ch_%d,val_%d ,minvalue_%d ,maxvalue_%d\r\n",i,valueformadc,minvalue,maxvalue);
                            MqttService::mqttservice_publishMessage(_topic,_message);
                            //i++;
                        }
                    else if(i == 4)
                        {
                        QString _topic = ini.GetString("Device.AnalogTopic5").c_str();
                        int minvalue = ini.GetLong("Device.Topic5MinValue");
                        int maxvalue = ini.GetLong("Device.Topic5MaxValue");
                        int valueformadc;
                        //QByteArray _topic("_itopic.()");
                            /*  JSON 包:
                             * {
                             *      "EdgeCtrID": "",
                             *      "Pressure": -2147483648
                             *      "EventDate":
                             * }
                             *
                             */
                            //  创建JSON 包
                            cJSON *event_data = cJSON_CreateObject();

                            //  获取当前设备系统时间
                            QDateTime current_date_time = QDateTime::currentDateTime();
                            QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm::ss.zzz");
                            QByteArray time_bytearray =  current_date.toLatin1();
                            valueformadc = (int)((maxvalue-minvalue)*(analogsendpackage.analogchanalog[4].adcvalue-13107)/(65535-13107)+minvalue);
                            QString analogdata = QString("%1").arg(valueformadc);
                            if(_topic.compare("TotalValveLow")==0)
                                {
                                    int valueout = 0;
                                    int mode = 0;

                                    mode = ini.GetLong("Device.ValueModeLow");
                                    if(mode ==0)
                                        {
                                            QString analogoutput = QString("%1").arg(valueout);
                                            cJSON_AddStringToObject(event_data, "ValueOutput", analogoutput.toUtf8().data());
                                        }
                                    if(mode ==1)
                                        {
                                            valueout = ini.GetLong("Device.ValueLowOutput");
                                            QString analogoutput = QString("%1").arg(valueout);
                                            cJSON_AddStringToObject(event_data, "ValueOutput", analogoutput.toUtf8().data());
                                        }

                                }
                            else if(_topic.compare("TotalValveHigh")==0)
                            {
                                int valueout = 0;
                                int mode = 0;

                                mode = ini.GetLong("Device.ValueModeHigh");
                                if(mode ==0)
                                    {
                                        QString analogoutput = QString("%1").arg(valueout);
                                        cJSON_AddStringToObject(event_data, "ValueOutputH", analogoutput.toUtf8().data());
                                    }
                                if(mode ==1)
                                    {
                                        valueout = ini.GetLong("Device.ValueHighOutput");
                                        QString analogoutput = QString("%1").arg(valueout);
                                        cJSON_AddStringToObject(event_data, "ValueOutputH", analogoutput.toUtf8().data());
                                    }
                            }

                            cJSON_AddStringToObject(event_data, "EdgeCtrID",_devicenumber.toLatin1().data());
                            int _topicnum = topictonum(_topic);
                            switch(_topicnum) {
                                    case 3:
                                        {
                                            cJSON_AddStringToObject(event_data, "InletFlowTemp", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:InletFlowTemp"<<analogdata;
                                        }
                                        break;
                                    case 5:
                                        {
                                            cJSON_AddStringToObject(event_data, "InletFlowTempH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:InletFlowTempH"<<analogdata;
                                        }
                                        break;

                                    case 4:
                                        {
                                            cJSON_AddStringToObject(event_data, "OutletFlowTemp", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:OutletFlowTemp"<<analogdata;
                                        }
                                        break;
                                    case 6:
                                        {
                                            cJSON_AddStringToObject(event_data, "OutletFlowTempH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:OutletFlowTempH"<<analogdata;
                                        }
                                        break;
                                    case 1:
                                        {
                                            cJSON_AddStringToObject(event_data, "Pressure", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:Pressure"<<analogdata;
                                        }
                                        break;
                                    case 2:
                                        {
                                            cJSON_AddStringToObject(event_data, "PressureH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:PressureH"<<analogdata;
                                        }
                                        break;
                                    case 7:
                                        {
                                            cJSON_AddStringToObject(event_data, "ValueFeedback", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:ValueFeedback"<<analogdata;
                                        }
                                        break;
                                    case 8:
                                        {
                                            cJSON_AddStringToObject(event_data, "ValueFeedbackH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:ValueFeedbackH"<<analogdata;
                                        }
                                        break;

                            }

                            cJSON_AddStringToObject(event_data, "EventDate", time_bytearray.data());//设备版本号"01"
                            //  字节数组转化
                            QByteArray _message(cJSON_Print(event_data));
                            cJSON_Delete(event_data);
                            printf("mqttservice_senddata: ch_%d,val_%d ,minvalue_%d ,maxvalue_%d\r\n",i,valueformadc,minvalue,maxvalue);
                            MqttService::mqttservice_publishMessage(_topic,_message);
                            //i++;
                        }
                    else if(i == 5)
                        {
                            QString _topic = ini.GetString("Device.AnalogTopic6").c_str();
                            int minvalue = ini.GetLong("Device.Topic6MinValue");
                            int maxvalue = ini.GetLong("Device.Topic6MaxValue");
                            int valueformadc;
                            //QByteArray _topic("_itopic.()");
                            /*  JSON 包:
                             * {
                             *      "EdgeCtrID": "",
                             *      "Pressure": -2147483648
                             *      "EventDate":
                             * }
                             *
                             */
                            //  创建JSON 包
                            cJSON *event_data = cJSON_CreateObject();

                            //  获取当前设备系统时间
                            QDateTime current_date_time = QDateTime::currentDateTime();
                            QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm::ss.zzz");
                            QByteArray time_bytearray =  current_date.toLatin1();
                            valueformadc = (int)((maxvalue-minvalue)*(analogsendpackage.analogchanalog[5].adcvalue-13107)/(65535-13107)+minvalue);
                            QString analogdata = QString("%1").arg(valueformadc);
                            if(_topic.compare("TotalValveLow")==0)
                                {
                                    int valueout = 0;
                                    int mode = 0;

                                    mode = ini.GetLong("Device.ValueModeLow");
                                    if(mode ==0)
                                        {
                                            QString analogoutput = QString("%1").arg(valueout);
                                            cJSON_AddStringToObject(event_data, "ValueOutput", analogoutput.toUtf8().data());
                                        }
                                    if(mode ==1)
                                        {
                                            valueout = ini.GetLong("Device.ValueLowOutput");
                                            QString analogoutput = QString("%1").arg(valueout);
                                            cJSON_AddStringToObject(event_data, "ValueOutput", analogoutput.toUtf8().data());
                                        }

                                }
                            else if(_topic.compare("TotalValveHigh")==0)
                            {
                                int valueout = 0;
                                int mode = 0;

                                mode = ini.GetLong("Device.ValueModeHigh");
                                if(mode ==0)
                                    {
                                        QString analogoutput = QString("%1").arg(valueout);
                                        cJSON_AddStringToObject(event_data, "ValueOutputH", analogoutput.toUtf8().data());

                                    }
                                if(mode ==1)
                                    {
                                        valueout = ini.GetLong("Device.ValueHighOutput");
                                        QString analogoutput = QString("%1").arg(valueout);
                                        cJSON_AddStringToObject(event_data, "ValueOutputH", analogoutput.toUtf8().data());
                                    }
                            }

                            cJSON_AddStringToObject(event_data, "EdgeCtrID",_devicenumber.toLatin1().data());
                            int _topicnum = topictonum(_topic);
                            switch(_topicnum) {
                                    case 3:
                                        {
                                            cJSON_AddStringToObject(event_data, "InletFlowTemp", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:InletFlowTemp"<<analogdata;
                                        }
                                        break;
                                    case 5:
                                        {
                                            cJSON_AddStringToObject(event_data, "InletFlowTempH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:InletFlowTempH"<<analogdata;
                                        }
                                        break;

                                    case 4:
                                        {
                                            cJSON_AddStringToObject(event_data, "OutletFlowTemp", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:OutletFlowTemp"<<analogdata;
                                        }
                                        break;
                                    case 6:
                                        {
                                            cJSON_AddStringToObject(event_data, "OutletFlowTempH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:OutletFlowTempH"<<analogdata;
                                        }
                                        break;
                                    case 1:
                                        {
                                            cJSON_AddStringToObject(event_data, "Pressure", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:Pressure"<<analogdata;
                                        }
                                        break;
                                    case 2:
                                        {
                                            cJSON_AddStringToObject(event_data, "PressureH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:PressureH"<<analogdata;
                                        }
                                        break;
                                    case 7:
                                        {
                                            cJSON_AddStringToObject(event_data, "ValueFeedback", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:ValueFeedback"<<analogdata;
                                        }
                                        break;
                                    case 8:
                                        {
                                            cJSON_AddStringToObject(event_data, "ValueFeedbackH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:ValueFeedbackH"<<analogdata;
                                        }
                                        break;

                            }

                            cJSON_AddStringToObject(event_data, "EventDate", time_bytearray.data());//设备版本号"01"
                            //  字节数组转化
                            QByteArray _message(cJSON_Print(event_data));
                            cJSON_Delete(event_data);
                            printf("mqttservice_senddata: ch_%d,val_%d ,minvalue_%d ,maxvalue_%d\r\n",i,valueformadc,minvalue,maxvalue);
                            MqttService::mqttservice_publishMessage(_topic,_message);
                            //i++;
                        }
                    else if(i == 6)
                        {
                            QString _topic = ini.GetString("Device.AnalogTopic7").c_str();
                            int minvalue = ini.GetLong("Device.Topic7MinValue");
                            int maxvalue = ini.GetLong("Device.Topic7MaxValue");
                            int valueformadc;
                            //QByteArray _topic("_itopic.()");
                            /*  JSON 包:
                             * {
                             *      "EdgeCtrID": "",
                             *      "Pressure": -2147483648
                             *      "EventDate":
                             * }
                             *
                             */
                            //  创建JSON 包
                            cJSON *event_data = cJSON_CreateObject();

                            //  获取当前设备系统时间
                            QDateTime current_date_time = QDateTime::currentDateTime();
                            QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm::ss.zzz");
                            QByteArray time_bytearray =  current_date.toLatin1();
                            valueformadc = (int)((maxvalue-minvalue)*(analogsendpackage.analogchanalog[6].adcvalue-13107)/(65535-13107)+minvalue);
                            QString analogdata = QString("%1").arg(valueformadc);

                            if(_topic.compare("TotalValveLow")==0)
                                {
                                    int valueout = 0;
                                    int mode = 0;

                                    mode = ini.GetLong("Device.ValueModeLow");
                                    if(mode ==0)
                                        {
                                            QString analogoutput = QString("%1").arg(valueout);
                                            cJSON_AddStringToObject(event_data, "ValueOutput", analogoutput.toUtf8().data());
                                        }
                                    if(mode ==1)
                                        {
                                            valueout = ini.GetLong("Device.ValueLowOutput");
                                            QString analogoutput = QString("%1").arg(valueout);
                                            cJSON_AddStringToObject(event_data, "ValueOutput", analogoutput.toUtf8().data());
                                        }

                                }
                            else if(_topic.compare("TotalValveHigh")==0)
                            {
                                int valueout = 0;
                                int mode = 0;

                                mode = ini.GetLong("Device.ValueModeHigh");
                                if(mode ==0)
                                    {
                                        QString analogoutput = QString("%1").arg(valueout);
                                        cJSON_AddStringToObject(event_data, "ValueOutputH", analogoutput.toUtf8().data());
                                    }
                                if(mode ==1)
                                    {
                                        valueout = ini.GetLong("Device.ValueHighOutput");
                                        QString analogoutput = QString("%1").arg(valueout);
                                        cJSON_AddStringToObject(event_data, "ValueOutputH", analogoutput.toUtf8().data());
                                    }
                            }


                            cJSON_AddStringToObject(event_data, "EdgeCtrID",_devicenumber.toLatin1().data());
                            int _topicnum = topictonum(_topic);
                            switch(_topicnum) {
                                    case 3:
                                        {
                                            cJSON_AddStringToObject(event_data, "InletFlowTemp", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:InletFlowTemp"<<analogdata;
                                        }
                                        break;
                                    case 5:
                                        {
                                            cJSON_AddStringToObject(event_data, "InletFlowTempH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:InletFlowTempH"<<analogdata;
                                        }
                                        break;

                                    case 4:
                                        {
                                            cJSON_AddStringToObject(event_data, "OutletFlowTemp", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:OutletFlowTemp"<<analogdata;
                                        }
                                        break;
                                    case 6:
                                        {
                                            cJSON_AddStringToObject(event_data, "OutletFlowTempH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:OutletFlowTempH"<<analogdata;
                                        }
                                        break;
                                    case 1:
                                        {
                                            cJSON_AddStringToObject(event_data, "Pressure", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:Pressure"<<analogdata;
                                        }
                                        break;
                                    case 2:
                                        {
                                            cJSON_AddStringToObject(event_data, "PressureH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:PressureH"<<analogdata;
                                        }
                                        break;
                                    case 7:
                                        {
                                            cJSON_AddStringToObject(event_data, "ValueFeedback", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:ValueFeedback"<<analogdata;
                                        }
                                        break;
                                    case 8:
                                        {
                                            cJSON_AddStringToObject(event_data, "ValueFeedbackH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:ValueFeedbackH"<<analogdata;
                                        }
                                        break;

                            }

                            cJSON_AddStringToObject(event_data, "EventDate", time_bytearray.data());//设备版本号"01"
                            //  字节数组转化
                            QByteArray _message(cJSON_Print(event_data));
                            cJSON_Delete(event_data);
                            printf("mqttservice_senddata: ch_%d,val_%d ,minvalue_%d ,maxvalue_%d\r\n",i,valueformadc,minvalue,maxvalue);
                            MqttService::mqttservice_publishMessage(_topic,_message);
                            //i++;
                        }
                    else if(i == 7)
                        {
                            QString _topic = ini.GetString("Device.AnalogTopic8").c_str();
                            int minvalue = ini.GetLong("Device.Topic8MinValue");
                            int maxvalue = ini.GetLong("Device.Topic8MaxValue");
                            int valueformadc;
                            //QByteArray _topic("_itopic.()");
                            /*  JSON 包:
                             * {
                             *      "EdgeCtrID": "",
                             *      "Pressure": -2147483648
                             *      "EventDate":
                             * }
                             *
                             */
                            //  创建JSON 包
                            cJSON *event_data = cJSON_CreateObject();

                            //  获取当前设备系统时间
                            QDateTime current_date_time = QDateTime::currentDateTime();
                            QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm::ss.zzz");
                            QByteArray time_bytearray =  current_date.toLatin1();
                            valueformadc = (int)((maxvalue-minvalue)*(analogsendpackage.analogchanalog[7].adcvalue-13107)/(65535-13107)+minvalue);
                            QString analogdata = QString("%1").arg(valueformadc);
                            if(_topic.compare("TotalValveLow")==0)
                                {
                                    int valueout = 0;
                                    int mode = 0;

                                    mode = ini.GetLong("Device.ValueModeLow");
                                    if(mode ==0)
                                        {
                                            QString analogoutput = QString("%1").arg(valueout);
                                            cJSON_AddStringToObject(event_data, "ValueOutput", analogoutput.toUtf8().data());
                                        }
                                    if(mode ==1)
                                        {
                                            valueout = ini.GetLong("Device.ValueLowOutput");
                                            QString analogoutput = QString("%1").arg(valueout);
                                            cJSON_AddStringToObject(event_data, "ValueOutput", analogoutput.toUtf8().data());
                                        }

                                }
                            else if(_topic.compare("TotalValveHigh")==0)
                            {
                                int valueout = 0;
                                int mode = 0;

                                mode = ini.GetLong("Device.ValueModeHigh");
                                if(mode ==0)
                                    {
                                        QString analogoutput = QString("%1").arg(valueout);
                                        cJSON_AddStringToObject(event_data, "ValueOutputH", analogoutput.toUtf8().data());
                                    }
                                if(mode ==1)
                                    {
                                        valueout = ini.GetLong("Device.ValueHighOutput");
                                        QString analogoutput = QString("%1").arg(valueout);
                                        cJSON_AddStringToObject(event_data, "ValueOutputH", analogoutput.toUtf8().data());
                                    }
                            }

                            cJSON_AddStringToObject(event_data, "EdgeCtrID",_devicenumber.toLatin1().data());
                            int _topicnum = topictonum(_topic);
                            switch(_topicnum) {
                                    case 3:
                                        {
                                            cJSON_AddStringToObject(event_data, "InletFlowTemp", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:InletFlowTemp"<<analogdata;
                                        }
                                        break;
                                    case 5:
                                        {
                                            cJSON_AddStringToObject(event_data, "InletFlowTempH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:InletFlowTempH"<<analogdata;
                                        }
                                        break;

                                    case 4:
                                        {
                                            cJSON_AddStringToObject(event_data, "OutletFlowTemp", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:OutletFlowTemp"<<analogdata;
                                        }
                                        break;
                                    case 6:
                                        {
                                            cJSON_AddStringToObject(event_data, "OutletFlowTempH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:OutletFlowTempH"<<analogdata;
                                        }
                                        break;
                                    case 1:
                                        {
                                            cJSON_AddStringToObject(event_data, "Pressure", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:Pressure"<<analogdata;
                                        }
                                        break;
                                    case 2:
                                        {
                                            cJSON_AddStringToObject(event_data, "PressureH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:PressureH"<<analogdata;
                                        }
                                        break;
                                    case 7:
                                        {
                                            cJSON_AddStringToObject(event_data, "ValueFeedback", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:ValueFeedback"<<analogdata;
                                        }
                                        break;
                                    case 8:
                                        {
                                            cJSON_AddStringToObject(event_data, "ValueFeedbackH", analogdata.toUtf8().data());
                                            qDebug()<<"mqttservice_senddata:ValueFeedbackH"<<analogdata;
                                        }
                                        break;

                            }

                            cJSON_AddStringToObject(event_data, "EventDate", time_bytearray.data());//设备版本号"01"
                            //  字节数组转化
                            QByteArray _message(cJSON_Print(event_data));
                            cJSON_Delete(event_data);
                            printf("mqttservice_senddata: ch_%d,val_%d ,minvalue_%d ,maxvalue_%d\r\n",i,valueformadc,minvalue,maxvalue);
                            MqttService::mqttservice_publishMessage(_topic,_message);
                            analogsendpackage.flag = 0;
                            //i = 0;
                        }
                    else
                        {
                            printf("mqttservice_senddata cannt come in!\r\n");
                            res = -1;
                        }
                    }
                }
        else
        {
            res = -1;
        }

    return res;
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

ANALOG_OUTPUT_REV_STRUCT getAnalogDataFromMqtt()
{
    ANALOG_OUTPUT_REV_STRUCT analogout;

    analogout = anlaogoutput;
    printf("getAnalogDataFromMqtt 0:%d \r\n",analogout.analogoutputsingalch[0].angle);
    printf("getAnalogDataFromMqtt 1:%d \r\n",analogout.analogoutputsingalch[1].angle);
    return analogout;
}

void clearAnalogDataFlag(void)
{
    ANALOG_OUTPUT_REV_STRUCT *panalogout;

    panalogout = &anlaogoutput;

    if(panalogout->flag ==1)
        panalogout->flag=0;

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
