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
    bool updateUser(const QList <User_Message_> userList);
//    biotech::osla::UserList getAllUsers();
//    User getUserInfoByCardSerial(uint cardSerial);
//    User getUserInfoById(uint id);
//    bool getUserIsInPenaltyStop(User& user, int now);

//    /// 账户信息
//    bool updateAccount(const biotech::osla::AccountList &accountList);
//    /// 权限信息
//    bool updateUserRight(const biotech::osla::UserRightList &userRightList);
//    /// 临时卡号信息
//    bool updateTempCardBinding(const biotech::osla::TempCardBindingList &tempCardBindingList);

//    /// 专享信息
//    bool updateReservation(const biotech::osla::ReservationList &reservationList);
//    bool hasReservation(const QDate &date, biotech::osla::ReservationList &reservationList);

//    /// 委托实验信息
//    bool updateEntrust(const biotech::osla::EntrustList& entrustList);
//    /// 0 非实验中 1 实验中 2全部
//    biotech::osla::EntrustList getEntrustList(int begin, int size, std::list<int> status, int isEx);
//    /// 0 非实验中 1 实验中 2全部
//    int countValidEntrustList(std::list<int> status, int inEx);
//    bool updateEntrustStatus(const biotech::osla::Entrust& entrust);
//    bool addEntrustEvent(const biotech::osla::EntrustEvent &entrustEvent);
//    biotech::osla::EntrustEventList getEntrustEvent(uint syncTimestamp, uint count);

//    /// 预约信息
//    bool updateBooking(const biotech::osla::BookingList &bookingList);
//    biotech::osla::Booking getCurrentBooking(QDateTime currentDateTime);
//    biotech::osla::Booking getBookingById(int bookingId);
//    biotech::osla::BookingList getRecentBookingsInDays(QDateTime currentDateTime, uint days);
//    bool updateBookingStatus(const biotech::osla::Booking &booking, bool isEnd);
//    int endExpiredBookings(const QDateTime &currentDateTime);

//    /// 日历信息
//    bool updateCalendar(const biotech::osla::CalendarList &calendarList);
//    bool isHoliday(const QDate &date);

//    /// 用户事件
//    bool updateUserEvent(const biotech::osla::UserEventList &userEventList);
//    bool addUserEvent(const biotech::osla::UserEvent &userEvent);
//    bool addUserEvent(const User &user, uint actionType, uint actionResult);
//    biotech::osla::UserEventList getUserEvent(uint syncTimestamp, uint count);
//    quint64 getUserEventByUser(const User &user);
//    void getUserEvent_data();
//    /// 系统事件
//    bool updateSystemEvent(const biotech::osla::SystemEventList &systemEventList);
//    bool addSystemEvent(const biotech::osla::SystemEvent &systemEvent);
//    bool addSystemEvent(uint eventType, quint64 eventContent = 0);
//    biotech::osla::SystemEventList getSystemEvent(uint syncTimestamp, uint count);

//    /// 会话事件
//    bool updateSession(const biotech::osla::SessionList &sessionList);
//    biotech::osla::SessionList getSession(uint syncTimestamp, uint count);
//    biotech::osla::Session getLastSessionUncompleted();
//    bool addSession(biotech::osla::Session &session, bool isCompleted);
//    int endUncompletedSession();

//    /// 培训事件
//    bool updateUserInTraining(const biotech::osla::UserInTrainingList &userInTraingingList);
//    bool addUserInTraining(const biotech::osla::UserInTraining &userInTraining);
//    biotech::osla::UserInTrainingList getUserInTraining(uint syncTimestamp, uint count);
//    int userInTrainingCount(quint64 sessionId);
//    bool hasCheckinRecord(quint64 sessionId, uint cardSerial);
//    int endUserInTraining(quint64 sessionId);
//    int endUncompletedUserInTraining();

//    /// ftp上传事件
//    bool addFtpEvent(const biotech::osla::FtpEvent& ftpEvent);
//    biotech::osla::FtpEventList getFtpEvent(uint syncTimestamp, uint count);

//    bool addDeviceCaptureEvent(const biotech::osla::DeviceCaptureEvent& deviceCaptureEvent);
//    biotech::osla::DeviceCaptureEventList getDeviceCaptureEvent(uint syncTimestamp, uint count);

//    /// 设置同步信息
//    bool setSyncFlag(QString tableName, long long id, int val);
//    bool delSyncFlag(QString tableName, long long id);
//    bool setSyncFlag(long long sessionId, uint serialCard, int val);

//    /// 获取未同步记录
//    QVector<int> unsyncedRecordCount();
//    /// 删除未同步记录
//    bool deleteUnsyncedRecords();
//    /// 导出数据库记录到制定目录
//    bool exportRecordEvent(QString path);
//    /// 将未同步的数据状态(1,0)转为0
//    bool convertUnsyncRecordStatus();

//    /// 获取会话ID
//    quint64 getLastId(QString sql);
//    quint64 getNextId(quint64 lastId, uint timestamp, QString sql = "");
//    QString formatIdString(quint64 id);
//    void checkOldData(void);

//    bool updateEnumTextAndLanguage();
signals:
//    void error(QString errorString);
//    void adminUpdated();
//    void ready();
//    void error(const SqlError error);

private:
//    quint64 getLastIdLocal(QString sql);
//    quint64 getNextIdLocal(quint64 lastId, uint timestamp, QString sql = "");
//    bool addSystemEventLocal(uint eventType, quint64 eventContent = 0);
////    biotech::osla::Session getSession(long long id);
//    //删除当前时间之前sec秒的数据记录，以节省空间
//    void cleanOldData(int sec);
private:
    QSqlDatabase db;
    int instrumentId;
};

#endif // SQLWORKER_H
