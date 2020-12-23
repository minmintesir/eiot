#include "mainservice.h"

MainService::MainService(QObject *parent) : QObject(parent)
{

}

void MainService::init(){
    mainTimer = new QTimer();
    mainTimer->start(1000);

    mqttservice = new MqttService;
    mqtt_client = mqttservice->mqttservice_init(QString("standard"));

    connect(mainTimer,SIGNAL(timeout()),this,SLOT(on_mainTimeOut()));


    connect(this,SIGNAL(service_flow(unsigned int)),this,SLOT(on_service_flow(unsigned int)));
}

void MainService::on_service_flow(unsigned int flow)
{

    switch(flow){
        case 1:{

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
    if(timeKeeping_init == 30){
        timeKeeping_init = 30;
    }

    QDateTime  timeString = QDateTime::currentDateTime();//获取系统现在的时间
    QString str = timeString.toString("yyyy-MM-dd hh:mm:ss "); //设置显示格式
    //emit label_time_refresh(str);  触发mainwindow里面的显示刷新


    if(timeKeeping_run ==5){

        service_flow(1);

    }


    switch (timeKeeping_init){

        case 11:{

           qDebug()<<"MQTT subscribe"<<"timeKeeping_init"<<timeKeeping_init;
            mqttservice->mqttservice_subscribeTopic(mqtt_client);
        }break;
        case 13:{

        }break;
        case 17:{

        }break;
        case 20:{

        }break;
        default:
         break;
    }
}
