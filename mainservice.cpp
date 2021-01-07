#include "mainservice.h"
#include "globaldata.h"
#include "mqttservice.h"
#include "analogoutput.h"

MainService::MainService(QObject *parent) : QObject(parent)
{

}

void MainService::init(){
    mainTimer = new QTimer();
    mainTimer->start(20);

    mqttservice = new MqttService;
    mqtt_client = mqttservice->mqttservice_init(QString("standard"));

    analoginput = new AnalogInput;

    analoginput->analoginput_init();

    analogoutput =new AnalogOutput;
    analogoutput->analogoutput_init();
    digitalOutput = new DigitalOutput;
    digitalOutput->digitaloutput_init();

    connect(mainTimer,SIGNAL(timeout()),this,SLOT(on_mainTimeOut()));
    connect(this,SIGNAL(service_flow(unsigned int)),this,SLOT(on_service_flow(unsigned int)));
}

int MainService::main_Mqtt_PublishMessage(ANALOG_SEND_PACKAGE analogsendpackage)
{
    int res = 0;

    mqttservice->mqttservice_senddata(analogsendpackage);

    return res;
}

void MainService::on_service_flow(unsigned int flow)
{
    int res =0;

    switch(flow){
        case 1:{
            ANALOG_SEND_PACKAGE analogsendpackage;          

            analogsendpackage = analoginput->analoginput_get_analogdata();

            res = main_Mqtt_PublishMessage(analogsendpackage);

            for(int i=0; i< 8;i++)
            {
                printf("on_service_flow:analogsendpackage flag:%d\r\n",analogsendpackage.flag);
                printf("on_service_flow:analogsendpackage adc value:%d\r\n", analogsendpackage.analogchanalog[i].adcvalue);
            }
        }break;

        case 2:
        {
            analogoutput->analogoutput_run();
        }break;

        default:
         break;
    }
}


void MainService::on_mainTimeOut()
{
    static unsigned int timeKeeping_init;
    static unsigned int timeKeeping_run;

    timeKeeping_init++;
    timeKeeping_run++;
    if(timeKeeping_init == 750){
        timeKeeping_init = 750;

    }

    QDateTime  timeString = QDateTime::currentDateTime();//获取系统现在的时间
    QString str = timeString.toString("yyyy-MM-dd hh:mm:ss "); //设置显示格式
    //emit label_time_refresh(str);  触发mainwindow里面的显示刷新

    if(timeKeeping_run%10 == 0){
            analoginput->analoginput_run();
        }

    if(timeKeeping_run ==1000){
        timeKeeping_run =0;
        service_flow(1);
        service_flow(2);

    }


    switch (timeKeeping_init){

        case 600:{
            qDebug()<<"MQTT subscribe"<<"timeKeeping_init"<<timeKeeping_init;
            mqttservice->mqttservice_subscribeTopic(mqtt_client);
        }break;

        default:
         break;
    }

}
