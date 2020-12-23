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

//#include <QtNetwork/QNetworkAccessManager>
//#include <QtNetwork/QNetworkRequest>
//#include <QtNetwork/QNetworkReply>

class MainService : public QObject
{
    Q_OBJECT
public:
    explicit MainService(QObject *parent = nullptr);
    void init();

private:
    QTimer * mainTimer;

    MqttService *mqttservice;
    QMqttClient *mqtt_client;
signals:
    void service_flow(unsigned int);


signals:

private slots:
    void on_mainTimeOut();
    void on_service_flow(unsigned int);
};

#endif // MAINSERVICE_H
