#ifndef MQTTSERVICE_H
#define MQTTSERVICE_H
//
#include <QObject>
#include <QtCore/QDateTime>
#include <QtMqtt/qmqttclient.h>

#include <QtWidgets/QMessageBox>

#include <QTcpSocket>
#include "tools/log.h"
#include "tools/iniparse.h"
#include "deviceinfo/version.h"
#include "tools/macian_json.h"


class MqttService : public QObject
{
    Q_OBJECT
public:
    explicit MqttService(QObject *parent = nullptr);
    ~MqttService();
    //mqtt初始化
    QMqttClient* mqttservice_init(QString str);
    //mqtt发布消息 "{\"work_state\":0,\"target_temperature\":0,\"current_temperature\":0}"
    void mqttservice_publishMessage(QString _topicname,QByteArray _message);
    void mqttservice_subscribeTopic(QMqttClient *mqtt_client);
    void mqttservice_requestPing(QMqttClient *mqtt_client);
    void mqttservice_reconnected(QString str);
private:
    //支持同时连接多个服务器，保存服务器名称
    QString service_name;
    //mqtt登录信息
    QString _hostname;
    quint16 _port;
    QString _clientid;
    QString _username;
    QString _password;
    quint8 _sub_qos;
    quint8 _pub_qos;
    bool _retain;
    QString _topic_str;

    QMqttClient *m_client;
    QMqttSubscription *m_sub;
    QMqttTopicFilter _topicname = QString("CommandMessage");
signals:

private slots:
    void on_pingResponse();
    void on_brokerDisconnected();
    void on_subscribeParse(const QByteArray &message, const QMqttTopicName &topic);
};

#endif // MQTTSERVICE_H
