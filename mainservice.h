#ifndef MAINSERVICE_H
#define MAINSERVICE_H

#include <QObject>
#include <QTimer>
#include <QDebug>
#include <QApplication>
#include <QTranslator>
#include <QTextCodec>
#include <QThread>
#include <QDateTime>

#include "mqttservice.h"
#include "analoginput.h"
#include "digitaloutput.h"
//#include <QtNetwork/QNetworkAccessManager>
//#include <QtNetwork/QNetworkRequest>
//#include <QtNetwork/QNetworkReply>

#define MQTTDELAY 300

typedef  enum _MAINSERVICE_RUN_STATE
{
    MAINSERVICE_RUN_STATE_INIT = 0,
    MAINSERVICE_RUN_STATE_MQTTSTART,
    MAINSERVICE_RUN_STATE_RUN,
    MAINSERVICE_RUN_STATE_MAX
}MAINSERVICE_RUN_STATE;

class MainService : public QObject
{
    Q_OBJECT
public:
    explicit MainService(QObject *parent = nullptr);
    void init();
    int main_Mqtt_PublishMessage(ANALOG_SEND_PACKAGE analogsendpackage);

    MAINSERVICE_RUN_STATE mainservicerunstate;

private:

    QTimer * mainTimer;
    MqttService *mqttservice;
    QMqttClient *mqtt_client;
    AnalogInput *analoginput;
    AnalogOutput *analogoutput;
    DigitalOutput *digitalOutput;

signals:
    void service_flow(unsigned int);

signals:

private slots:
    void on_mainTimeOut();
    void on_service_flow(unsigned int);
};

#endif // MAINSERVICE_H
