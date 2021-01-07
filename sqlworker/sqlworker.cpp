/******************************************************************************************
*-----First Created. time: 16-November-2020	4:35  P.m.
* @File name    :sqlworker.cpp
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

#include <time.h>
#include <QThread>
#include <QtCore/QtCore>
#include <QtSql/QtSql>
#include <tools/log.h>
#include "sqlworker/sqlworker.h"
#include "sqlworker/tablebaseinfo.h"
#include "deviceinfo/enum.h"
#include "deviceinfo/version.h"
#include "tools/iniparse.h"
#include "deviceinfo/globaldata.h"


#define likely(x) __builtin_expect((x),1)
#define unlikely(x) __builtin_expect((x),0)

#define ConvertQ2CString(value) value.toLocal8Bit().data()
#define ConvertC2QString(value) QString(value.c_str())


static QMutex m_Mutex;

SqlWorker *SqlWorker::GetSqlWorkByThread() {
    static std::map<Qt::HANDLE, SqlWorker *> threadSqlConnectionPool;
    static QMutex mutex;

    SqlWorker *sqlWorker = nullptr;
    auto iter = threadSqlConnectionPool.find(QThread::currentThreadId());
    if (threadSqlConnectionPool.end() == iter) {
        QMutexLocker locker(&mutex);
        auto iter = threadSqlConnectionPool.find(QThread::currentThreadId());
        if (threadSqlConnectionPool.end() == iter) {
            char temp[128] = {0};
            sprintf(temp, "%x", (uint)QThread::currentThreadId());
            sqlWorker = new SqlWorker(nullptr, temp);
            threadSqlConnectionPool.insert(std::make_pair(QThread::currentThreadId(), sqlWorker));
        }
    } else {
        sqlWorker = iter->second;
    }
    return sqlWorker;
}

SqlWorker::SqlWorker( QObject *parent, QString name ) : QObject( parent ) {
    db = QSqlDatabase::addDatabase( DATABASE_DRIVER, name );
    db.setDatabaseName( DATABASE_NAME );
    db.open();

    if (!isReady()) {
        if (!db.open()) {
            DEBUG_LOG( "SqlWorker: Database open error:" << ConvertQ2CString(db.lastError().text()) );
            return;
        }
    }
}

SqlWorker::~SqlWorker() {
    closeDatabase();
}

bool SqlWorker::isReady() const {
    return db.isOpen() && (!db.isOpenError());
}

void SqlWorker::closeDatabase() {
    db.close();
}

void SqlWorker::removeDatabase() {
    if (db.isOpen()) {
        db.close();
    }
    QFile(DATABASE_NAME).remove();
}

void SqlWorker::createTables() {
    // 用户表
    for (int i = 0; tableDefineArr[i].name != nullptr && tableDefineArr[i].def != nullptr; ++i) {
        if (!db.tables().contains(tableDefineArr[i].name)) {
            db.exec(tableDefineArr[i].def);
        }
    }
    QProcess::execute("sync");
}


bool SqlWorker::deleteUser(uint id) {
    QSqlQuery query(db);
    query.prepare("delete from TUserMessage where ID=:ID");
    query.bindValue(":ID", id);
    if (!query.exec()) {
        DEBUG_LOG("SqlWorker: Failed to update TUser:" << ConvertQ2CString(query.lastError().text()));
        db.rollback();
        return false;
    }
    DEBUG_LOG("SqlWorker: Userid " << id<< "deleted");
    return true;
}

bool SqlWorker::updateUser(const QList <User_Message_> userList) {
    QMutexLocker locker(&m_Mutex);
    DEBUG_LOG("Parse Server Command : [" << __FUNCTION__ << "]");

    {
        QSqlQuery query(db);
        db.transaction();
        for (int i = 0; i < userList.size(); i++) {
            DEBUG_LOG("update user info : " << userList.at(i).userid << "," << userList.at(i).buildingnumber << ","
                                            << userList.at(i).roomnumber << ","<< userList.at(i).panelsn << ","
                                            << userList.at(i).adapteraddr << ","<< userList.at(i).heatmeteraddr << ","
                                            << userList.at(i).controlvalveaddr);

            query.prepare("replace into TUserMessage(UserID,BuildingNumber,RoomNumber,PanelSN,"
                                  "AdapterAddr,HeatMeterAddr,ControlValveAddr) "
                                  "values(:UserID,:BuildingNumber,:RoomNumber,:PanelSN,"
                                  ":AdapterAddr,:HeatMeterAddr,:ControlValveAddr)");
            query.bindValue(":UserID", (uint) userList.at(i).userid);
            query.bindValue(":BuildingNumber", QString(userList.at(i).buildingnumber.c_str()));
            query.bindValue(":RoomNumber",  QString(userList.at(i).roomnumber.c_str()));
            query.bindValue(":PanelSN",  QString(userList.at(i).panelsn.c_str()));
            query.bindValue(":AdapterAddr",  QString(userList.at(i).adapteraddr.c_str()));
            query.bindValue(":HeatMeterAddr",  QString(userList.at(i).heatmeteraddr.c_str()));
            query.bindValue(":ControlValveAddr",  QString(userList.at(i).controlvalveaddr.c_str()));

            if (!query.exec()) {
                DEBUG_LOG("SqlWorker: Failed to update TUser:" << ConvertQ2CString(query.lastError().text()));
                db.rollback();
                return false;
            }

            DEBUG_LOG("update useid is : " << userList.at(i).userid
                      << ", buildingnumber is :"<< userList.at(i).buildingnumber
                      << ", roomnumber is :"<< userList.at(i).roomnumber);
        }
    }
    db.commit();
    QProcess::execute("sync");
    return true;
}

QList <User_Message_> SqlWorker::getAllUsers() {
    QMutexLocker locker(&m_Mutex);
    QSqlQuery query(db);
    query.exec("select * from TUserMessage order by UpdateTime desc");
    QList <User_Message_> userList;
    User_Message_ user;
    while (query.next()) {
        user.userid = query.value(0).toInt();
        user.buildingnumber = query.value(1).toByteArray().data();
        user.roomnumber = query.value(2).toByteArray().data();
        user.panelsn = query.value(3).toByteArray().data();
        user.adapteraddr = query.value(4).toByteArray().data();
        user.heatmeteraddr = query.value(5).toByteArray().data();
        user.controlvalveaddr = query.value(6).toByteArray().data();
        userList.append(user);
    }
    query.finish();
    return userList;
}

User_Message_ SqlWorker::getUserInfoById(uint id) {
    QMutexLocker locker(&m_Mutex);
    User_Message_ user;
    user.userid = 0;
    QSqlQuery query(db);

    query.prepare("select UserID,BuildingNumber,RoomNumber,PanelSN,AdapterAddr,HeatMeterAddr,ControlValveAddr"
                          "from (select * from TUserMessage where ID=:ID)"
                  );
    query.bindValue(":ID", id);

    if (!query.exec()) {
        DEBUG_LOG("SqlWorker: Failed to select from TUser:"
                          << ConvertQ2CString(query.lastError().text()));
        query.finish();
        return user;
    }
    if (query.first()) {
        user.userid = query.value(0).toInt();
        user.buildingnumber = query.value(1).toByteArray().data();
        user.roomnumber = query.value(2).toByteArray().data();
        user.panelsn = query.value(3).toByteArray().data();
        user.adapteraddr = query.value(4).toByteArray().data();
        user.heatmeteraddr = query.value(5).toByteArray().data();
        user.controlvalveaddr = query.value(6).toByteArray().data();
    }
    query.finish();
    return user;
}

User_Message_ SqlWorker::getUserInfoByPenal(::std::string sn) {
    QMutexLocker locker(&m_Mutex);
    User_Message_ user;
    user.userid = 0;
    QSqlQuery query(db);

    query.prepare("select UserID,BuildingNumber,RoomNumber,PanelSN,AdapterAddr,HeatMeterAddr,ControlValveAddr"
                          "from (select * from TUserMessage where PanelSN=:PanelSN)"
                  );
    query.bindValue(":PanelSN", QString(sn.c_str()));

    if (!query.exec()) {
        DEBUG_LOG("SqlWorker: Failed to select from TUserMessage:"
                          << ConvertQ2CString(query.lastError().text()));
        query.finish();
        return user;
    }
    if (query.first()) {
        user.userid = query.value(0).toInt();
        user.buildingnumber = query.value(1).toByteArray().data();
        user.roomnumber = query.value(2).toByteArray().data();
        user.panelsn = query.value(3).toByteArray().data();
        user.adapteraddr = query.value(4).toByteArray().data();
        user.heatmeteraddr = query.value(5).toByteArray().data();
        user.controlvalveaddr = query.value(6).toByteArray().data();
    }
    query.finish();
    return user;
}

void SqlWorker::cleanOldData(int sec)
{
    QSqlQuery query(db);

    QString sql;

    int currentTime = QDateTime::currentDateTime().toTime_t();

    if (sec > currentTime)
    {
        return;
    }
    int beforeTime = currentTime - sec;
    //delete old TUserEvent
    sql = QString("delete from TUserEvent where (SyncTime < %1) and (SyncTime <> 0)").arg(beforeTime);
    query.exec(sql);

    //delete old TBookingEntrustEvent
    sql = QString("delete from TBookingEntrustEvent where (SyncTime < %1) and (SyncTime <> 0)").arg(beforeTime);
    query.exec(sql);

    //delete old TDeviceCaptureEvent
    sql = QString("delete from TDeviceCaptureEvent where (SyncTime < %1) and (SyncTime <> 0)").arg(beforeTime);
    query.exec(sql);

    //delete old TFtpEvent
    sql = QString("delete from TFtpEvent where (SyncTime < %1) and (SyncTime <> 0)").arg(beforeTime);
    query.exec(sql);

    //delete old TSystemEvent
    sql = QString("delete from TSystemEvent where (SyncTime < %1) and (SyncTime <> 0)").arg(beforeTime);
    query.exec(sql);

    //delete old TSession
    sql = QString("delete from TSession where (SyncTime < %1) and (SyncTime <> 0)").arg(beforeTime);
    query.exec(sql);

    //delete old TBooking
    sql = QString("delete from TBooking where EndTime < %1").arg(beforeTime);
    query.exec(sql);
}
