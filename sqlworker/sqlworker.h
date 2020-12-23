/******************************************************************************************
*-----First Created. time: 16-November-2020	4:35  P.m.
* @File name    :sqlworker.h
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

#ifndef SQLWORKER_H
#define SQLWORKER_H

#include <QtCore/QObject>
#include <QtCore/QMutex>
#include <QtCore/QDate>
#include <QtSql/QSqlDatabase>
#include <deviceinfo/typedef.h>

#include "userservice.h"

class SqlWorker : public QObject
{
    Q_OBJECT
public:
    static SqlWorker* GetSqlWorkByThread();

private:
    SqlWorker(QObject *parent = 0, QString name = "default");
    ~SqlWorker();

public:
    enum SqlError {
        DATABASEOPENERROR
    };

    bool isReady() const;
    void closeDatabase();
    void removeDatabase();

    void createTables();
    void setInstrumentId(int instrumentId);

    /// 用户信息
    bool deleteUser(uint id);
    bool updateUser(const QList <User_Message_> userList);
    QList <User_Message_> getAllUsers() ;
    User_Message_ getUserInfoById(uint id);
    User_Message_ getUserInfoByPenal(::std::string sn);

signals:
//    void error(QString errorString);
//    void ready();
//    void error(const SqlError error);

private:
//    删除当前时间之前sec秒的数据记录，以节省空间
    void cleanOldData(int sec);
private:
    QSqlDatabase db;
    int instrumentId;
};

#endif // SQLWORKER_H
