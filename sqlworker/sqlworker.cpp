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
            sprintf(temp, "%x", QThread::currentThreadId());
            sqlWorker = new SqlWorker(nullptr, temp);
            threadSqlConnectionPool.insert(std::make_pair(QThread::currentThreadId(), sqlWorker));
        }
    } else {
        sqlWorker = iter->second;
    }
    return sqlWorker;
}

SqlWorker::SqlWorker(QObject *parent, QString name) : QObject(parent) {
    db = QSqlDatabase::addDatabase(DATABASE_DRIVER, name);
    db.setDatabaseName(DATABASE_NAME);
    db.open();

    if (!isReady()) {
        if (!db.open()) {
            DEBUG_LOG("SqlWorker: Database open error:" << ConvertQ2CString(db.lastError().text()));
            return;
        }
    }
//    IniParse ini(EIOT_CONFIG);
//    instrumentId = (int) ini.GetLong("Instrument.ID");
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


//bool SqlWorker::deleteUser(const QList <User_Message_> userList) {

//    query.prepare("delete from TUser where ID=:ID");
//    query.bindValue(":ID", (uint) userList.user(i).id());
//    if (!query.exec()) {
//        DEBUG_LOG("SqlWorker: Failed to update TUser:" << ConvertQ2CString(query.lastError().text()));
//        db.rollback();
//        return false;
//    }
//    DEBUG_LOG("SqlWorker: User" << (uint) userList.user(i).id() << "deleted");

//}
bool SqlWorker::updateUser(const QList <User_Message_> userList) {
    QMutexLocker locker(&m_Mutex);

    DEBUG_LOG("Parse Server Command : [" << __FUNCTION__ << "]");
    bool isAdminUpdated = false;
    uint adminIndex = 0;
    IniParse ini(EIOT_CONFIG);
    uint adminId = ini.GetLong("Instrument.AdminID");

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
            query.bindValue(":BuildingNumber", QString(userList.at(i).buildingnumber->c_str()));
            query.bindValue(":RoomNumber",  QString(userList.at(i).roomnumber->c_str()));
            query.bindValue(":PanelSN",  QString(userList.at(i).panelsn->c_str()));
            query.bindValue(":AdapterAddr",  QString(userList.at(i).adapteraddr->c_str()));
            query.bindValue(":HeatMeterAddr",  QString(userList.at(i).heatmeteraddr->c_str()));
            query.bindValue(":ControlValveAddr",  QString(userList.at(i).controlvalveaddr->c_str()));

            if (!query.exec()) {
                DEBUG_LOG("SqlWorker: Failed to update TUser:" << ConvertQ2CString(query.lastError().text()));
                db.rollback();
                return false;
            }

            DEBUG_LOG("update useid is : " << userList.at(i).userid
                      << ", buildingnumber is :"<< userList.at(i).buildingnumber
                      << ", roomnumber is :"<< userList.at(i).roomnumber);

            if ((uint) userList.at(i).userid == adminId) {
                adminIndex = i;
                isAdminUpdated = true;
            }
        }
    }
    db.commit();

//    DEBUG_LOG("SqlWorker: TUser updated");
//    if (true == isAdminUpdated) {
//        QString lang = ini.GetString("Global.Language").c_str();
//        if (lang.isEmpty()) {
//            lang = "en_US";
//        }

//        QString adminFullName;
//        if (lang.startsWith("zh_")) {
//            adminFullName = (userList.user(adminIndex).first_name() + userList.user(adminIndex).last_name()).c_str();
//        } else {
//            adminFullName = (userList.user(adminIndex).first_name() + " " +
//                             userList.user(adminIndex).last_name()).c_str();
//        }

//        ini.Set("Instrument.AdminFullName", adminFullName.toStdString());
//        ini.Set("Instrument.AdminPhone", userList.user(adminIndex).phone());
//        DEBUG_LOG("Update Admin Info : Name[" << adminFullName.toLocal8Bit().data() << "], Phone[" <<
//                                              QString(userList.user(adminIndex).phone().c_str()).toLocal8Bit().data()
//                                              << "], User Id :[" << adminId << "]");

//        ini.SaveFile();

//        //emit adminUpdated();
//        DEBUG_LOG("SqlWorker: Admin info updated");
//    }
//    QProcess::execute("sync");
//    return true;
}

//biotech::osla::UserList SqlWorker::getAllUsers() {
//    QMutexLocker locker(&m_Mutex);
//    QSqlQuery query(db);
//    query.exec("select * from TUser order by UpdateTime desc");
//    biotech::osla::UserList userList;
//    biotech::osla::User *user;
//    while (query.next()) {
//        user = userList.add_user();
//        user->set_id(query.value(0).toInt());
//        user->set_status(query.value(1).toInt());
//        user->set_first_name(query.value(2).toByteArray().data());
//        user->set_last_name(query.value(3).toByteArray().data());
//        user->set_phone(query.value(4).toByteArray().data());
//        user->set_card_serial(ConvertQ2CString(query.value(5).toString()));
//        user->set_account_id(query.value(6).toInt());
//        user->set_isblack(query.value(7).toInt());
//    }
//    query.finish();
//    return userList;
//}

//User SqlWorker::getUserInfoByCardSerial(uint c) {
//    QMutexLocker locker(&m_Mutex);
//    QSqlQuery query(db);
//    uint currentTime = QDateTime::currentDateTime().toTime_t();

//    char temp[128] = {0};
//    sprintf(temp, "%u", c);
//    QString cardSerial((const char *) temp);

//    User user;
//    user.id = biotech::osla::UNKNOWN_USER_ID; // 缺省为未知用户
//    user.right = 0;
//    user.longTimeRight = 0;
//    user.account.id = 0;
//    user.account.groupId = 0;
//    user.card.serial = std::string(temp);
//    user.card.type = biotech::osla::CARD_TYPE_UNKNOWN; // 缺省为未知卡
//    user.card.status = 0;
//    user.timestamp = currentTime;
//    user.isInBlacklist = false;

//    // 通过卡号查询用户表
//    query.prepare("select u.ID, u.Status, u.FirstName, u.LastName, u.Phone, u.AccountID, u.IsInBlack,"
//                          "(case when ur.RightTemp != ur.Right then ur.RightTemp else ur.Right end),"
//                          "ur.RightTempExpireTime, a.Status, a.GroupID, a.Deposit,"
//                          "a.Credit, ur.Right as longTimeRight "
//                          "from (select * from TUser where CardSerial=:CardSerial) as u "
//                          "left join TUserRight as ur on ur.UserID=u.ID "
//                          "left join TAccount as a on a.ID=u.AccountID");
////    query.prepare("select u.ID,u.Status,u.FirstName,u.LastName,u.Phone,u.AccountID,u.IsInBlack,"
////               --         "(case when ur.RightTempExpireTime>:CurrentTime1 then ur.RightTemp else ur.Right end),"
////                          "ur.RightTempExpireTime,a.Status,a.GroupID,a.Deposit,"
////               --         "(case when a.CreditTempExpireTime>:CurrentTime2 then a.CreditTemp else a.Credit end), "
////                          "ur.Right as longTimeRight "
////                          "from (select * from TUser where CardSerial=:CardSerial) as u "
////                          "left join TUserRight as ur on ur.UserID=u.ID "
////                          "left join TAccount as a on a.ID=u.AccountID");
//    query.bindValue(":CardSerial", cardSerial);
////    query.bindValue(":CurrentTime1", currentTime);
////    query.bindValue(":CurrentTime2", currentTime);
//    if (!query.exec()) { // 查询失败
//        DEBUG_LOG("SqlWorker: Failed to select from TUser:" << ConvertQ2CString(query.lastError().text()));
//        user.id = 0;
//        query.finish();
//        return user;
//    }

//    // 查询用户表无记录，查临时卡绑定表
//    if (!query.first()) {
//        query.finish();
//        query.prepare("select UserID,ActiveTime,ExpireTime from TTempCardBinding where CardSerial=:CardSerial");
//        query.bindValue(":CardSerial", cardSerial);
//        if (!query.exec()) { // 查询失败
//            DEBUG_LOG("SqlWorker: Failed to select from TTempCardBinding:"
//                              << ConvertQ2CString(query.lastError().text()));
//            user.id = 0;
//            query.finish();
//            return user;
//        }
//        if (!query.first()) { // 查询临时卡绑定表无记录，该卡为未知卡
//            DEBUG_LOG("SqlWorker: No user record is found");
//            user.id = biotech::osla::UNKNOWN_USER_ID;
//            query.finish();
//            return user;
//        }
//        if (query.value(1).toUInt() > query.value(2).toUInt()) { // 临时卡绑定生效时间大于过期时间，该记录非法
//            DEBUG_LOG("SqlWorker: Invalid TTempCardBinding record");
//            user.id = 0;
//            query.finish();
//            return user;
//        }
//        user.id = query.value(0).toUInt(); // 获取临时卡对应的用户ID
//        user.right = biotech::osla::USER_RIGHT_SENIOR; // 临时卡用户资格为资深
//        user.card.type = biotech::osla::CARD_TYPE_TEMPORARY; // 设置卡类型为临时卡
//        if (currentTime < query.value(1).toUInt()) { // 临时卡绑定未生效
//            user.card.status = 1;
//            query.finish();
//            return user;
//        } else if (currentTime >= query.value(2).toUInt()) { // 临时卡绑定已过期
//            user.card.status = 2;
//            query.finish();
//            return user;
//        }
//        query.finish();

//        // 通过用户ID再次查询用户表
//        query.prepare("select u.ID,u.Status,u.FirstName,u.LastName,u.Phone,u.AccountID,u.IsInBlack,"
//                              "(case when ur.RightTemp != ur.Right then ur.RightTemp else ur.Right end),"
//                              "ur.RightTempExpireTime,a.Status,a.GroupID,a.Deposit,"
//                              "a.Credit, ur.Right as longTimeRight "
//                              "from (select * from TUser where ID=:ID) as u "
//                              "left join TUserRight as ur on ur.UserID=u.ID "
//                              "left join TAccount as a on a.ID=u.AccountID");
//        query.bindValue(":ID", user.id);
//        query.bindValue(":CurrentTime", currentTime);
//        if (!query.exec()) { // 查询失败
//            DEBUG_LOG("SqlWorker: Failed to select from TUser:" << ConvertQ2CString(query.lastError().text()));
//            user.id = 0;
//            query.finish();
//            return user;
//        }
//        if (!query.first()) { // 查询用户表无记录（可能用户数据未同步），做未知卡处理
//            DEBUG_LOG("SqlWorker: No user record is found");
//            user.id = biotech::osla::UNKNOWN_USER_ID;
//            query.finish();
//            return user;
//        }
//    } else {   ///
//        user.id = query.value(0).toUInt();
//        user.right = query.value(7).toUInt();
//        user.rightExpireTime = query.value(8).toUInt();
//        switch (user.right) {
//            case 0: // 查不到
//                user.right = biotech::osla::USER_RIGHT_UNAUTH; // 不在资格表里的用户为未授权用户
//                user.card.type = biotech::osla::CARD_TYPE_USER; // 对未授权、普通、资深用户，设置卡类型为用户卡
//                break;
//            case biotech::osla::USER_RIGHT_UNAUTH: // 未授权
//            case biotech::osla::USER_RIGHT_NORMAL: // 普通
//            case biotech::osla::USER_RIGHT_SENIOR: // 资深
//                user.card.type = biotech::osla::CARD_TYPE_USER; // 对未授权、普通、资深用户，设置卡类型为用户卡
//                break;
//            case biotech::osla::USER_RIGHT_ADMIN:
//            case biotech::osla::USER_RIGHT_ADMIN_MAIN:
//                user.card.type = biotech::osla::CARD_TYPE_ADMIN; // 对仪器管理员、主要仪器管理员，设置卡类型为管理员卡
//                break;
//        }
//    }
//    user.status = query.value(1).toUInt();
//    user.firstName = query.value(2).toString();
//    user.lastName = query.value(3).toString();
//    user.phone = query.value(4).toString();
//    user.isInBlacklist = query.value(6).toUInt() > 0 ? true : false;
//    user.account.id = query.value(5).toUInt();
//    user.account.status = query.value(9).toUInt();
//    user.account.groupId = query.value(10).toUInt();
//    user.account.deposit = query.value(11).toUInt();
//    user.account.credit = query.value(12).toUInt();
//    user.longTimeRight = query.value(13).toInt();
//    query.finish();

//    IniParse ini(OSLA_CONFIG);
//    QString lang = ini.GetString("Global.Language").c_str();

//    if (lang.startsWith("zh_")) {
//        user.fullName = user.firstName + user.lastName;
//    } else {
//        user.fullName = user.firstName + " " + user.lastName;
//    }

//    DEBUG_LOG("use user right is : " << user.right << ", long time right is : " << user.longTimeRight);
//    return user;
//}

//User SqlWorker::getUserInfoById(uint id) {
//    QMutexLocker locker(&m_Mutex);
//    User user;
//    user.id = 0;
//    uint now_t = QDateTime::currentDateTime().toTime_t();
//    QSqlQuery query(db);
////    query.prepare("select u.ID,u.Status,u.FirstName,u.LastName,u.Phone,u.CardSerial,u.AccountID,u.IsInBlack,"
////                          "(case when ur.RightTempExpireTime>:Timestamp1 then ur.RightTemp else ur.Right end),"
////                          "ur.RightTempExpireTime,a.Status,a.GroupID,a.Deposit,"
////                          "(case when a.CreditTempExpireTime>:Timestamp2 then a.CreditTemp else a.Credit end) "
////                          "from (select * from TUser where ID=:ID) as u "
////                          "left join TUserRight as ur on ur.UserID=u.ID "
////                          "left join TAccount as a on a.ID=u.AccountID");
//    query.prepare("select u.ID,u.Status,u.FirstName,u.LastName,u.Phone,u.CardSerial,u.AccountID,u.IsInBlack,"
//                          "(case when ur.RightTemp != ur.Right then ur.RightTemp else ur.Right end),"
//                          "ur.RightTempExpireTime,a.Status,a.GroupID,a.Deposit,"
//                          "a.Credit, ur.Right as longTimeRight "
//                          "from (select * from TUser where ID=:ID) as u "
//                          "left join TUserRight as ur on ur.UserID=u.ID "
//                          "left join TAccount as a on a.ID=u.AccountID");
//    query.bindValue(":ID", id);
////    query.bindValue(":Timestamp1", now_t);
////    query.bindValue(":Timestamp2", now_t);
//    if (!query.exec()) {
//        DEBUG_LOG("SqlWorker: Failed to select from TUser:"
//                          << ConvertQ2CString(query.lastError().text()));
//        query.finish();
//        return user;
//    }
//    if (query.first()) {
//        user.id = query.value(0).toUInt();
//        user.status = query.value(1).toUInt();
//        user.firstName = query.value(2).toString();
//        user.lastName = query.value(3).toString();
//        user.phone = query.value(4).toString();
//        user.card.serial = ConvertQ2CString(query.value(5).toString());
//        user.account.id = query.value(6).toUInt();
//        user.isInBlacklist = query.value(7).toUInt() > now_t ? true : false;
//        user.right = query.value(8).toUInt();
//        user.rightExpireTime = query.value(9).toUInt();
//        user.account.status = query.value(10).toUInt();
//        user.account.groupId = query.value(11).toUInt();
//        user.account.deposit = query.value(12).toUInt();
//        user.account.credit = query.value(13).toUInt();
//        user.longTimeRight = query.value(14).toUInt();
//    }
//    switch (user.right) {
//        case 0: // 查不到
//            user.right = biotech::osla::USER_RIGHT_UNAUTH; // 不在资格表里的用户为未授权用户
//            user.card.type = biotech::osla::CARD_TYPE_USER; // 对未授权、普通、资深用户，设置卡类型为用户卡
//            break;
//        case biotech::osla::USER_RIGHT_UNAUTH: // 未授权
//        case biotech::osla::USER_RIGHT_NORMAL: // 普通
//        case biotech::osla::USER_RIGHT_SENIOR: // 资深
//            user.card.type = biotech::osla::CARD_TYPE_USER; // 对未授权、普通、资深用户，设置卡类型为用户卡
//            break;
//        case biotech::osla::USER_RIGHT_ADMIN:
//        case biotech::osla::USER_RIGHT_ADMIN_MAIN:
//            user.card.type = biotech::osla::CARD_TYPE_ADMIN; // 对仪器管理员、主要仪器管理员，设置卡类型为管理员卡
//            break;
//    }
//    query.finish();

//    IniParse ini(OSLA_CONFIG);
//    QString lang = ini.GetString("Global.Language").c_str();
//    if (lang.isEmpty()) {
//        lang = "en_US";
//    }

//    if (lang.startsWith("zh_")) {
//        user.fullName = user.firstName + user.lastName;
//    } else {
//        user.fullName = user.firstName + " " + user.lastName;
//    }

//    return user;
//}

//bool SqlWorker::updateEnumTextAndLanguage() {
//    QMutexLocker locker(&m_Mutex);
//    DEBUG_LOG("Parse Server Command : [" << __FUNCTION__);

//    IniParse osla(OSLA_CONFIG);
//    GlobalData::currentLocale = osla.GetString("Global.Language").c_str();
//    qDebug()<< "updateAccount" << osla.GetString("Global.Language").c_str();;

//    db.exec("drop table TEnumText");

//    if (GlobalData::currentLocale.isEmpty()) {
//        GlobalData::currentLocale = "en_US";
//    }

//    db.exec( "create table TEnumText(ID INTEGER PRIMARY KEY, Text TEXT)");
//    db.transaction();

//    if(QString::compare(GlobalData::currentLocale,"zh_CN") == 0){
//        for (int j = 0; nullptr != enumDefineArr[j]; ++j) {
//            db.exec(enumDefineArr[j]);
//        }
//    }
//    else{
//        for (int j = 0; nullptr != enumDefineArr_en[j]; ++j) {
//            db.exec(enumDefineArr_en[j]);
//        }
//    }
//    db.commit();

//    DEBUG_LOG("SqlWorker: TEnumText updated");
//    QProcess::execute("sync");
//    return true;
//}

//bool SqlWorker::updateAccount(const biotech::osla::AccountList &accountList) {
//    QMutexLocker locker(&m_Mutex);
//    DEBUG_LOG("Parse Server Command : [" << __FUNCTION__);
//    QSqlQuery query(db);

//    db.transaction();
//    for (int i = 0; i < accountList.accout_size(); i++) {
//        query.prepare("replace into TAccount(ID,Status,GroupID,Deposit,Credit,CreditTemp,CreditTempExpireTime) "
//                              "values(:ID,:Status,:GroupID,:Deposit,:Credit,:CreditTemp,:CreditTempExpireTime)");
//        query.bindValue(":ID", (uint) accountList.accout(i).id());
//        query.bindValue(":Status", (uint) accountList.accout(i).status());
//        query.bindValue(":GroupID", (uint) accountList.accout(i).group_id());
//        query.bindValue(":Deposit", accountList.accout(i).deposit());
//        query.bindValue(":Credit", accountList.accout(i).credit());
//        query.bindValue(":CreditTemp", accountList.accout(i).credit_temp());
//        query.bindValue(":CreditTempExpireTime", accountList.accout(i).credit_temp_expire_time());
//        if (!query.exec()) {
//            ERROR_LOG("SqlWorker: Failed to update TAccount:" << ConvertQ2CString(query.lastError().text()));
//            db.rollback();
//            return false;
//        }
//    }
//    db.commit();
//    DEBUG_LOG("SqlWorker: TAccount updated");
//    QProcess::execute("sync");
//    return true;
//}

//bool SqlWorker::updateUserRight(const biotech::osla::UserRightList &userRightList) {
//    QMutexLocker locker(&m_Mutex);
//    DEBUG_LOG("Parse Server Command : [" << __FUNCTION__ << "]");

//    bool isAdminChanged = false;
//    uint newAdminId = 0;
//    IniParse osla(OSLA_CONFIG);
//    uint adminId = osla.GetLong("Instrument.AdminID");

//    {
//        QSqlQuery query(db);
//        db.transaction();
//        for (int i = 0; i < userRightList.user_right_size(); i++) {
//            query.prepare("replace into TUserRight(UserID,Right,RightTemp,RightTempExpireTime) "
//                                  "values(:UserID,:Right,:RightTemp,:RightTempExpireTime)");
//            query.bindValue(":UserID", (uint) userRightList.user_right(i).user_id());
//            query.bindValue(":Right", (uint) userRightList.user_right(i).right());
//            query.bindValue(":RightTemp", (uint) userRightList.user_right(i).right_temp());
//            DEBUG_LOG("Right Temp is : " << (uint) userRightList.user_right(i).right_temp());
//            //query.bindValue(":RightTempExpireTime", (uint) userRightList.user_right(i).right_temp_expire_time());
//            //query.bindValue(":RightTemp", 0);
//            query.bindValue(":RightTempExpireTime", 0);
//            if (!query.exec()) {
//                DEBUG_LOG("SqlWorker: Failed to update TUserRight:" << ConvertQ2CString(query.lastError().text()));
//                db.rollback();
//                return false;
//            }
//            if (unlikely((uint) userRightList.user_right(i).user_id() == adminId &&
//                         (uint) userRightList.user_right(i).right() != biotech::osla::USER_RIGHT_ADMIN_MAIN)) {
//                newAdminId = 0;
//                isAdminChanged = true;
//            }
//            if ((uint) userRightList.user_right(i).right() == biotech::osla::USER_RIGHT_ADMIN_MAIN) { // 仪器主要管理员
//                newAdminId = (uint) userRightList.user_right(i).user_id();
//                if (newAdminId != adminId) { // 仪器主要管理员变更
//                    adminId = newAdminId;
//                    isAdminChanged = true;
//                }
//            }
//        }
//        db.commit();
//    }

//    DEBUG_LOG("SqlWorker: TUserRight updated");
//    if (true == isAdminChanged) {
//        osla.Set("Instrument.AdminID", (long) newAdminId);
//        DEBUG_LOG("SqlWorker: Admin is changed to" << newAdminId);
//        {
//            QSqlQuery query(db);
//            query.prepare("select FirstName,LastName,Phone from TUser where ID=:AdminID");
//            query.bindValue(":AdminID", adminId);
//            if (query.exec()) {
//                if (query.next()) {
//                    QString lang = osla.GetString("Global.Language").c_str();
//                    if (lang.isEmpty()) {
//                        lang = "en_US";
//                    }

//                    QString adminFullName;
//                    if (lang.startsWith("zh_")) {
//                        adminFullName = query.record().value(0).toString() + query.record().value(1).toString();
//                    } else {
//                        adminFullName = query.record().value(0).toString() + " " + query.record().value(1).toString();
//                    }
//                    osla.Set("Instrument.AdminFullName", adminFullName.toStdString());
//                    osla.Set("Instrument.AdminPhone", query.record().value(2).toString().toStdString());
//                    DEBUG_LOG("SqlWorker: Admin's information updated");
//                }
//            }
//            osla.SaveFile();
//            query.finish();
//        }
//        //emit adminUpdated();
//    }
//    QProcess::execute("sync");
//    return true;
//}

//bool SqlWorker::updateTempCardBinding(const biotech::osla::TempCardBindingList &tempCardBindingList) {
//    QMutexLocker locker(&m_Mutex);
//    DEBUG_LOG("Parse Server Command : [" << __FUNCTION__);
//    QSqlQuery query(db);

//    db.transaction();
//    for (int i = 0; i < tempCardBindingList.temp_card_binding_size(); i++) {
//        query.prepare("replace into TTempCardBinding(CardSerial,UserID,ActiveTime,ExpireTime) "
//                              "values(:CardSerial,:UserID,:ActiveTime,:ExpireTime)");
//        query.bindValue(":CardSerial", QString(tempCardBindingList.temp_card_binding(i).card_serial().c_str()));
//        query.bindValue(":UserID", (uint) tempCardBindingList.temp_card_binding(i).user_id());
//        query.bindValue(":ActiveTime", (uint) tempCardBindingList.temp_card_binding(i).active_time());
//        query.bindValue(":ExpireTime", (uint) tempCardBindingList.temp_card_binding(i).expire_time());
//        if (!query.exec()) {
//            DEBUG_LOG("SqlWorker: Failed to update TTempCardBinding:" << ConvertQ2CString(query.lastError().text()));
//            db.rollback();
//            return false;
//        }
//    }
//    db.commit();
//    DEBUG_LOG("SqlWorker: TTempCardBinding updated");
//    QProcess::execute("sync");
//    return true;
//}

//bool SqlWorker::updateReservation(const biotech::osla::ReservationList &reservationList) {
//    QMutexLocker locker(&m_Mutex);
//    DEBUG_LOG("Parse Server Command : [" << __FUNCTION__);
//    QSqlQuery query(db);

//    db.transaction();
//    for (int i = 0; i < reservationList.reservation_size(); i++) {
//        if (reservationList.reservation(i).has_delete_time() &&
//            (uint) reservationList.reservation(i).delete_time() != 0) {
//            query.prepare("delete from TReservation where ID=:ID");
//            query.bindValue(":ID", (uint) reservationList.reservation(i).id());
//            if (!query.exec()) {
//                ERROR_LOG("SqlWorker: Failed to update TReservation:"
//                                  << ConvertQ2CString(query.lastError().text()));
//                db.rollback();
//                return false;
//            }
//        } else {
//            query.prepare("replace into TReservation(ID,GroupID,Week,StartTime,EndTime,ActiveTime,ExpireTime) "
//                                  "values(:ID,:GroupID,:Week,:StartTime,:EndTime,:ActiveTime,:ExpireTime)");
//            query.bindValue(":ID", (uint) reservationList.reservation(i).id());
//            query.bindValue(":GroupID", (uint) reservationList.reservation(i).group_id());
//            query.bindValue(":Week", (uint) reservationList.reservation(i).week());
//            query.bindValue(":StartTime", (uint) reservationList.reservation(i).start_time());
//            query.bindValue(":EndTime", (uint) reservationList.reservation(i).end_time());
//            query.bindValue(":ActiveTime", (uint) reservationList.reservation(i).active_time());
//            query.bindValue(":ExpireTime", (uint) reservationList.reservation(i).expire_time());
//            if (!query.exec()) {
//                ERROR_LOG("SqlWorker: Failed to update TReservation:" << ConvertQ2CString(query.lastError().text()));
//                db.rollback();
//                return false;
//            }
//        }
//    }
//    db.commit();
//    QProcess::execute("sync");
//    DEBUG_LOG("SqlWorker: TReservation updated");
//    return true;
//}

//bool SqlWorker::hasReservation(const QDate &date, biotech::osla::ReservationList &reservationList) {
//    QMutexLocker locker(&m_Mutex);
//    reservationList.clear_reservation();
//    int week = date.dayOfWeek();

//    QSqlQuery query(db);
//    query.prepare("select * from TReservation "
//                          " where Week=:Week and ActiveTime<=strftime('%s','now') "
//                          "   and ExpireTime>strftime('%s','now') "
//                          " order by StartTime");
//    query.bindValue(":Week", week);

//    if (!query.exec()) {
//        DEBUG_LOG("SqlWorker: Failed to select from TReservation:"
//                          << ConvertQ2CString(query.lastError().text()));
//        query.finish();
//        return false;
//    }

//    biotech::osla::Reservation *reservation;
//    while (query.next()) {
//        reservation = reservationList.add_reservation();
//        reservation->set_id(query.value(0).toInt());
//        reservation->set_group_id(query.value(1).toInt());
//        reservation->set_week(query.value(2).toInt());
//        reservation->set_start_time(query.value(3).toInt());
//        reservation->set_end_time(query.value(4).toInt());
//        reservation->set_active_time(query.value(5).toInt());
//        reservation->set_expire_time(query.value(6).toInt());
//    }
//    if (reservationList.reservation_size() == 0) {
//        query.finish();
//        return false;
//    }
//    query.finish();
//    return true;
//}

//bool SqlWorker::updateBooking(const biotech::osla::BookingList &bookingList) {
//    QMutexLocker locker(&m_Mutex);
//    DEBUG_LOG("Parse Server Command : [" << __FUNCTION__);
//    QSqlQuery query(db);

//    db.transaction();
//    for (int i = 0; i < bookingList.booking_size(); i++) {
//        if ((uint) bookingList.booking(i).status() != biotech::osla::BOOKING_STATUS_VALID) {
//            DEBUG_LOG("delete from tbooking");
//            query.prepare("delete from TBooking where ID=:ID");
//            query.bindValue(":ID", (uint) bookingList.booking(i).id());
//            if (!query.exec()) {
//                ERROR_LOG("SqlWorker: Failed to update TBooking:" << ConvertQ2CString(query.lastError().text()));
//                db.rollback();
//                return false;
//            }
//            DEBUG_LOG("SqlWorker: Booking" << (uint) bookingList.booking(i).id() << "deleted");
//        } else {
//            query.prepare("replace into TBooking(ID,UserID,StartTime,EndTime,Type,Status) "
//                                  "values(:ID,:UserID,:StartTime,:EndTime,:Type,:Status)");
//            query.bindValue(":ID", (uint) bookingList.booking(i).id());
//            query.bindValue(":UserID", (uint) bookingList.booking(i).user_id());
//            query.bindValue(":StartTime", (uint) bookingList.booking(i).start_time());
//            query.bindValue(":EndTime", (uint) bookingList.booking(i).end_time());
//            query.bindValue(":Type", (uint) bookingList.booking(i).type());
//            query.bindValue(":Status", (uint) bookingList.booking(i).status());
//            if (!query.exec()) {
//                ERROR_LOG("SqlWorker: Failed to update TBooking:" << ConvertQ2CString(query.lastError().text()));
//                db.rollback();
//                return false;
//            }
//        }
//    }
//    db.commit();
//    QProcess::execute("sync");
//    DEBUG_LOG("SqlWorker: TBooking updated");
//    return true;
//}

//biotech::osla::Booking SqlWorker::getCurrentBooking(QDateTime currentDateTime) {
//    QMutexLocker locker(&m_Mutex);
//    biotech::osla::Booking booking;
//    booking.set_id(0);
//    QSqlQuery query(db);

//    IniParse ini(OSLA_CONFIG);
//    uint bookingHoldingTime = ini.GetLong("Instrument.BookingHoldingTime");
//    uint bookingBreakTime = ini.GetLong("Instrument.BookingBreakTime");

//    //query.prepare("select * from TBooking where StartTime>:Time and EndFlag=0 order by StartTime limit 1");
//    query.prepare("select * from TBooking where (Status=1105 or Status=1111) and EndFlag=0 order by StartTime limit 1");
//   // query.bindValue(":Time", currentDateTime.toTime_t() - bookingHoldingTime * 60);
//    if (!query.exec()) {
//        ERROR_LOG("SqlWorker: Failed to select from TBooking:" << ConvertQ2CString(query.lastError().text()));
//        query.finish();
//        return booking;
//    }
//    if (query.first()) {
//        booking.set_id(query.value(0).toInt());
//        booking.set_user_id(query.value(1).toInt());
//        booking.set_start_time(query.value(2).toInt());
//        booking.set_end_time(query.value(3).toInt());
//        booking.set_type(query.value(4).toInt());
//        booking.set_status(query.value(5).toInt());
//    }
//    query.finish();
//    return booking;
//}

//biotech::osla::Booking SqlWorker::getBookingById(int bookingId) {
//    QMutexLocker locker(&m_Mutex);
//    biotech::osla::Booking booking;
//    booking.set_id(0);

//    QSqlQuery query(db);
//    query.prepare("select * from TBooking where id = :id");
//    query.bindValue(":id", bookingId);
//    if (!query.exec()) {
//        ERROR_LOG("SqlWorker: Failed to select from TBooking by Id :" << ConvertQ2CString(query.lastError().text()));
//        query.finish();
//        return booking;
//    }
//    if (query.first()) {
//        booking.set_id(query.value(0).toInt());
//        booking.set_user_id(query.value(1).toInt());
//        booking.set_start_time(query.value(2).toInt());
//        booking.set_end_time(query.value(3).toInt());
//        booking.set_type(query.value(4).toInt());
//        booking.set_status(query.value(5).toInt());
//    }
//    query.finish();

//    return booking;
//}

//biotech::osla::BookingList SqlWorker::getRecentBookingsInDays(QDateTime currentDateTime, uint days) {
//    QMutexLocker locker(&m_Mutex);
//    biotech::osla::BookingList bookingList;
//    QSqlQuery query(db);
//    IniParse ini(OSLA_CONFIG);
//    uint bookingHoldingTime = ini.GetLong("Instrument.BookingHoldingTime");

//    auto fun = [&](int start, int end) -> bool {
//        query.prepare(
//                "select * from TBooking where StartTime>:StartTime and StartTime<:LimitTime and EndFlag=0 order by StartTime");
//        query.bindValue(":StartTime", start);
//        query.bindValue(":LimitTime", end);
//        if (!query.exec()) {
//            ERROR_LOG("SqlWorker: Failed to select from TBooking:" << ConvertQ2CString(query.lastError().text()));
//            query.finish();
//            return false;
//        }
//        biotech::osla::Booking *booking;
//        while (query.next()) {
//            booking = bookingList.add_booking();
//            booking->set_id(query.value(0).toInt());
//            booking->set_user_id(query.value(1).toInt());
//            booking->set_start_time(query.value(2).toInt());
//            booking->set_end_time(query.value(3).toInt());
//            booking->set_type(query.value(4).toInt());
//            booking->set_status(query.value(5).toInt());
//        }
//        query.finish();
//    };

//    fun(currentDateTime.toTime_t() - bookingHoldingTime * 60,
//        QDateTime(currentDateTime.date().addDays(days)).toTime_t());

//    return bookingList;
//}

//bool SqlWorker::updateBookingStatus(const biotech::osla::Booking &booking, bool isEnd) {
//    QMutexLocker locker(&m_Mutex);
//    QSqlQuery query(db);
//    db.transaction();
//    query.prepare("update TBooking set Status=:Status,EndFlag=:EndFlag where ID=:ID");
//    query.bindValue(":Status", booking.status());
//    query.bindValue(":EndFlag", (false == isEnd ? 0 : 1));
//    query.bindValue(":ID", booking.id());
//    if (!query.exec()) {
//        ERROR_LOG("SqlWorker: Failed to update TBooking:" << ConvertQ2CString(query.lastError().text()));
//        db.rollback();
//        return false;
//    }
//    db.commit();
//    QProcess::execute("sync");
//    return true;
//}

//int SqlWorker::endExpiredBookings(const QDateTime &currentDateTime) {
//    QMutexLocker locker(&m_Mutex);
//    int count = 0;
//    IniParse ini(OSLA_CONFIG);
//    uint bookingHoldingTime = ini.GetLong("Instrument.BookingHoldingTime");
//    uint timestamp = currentDateTime.toTime_t() - bookingHoldingTime * 60;
//    QSqlQuery query(db);

//    // 结束过期的“有效预约”和“被终止预约”

//    auto getBookingFun = [&](uint stamp, int eventType, int status1, int status2) -> std::list<quint64> {
//        std::list<quint64> bookIds;
//        // 产生系统事件
//        query.prepare("select :EventType, ID"
//                              "  from TBooking "
//                              " where StartTime <= :Timestamp "
//                              "   and (Status=:Status1 or Status=:Status2) "
//                              "   and EndFlag=0");
//        query.bindValue(":EventType", eventType);
//        query.bindValue(":Timestamp", stamp);
//        query.bindValue(":Status1", status1);
//        query.bindValue(":Status2", status2);

//        if (!query.exec()) {
//            ERROR_LOG("SqlWorker: Failed to select from TBooking:" << ConvertQ2CString(query.lastError().text()));
//            query.finish();
//            return bookIds;
//        }

//        while (query.next()) {
//            uint eventType = query.value(0).toUInt();
//            quint64 bookingId = query.value(1).toLongLong();
//            bookIds.push_back(bookingId);
//            addSystemEventLocal(eventType, bookingId);
//        }
//        query.finish();
//        return bookIds;
//    };

//    auto updateBookingFun = [&](std::list<quint64> bookIdL, int status) -> bool {
//        db.transaction();
//        for (quint64 v : bookIdL) {
//            query.prepare("update TBooking set Status=:FinalStatus,EndFlag=1 where ID=:ID");
//            query.bindValue(":FinalStatus", status);
//            query.bindValue(":ID", v);
//            if (!query.exec()) {
//                ERROR_LOG("SqlWorker: Failed to update TBooking:" << ConvertQ2CString(query.lastError().text()));
//                query.finish();
//                db.rollback();
//                return false;
//            }
//        }
//        query.finish();
//        db.commit();
//        return true;
//    };

//    {
//        std::list<quint64> bookIds = getBookingFun(timestamp, biotech::osla::SYSTEM_EVENT_BOOKING_TERMINATED,
//                                                   biotech::osla::BOOKING_STATUS_VALID,
//                                                   biotech::osla::BOOKING_STATUS_TERMINATED);
//        // 修改“有效预约”状态为“被终止预约”，置位结束标志
//        // "被终止预约“直接置位结束标志
//        if (false == updateBookingFun(bookIds, biotech::osla::BOOKING_STATUS_TERMINATED)) {
//            ERROR_LOG("SqlWorker: Failed to update TBooking");
//            return -1;
//        }
////        // 产生系统事件
////        query.prepare("select :EventType, ID"
////                              "  from TBooking "
////                              " where StartTime <= :Timestamp "
////                              "   and (Status=:Status1 or Status=:Status2) "
////                              "   and EndFlag=0");
////        query.bindValue(":EventType", biotech::osla::SYSTEM_EVENT_BOOKING_TERMINATED);
////        query.bindValue(":Timestamp", timestamp);
////        query.bindValue(":Status1", biotech::osla::BOOKING_STATUS_VALID);
////        query.bindValue(":Status2", biotech::osla::BOOKING_STATUS_TERMINATED);
////        if (!query.exec()) {
////            ERROR_LOG("SqlWorker: Failed to select from TBooking:" << ConvertQ2CString(query.lastError().text()));
////            query.finish();
////            return -1;
////        }
////        while (query.next()) {
////            uint eventType = query.value(0).toUInt();
////            quint64 bookingId = query.value(1).toLongLong();
////            bookIds.push_back(bookingId);
////            addSystemEventLocal(eventType, bookingId);
////        }
////        query.finish();

//        // 修改“有效预约”状态为“被终止预约”，置位结束标志
//        // "被终止预约“直接置位结束标志
//        if (false == updateBookingFun(bookIds, biotech::osla::BOOKING_STATUS_TERMINATED)) {
//            ERROR_LOG("SqlWorker: Failed to update TBooking");
//            return -1;
//        }
//    }

//    // 结束“被占用预约”
//    {
//        std::list<quint64> bookIds = getBookingFun(timestamp, biotech::osla::SYSTEM_EVENT_BOOKING_OCCUPIED,
//                                                   biotech::osla::BOOKING_STATUS_OCCUPIED, -1);
//        if (false == updateBookingFun(bookIds, biotech::osla::BOOKING_STATUS_OCCUPIED)) {
//            ERROR_LOG("SqlWorker: Failed to update TBooking");
//            return -1;
//        }
////        db.transaction();
////        query.prepare("replace into TSystemEvent(EventType,EventContent,CreateTime) "
////                              "select :EventType,ID,strftime('%s','now') from TBooking "
////                              "where StartTime<=:Timestamp and Status=:Status and EndFlag=0");
////        query.bindValue(":EventType", biotech::osla::SYSTEM_EVENT_BOOKING_OCCUPIED);
////        query.bindValue(":Timestamp", timestamp);
////        query.bindValue(":Status", biotech::osla::BOOKING_STATUS_OCCUPIED);
////        if (!query.exec()) {
////            DEBUG_LOG("SqlWorker: Failed to select from TBooking:" << ConvertQ2CString(query.lastError().text()));
////            query.finish();
////            db.rollback();
////            return -1;
////        }
////        query.finish();

//        // 置位结束标志
////        query.prepare("update TBooking set EndFlag=1 where StartTime<=:Timestamp and Status=:Status and EndFlag=0");
////        query.bindValue(":Timestamp", timestamp);
////        query.bindValue(":Status", biotech::osla::BOOKING_STATUS_OCCUPIED);
////        if (!query.exec()) {
////            DEBUG_LOG("SqlWorker: Failed to update TBooking:" << ConvertQ2CString(query.lastError().text()));
////            query.finish();
////            db.rollback();
////            return -1;
////        }
////        count += query.numRowsAffected();
////        query.finish();
////        db.commit();
//    }

//    // 结束“缺席预约”（理论上不会有，因为预约状态变为“缺席”的时候会立刻置位结束标志）
//    {
//        std::list<quint64> bookIds = getBookingFun(timestamp, biotech::osla::SYSTEM_EVENT_BOOKING_ABSENT,
//                                                   biotech::osla::BOOKING_STATUS_ABSENT, -1);
//        if (false == updateBookingFun(bookIds, biotech::osla::BOOKING_STATUS_ABSENT)) {
//            ERROR_LOG("SqlWorker: Failed to update TBooking");
//            return -1;
//        }
////
////        db.transaction();
////        query.prepare("replace into TSystemEvent(EventType,EventContent,CreateTime) "
////                              "select :EventType,ID,strftime('%s','now') from TBooking "
////                              "where StartTime<=:Timestamp and Status=:Status and EndFlag=0");
////        query.bindValue(":EventType", biotech::osla::SYSTEM_EVENT_BOOKING_ABSENT);
////        query.bindValue(":Timestamp", timestamp);
////        query.bindValue(":Status", biotech::osla::BOOKING_STATUS_ABSENT);
////        if (!query.exec()) {
////            ERROR_LOG("SqlWorker: Failed to select from TBooking:" << ConvertQ2CString(query.lastError().text()));
////            query.finish();
////            db.rollback();
////            return -1;
////        }
////        query.finish();
////
////        // 置位结束标志
////        query.prepare("update TBooking set EndFlag=1 where StartTime<=:Timestamp and Status=:Status and EndFlag=0");
////        query.bindValue(":Timestamp", timestamp);
////        query.bindValue(":Status", biotech::osla::BOOKING_STATUS_ABSENT);
////        if (!query.exec()) {
////            ERROR_LOG("SqlWorker: Failed to update TBooking:" << ConvertQ2CString(query.lastError().text()));
////            query.finish();
////            db.rollback();
////            return -1;
////        }
////        count += query.numRowsAffected();
////        query.finish();
////        db.commit();
//    }

//    // 结束“完成预约”（理论上不会有，因为预约状态变为“完成”的时候会立刻置位结束标志）
//    {
//        std::list<quint64> bookIds = getBookingFun(timestamp, biotech::osla::SYSTEM_EVENT_BOOKING_COMPLETED,
//                                                   biotech::osla::BOOKING_STATUS_COMPLETED, -1);
//        if (false == updateBookingFun(bookIds, biotech::osla::BOOKING_STATUS_COMPLETED)) {
//            ERROR_LOG("SqlWorker: Failed to update TBooking");
//            return -1;
//        }
////        db.transaction();
////        // 产生系统事件
////        query.prepare("replace into TSystemEvent(EventType,EventContent,CreateTime) "
////                              "select :EventType,ID,strftime('%s','now') from TBooking "
////                              "where StartTime<=:Timestamp and Status=:Status and EndFlag=0");
////        query.bindValue(":EventType", biotech::osla::SYSTEM_EVENT_BOOKING_COMPLETED);
////        query.bindValue(":Timestamp", timestamp);
////        query.bindValue(":Status", biotech::osla::BOOKING_STATUS_COMPLETED);
////        if (!query.exec()) {
////            ERROR_LOG("SqlWorker: Failed to select from TBooking:" << ConvertQ2CString(query.lastError().text()));
////            query.finish();
////            db.rollback();
////            return -1;
////        }
////        query.finish();
////
////        // 置位结束标志
////        query.prepare("update TBooking set EndFlag=1 where StartTime<=:Timestamp and Status=:Status and EndFlag=0");
////        query.bindValue(":Timestamp", timestamp);
////        query.bindValue(":Status", biotech::osla::BOOKING_STATUS_COMPLETED);
////        if (!query.exec()) {
////            ERROR_LOG("SqlWorker: Failed to update TBooking:" << ConvertQ2CString(query.lastError().text()));
////            query.finish();
////            db.rollback();
////            return -1;
////        }
////        count += query.numRowsAffected();
////        query.finish();
////        db.commit();
//    }
//    QProcess::execute("sync");
//    return 0;
//}

//bool SqlWorker::updateCalendar(const biotech::osla::CalendarList &calendarList) {
//    QMutexLocker locker(&m_Mutex);
//    DEBUG_LOG("Parse Server Command : [" << __FUNCTION__);
//    QSqlQuery query(db);

//    db.transaction();
//    for (int i = 0; i < calendarList.calendar_size(); i++) {
//        query.prepare("replace into TCalendar(Day,IsHoliday) "
//                              "values(:Day,:IsHoliday)");
//        query.bindValue(":Day", (uint) calendarList.calendar(i).day());
//        query.bindValue(":IsHoliday", calendarList.calendar(i).is_holiday());
//        if (!query.exec()) {
//            DEBUG_LOG(
//                    "SqlWorker: Failed to update TCalendar:" << ConvertQ2CString(query.lastError().text()));
//            db.rollback();
//            return false;
//        }
//    } // for
//    db.commit();
//    DEBUG_LOG("SqlWorker: TCalendar updated");
//    QProcess::execute("sync");
//    return true;
//}

//bool SqlWorker::isHoliday(const QDate &date) {
//    QMutexLocker locker(&m_Mutex);
//    QSqlQuery query(db);
//    QDate now = QDateTime::currentDateTime().date();
//    int day = now.year() * 10000 + now.month() * 100 + now.day();
//    DEBUG_LOG("is holiday, day is : " << day);
//    //int day = 364 - date.daysTo(QDate(0001, 1, 1));
//    query.prepare("select IsHoliday from TCalendar where Day=:Day");
//    query.bindValue(":Day", day);
//    if (!query.exec()) {
//        DEBUG_LOG("SqlWorker: Failed to select from TCalendar:" << ConvertQ2CString(query.lastError().text()));
//        query.finish();
//        return false;
//    }
//    if (!query.first()) {
//        DEBUG_LOG("SqlWorker: No TCalendar record");
//        query.finish();
//        return false;
//    }
//    bool result = query.value(0).toBool();
//    query.finish();
//    return result;
//}

//bool SqlWorker::updateSession(const biotech::osla::SessionList &sessionList) {
//    QMutexLocker locker(&m_Mutex);
//    DEBUG_LOG("Parse Server Command : [" << __FUNCTION__);
//    QSqlQuery query(db);

//    db.transaction();
//    for (int i = 0; i < sessionList.session_size(); i++) {
//        query.prepare(
//                "replace into TSession(ID,StartTime,EndTime,Type,UserID,CardSerial,UserRight,AccountID,GroupID,BookingID,PenaltyFlag,CompleteFlag,SyncTime,SyncFlag) "
//                        "values(:ID,:StartTime,:EndTime,:Type,:UserID,:CardSerial,:UserRight,:AccountID,:GroupID,:BookingID,:PenaltyFlag,:CompleteFlag,:SyncTime,:SyncFlag)");
//        query.bindValue(":ID", (quint64) sessionList.session(i).id());
//        query.bindValue(":StartTime", (uint) sessionList.session(i).start_time());
//        query.bindValue(":EndTime", (uint) sessionList.session(i).end_time());
//        query.bindValue(":Type", (uint) sessionList.session(i).type());
//        query.bindValue(":UserID", (uint) sessionList.session(i).user_id());
//        query.bindValue(":CardSerial", ConvertC2QString(sessionList.session(i).card_serial()));
//        query.bindValue(":UserRight", (uint) sessionList.session(i).user_right());
//        query.bindValue(":AccountID", (uint) sessionList.session(i).account_id());
//        query.bindValue(":GroupID", (uint) sessionList.session(i).group_id());
//        if (sessionList.session(i).has_booking_id())
//            query.bindValue(":BookingID", (uint) sessionList.session(i).booking_id());
//        else
//            query.bindValue(":BookingID", QVariant(QVariant::Int));
//        query.bindValue(":PenaltyFlag", sessionList.session(i).penalty_flag());
//        query.bindValue(":CompleteFlag", 1);
//        query.bindValue(":SyncTime", QDateTime::currentDateTime().toTime_t());
//        query.bindValue(":SyncFlag", 2);
//        if (!query.exec()) {
//            DEBUG_LOG(
//                    "SqlWorker: Failed to update TSession:" << ConvertQ2CString(query.lastError().text()));
//            db.rollback();
//            return false;
//        }
//    }
//    db.commit();
//    QProcess::execute("sync");
//    DEBUG_LOG("SqlWorker: TSession updated");
//    return true;
//}

//bool SqlWorker::updateUserInTraining(const biotech::osla::UserInTrainingList &userInTraingingList) {
//    QMutexLocker locker(&m_Mutex);
//    DEBUG_LOG("Parse Server Command : [" << __FUNCTION__);
//    QSqlQuery query(db);

//    db.transaction();
//    for (int i = 0; i < userInTraingingList.user_in_training_size(); i++) {
//        query.prepare(
//                "replace into TUserInTraining(SessionID, CardSerial, UserID, CheckInTime, CompleteFlag, SyncTime, SyncFlag) "
//                        "values(:SessionID, :CardSerial, :UserID, :CheckInTime, :CompleteFlag, :SyncTime, :SyncFlag)");
//        query.bindValue(":SessionID", (quint64) userInTraingingList.user_in_training(i).session_id());
//        query.bindValue(":CardSerial", ConvertC2QString(userInTraingingList.user_in_training(i).card_serial()));
//        query.bindValue(":UserID", (uint) userInTraingingList.user_in_training(i).user_id());
//        query.bindValue(":CheckInTime", (uint) userInTraingingList.user_in_training(i).check_in_time());
//        query.bindValue(":CompleteFlag", 1);
//        query.bindValue(":SyncTime", QDateTime::currentDateTime().toTime_t());
//        query.bindValue(":SyncFlag", 2);
//        if (!query.exec()) {
//            ERROR_LOG("SqlWorker: Failed to update TUserInTraining:" << ConvertQ2CString(query.lastError().text()));
//            db.rollback();
//            return false;
//        }
//    }
//    db.commit();
//    DEBUG_LOG("SqlWorker: TUserInTraining updated");
//    QProcess::execute("sync");
//    return true;
//}

//bool SqlWorker::addUserInTraining(const biotech::osla::UserInTraining &userInTraining) {
//    QMutexLocker locker(&m_Mutex);
//    QSqlQuery query(db);
//    db.transaction();
//    query.prepare("replace into TUserInTraining(SessionID, CardSerial, UserID, CheckInTime) "
//                          "values(:SessionID, :CardSerial, :UserID, :CheckInTime)");
//    query.bindValue(":SessionID", (quint64) userInTraining.session_id());
//    query.bindValue(":CardSerial", ConvertC2QString(userInTraining.card_serial()));
//    query.bindValue(":UserID", (uint) userInTraining.user_id());
//    query.bindValue(":CheckInTime", (uint) userInTraining.check_in_time());
//    if (!query.exec()) {
//        ERROR_LOG("SqlWorker: Failed to update TUserInTraining:" << ConvertQ2CString(query.lastError().text()));
//        return false;
//    }
//    db.commit();
//    QProcess::execute("sync");
//    return true;
//}

//biotech::osla::UserInTrainingList SqlWorker::getUserInTraining(uint syncTimestamp, uint count) {
//    QMutexLocker locker(&m_Mutex);
//    biotech::osla::UserInTrainingList userInTrainingList;

//    QSqlQuery query(db);
//    query.prepare("select SessionID, CardSerial, UserID, CheckInTime, rowid "\
//                  "from TUserInTraining where SyncFlag = 0 order by CheckInTime limit :MaxListSize");
//    query.bindValue(":MaxListSize", count);
//    if (!query.exec()) {
//        DEBUG_LOG("SqlWorker: Failed to select TUserInTraining:" << ConvertQ2CString(query.lastError().text()));
//        query.finish();
//        return userInTrainingList;
//    }
//    biotech::osla::UserInTraining *userInTraining;
//    while (query.next()) {
//        userInTraining = userInTrainingList.add_user_in_training();
//        userInTraining->set_session_id(query.value(0).toLongLong());         // 使用记录ID
//        userInTraining->set_card_serial(ConvertQ2CString(query.value(1).toString()));   // 卡号
//        userInTraining->set_user_id(query.value(2).toInt());       // 用户ID
//        userInTraining->set_check_in_time(query.value(3).toInt());     // 签到时间
//        userInTraining->set_rowid(query.value(4).toInt());
//    }
//    query.finish();

//    db.transaction();
//    for (int i = 0; i < userInTrainingList.user_in_training_size(); ++i) {
//        const biotech::osla::UserInTraining &inst = userInTrainingList.user_in_training(i);
//        query.prepare("update TUserInTraining set SyncTime=:SyncTime, SyncFlag = 1 where rowid = :ROWID");
//        query.bindValue(":ROWID", inst.rowid());
//        if (!query.exec()) {
//            DEBUG_LOG("SqlWorker: Failed to update TUserInTraining:" << ConvertQ2CString(query.lastError().text()));
//            query.finish();
//            db.rollback();
//            return userInTrainingList;
//        }
//    }
//    db.commit();
//    query.finish();

//    QProcess::execute("sync");
//    return userInTrainingList;
//}

//int SqlWorker::userInTrainingCount(quint64 sessionId) {
//    QMutexLocker locker(&m_Mutex);
//    QSqlQuery query(db);
//    int count = 0;
//    query.prepare("select count(*) from TUserInTraining where SessionID=:SessionID");
//    query.bindValue(":SessionID", sessionId);
//    if (!query.exec()) {
//        ERROR_LOG("SqlWorker: Failed to select from TUserInTraining:" << ConvertQ2CString(query.lastError().text()));
//        query.finish();
//        return count;
//    }
//    if (query.first()) {
//        count = query.value(0).toInt();
//    }
//    query.finish();
//    return count;
//}

//bool SqlWorker::hasCheckinRecord(quint64 sessionId, uint cardSerial) {
//    QMutexLocker locker(&m_Mutex);
//    QSqlQuery query(db);
//    DEBUG_LOG("Session Id :" << sessionId);
//    DEBUG_LOG("CardSerial Id :" << cardSerial);
//    query.prepare("select * from TUserInTraining where SessionID=:SessionID and CardSerial=:CardSerial");
//    query.bindValue(":SessionID", sessionId);
//    query.bindValue(":CardSerial", cardSerial);
//    if (!query.exec()) {
//        ERROR_LOG("SqlWorker: Failed to select from TUserInTraining:"
//                          << ConvertQ2CString(query.lastError().text()));
//    }
//    if (!query.first()) {
//        DEBUG_LOG("SqlWorker: No checkin record");
//        query.finish();
//        return false;
//    }
//    query.finish();
//    return true;
//}

//int SqlWorker::endUserInTraining(quint64 sessionId) {
//    QMutexLocker locker(&m_Mutex);
//    QSqlQuery query(db);
//    db.transaction();
//    query.prepare("update TUserInTraining set CompleteFlag=1 where SessionID=:SessionID");
//    query.bindValue(":SessionID", sessionId);
//    if (!query.exec()) {
//        ERROR_LOG("SqlWorker: Failed to update TUserInTraining:" << ConvertQ2CString(query.lastError().text()));
//        db.rollback();
//        return -1;
//    }
//    db.commit();
//    QProcess::execute("sync");
//    return query.numRowsAffected();
//}

//int SqlWorker::endUncompletedUserInTraining() {
//    QMutexLocker locker(&m_Mutex);
//    QSqlQuery query(db);
//    db.transaction();
//    if (!query.exec("update TUserInTraining set CompleteFlag=1 where CompleteFlag=0")) {
//        ERROR_LOG("SqlWorker: Failed to update TUserInTraining:" << ConvertQ2CString(query.lastError().text()));
//        db.rollback();
//        return -1;
//    }
//    db.commit();
//    QProcess::execute("sync");
//    return query.numRowsAffected();
//}

//bool SqlWorker::updateUserEvent(const biotech::osla::UserEventList &userEventList) {
//    QMutexLocker locker(&m_Mutex);
//    DEBUG_LOG("Parse Server Command : [" << __FUNCTION__ << "]");
//    QSqlQuery query(db);

//    db.transaction();
//    for (int i = 0; i < userEventList.user_event_size(); i++) {
//        query.prepare(
//                "replace into TUserEvent(ID,UserID,GroupID,CardSerial,CardType,ActionType,ActionResult,CreateTime,SyncTime,SyncFlag) "
//                        "values(:ID,:UserID,:GroupID,:CardSerial,:CardType,:ActionType,:ActionResult,:CreateTime,:SyncTime,:SyncFlag)");
//        query.bindValue(":ID", (quint64) userEventList.user_event(i).id());
//        query.bindValue(":UserID", (uint) userEventList.user_event(i).user_id());
//        if (userEventList.user_event(i).has_group_id())
//            query.bindValue(":GroupID", (uint) userEventList.user_event(i).group_id());
//        else
//            query.bindValue(":GroupID", QVariant(QVariant::Int));
//        query.bindValue(":CardSerial", ConvertC2QString(userEventList.user_event(i).card_serial()));
//        query.bindValue(":CardType", (uint) userEventList.user_event(i).card_type());
//        query.bindValue(":ActionType", (uint) userEventList.user_event(i).action_type());
//        query.bindValue(":ActionResult", (uint) userEventList.user_event(i).action_result());
//        query.bindValue(":CreateTime", (uint) userEventList.user_event(i).create_time());
//        query.bindValue(":SyncTime", QDateTime::currentDateTime().toTime_t());
//        query.bindValue(":SyncFlag", 2);
//        if (!query.exec()) {
//            ERROR_LOG("SqlWorker: Failed to update TUserEvent:" << ConvertQ2CString(query.lastError().text()));
//            db.rollback();
//            return false;
//        }
//    }
//    db.commit();
//    DEBUG_LOG("SqlWorker: TUserEvent updated");
//    QProcess::execute("sync");
//    return true;
//}

//bool SqlWorker::addUserEvent(const biotech::osla::UserEvent &userEvent) {
//    QMutexLocker locker(&m_Mutex);
//    QSqlQuery query(db);
//    db.transaction();
//    query.prepare("replace into TUserEvent(ID,UserID,GroupID,CardSerial,"
//                          "CardType,ActionType,ActionResult,CreateTime) "
//                          "values(:ID,:UserID,:GroupID,:CardSerial,"
//                          ":CardType,:ActionType,:ActionResult,:CreateTime)");
//    query.bindValue(":ID", (quint64) userEvent.id());
//    query.bindValue(":UserID", (uint) userEvent.user_id());
//    if (userEvent.has_group_id()) {
//        query.bindValue(":GroupID", (uint) userEvent.group_id());
//    } else {
//        query.bindValue(":GroupID", QVariant(QVariant::Int));
//    }
//    query.bindValue(":CardSerial", ConvertC2QString(userEvent.card_serial()));
//    query.bindValue(":CardType", (uint) userEvent.card_type());
//    query.bindValue(":ActionType", (uint) userEvent.action_type());
//    query.bindValue(":ActionResult", (uint) userEvent.action_result());
//    query.bindValue(":CreateTime", (uint) userEvent.create_time());
//    if (!query.exec()) {
//        ERROR_LOG("SqlWorker: Failed to update TUserEvent:" << ConvertQ2CString(query.lastError().text()));
//        db.rollback();
//        return false;
//    }
//    db.commit();
//    QProcess::execute("sync");
//    return true;
//}

//bool SqlWorker::addUserEvent(const User &user, uint actionType, uint actionResult) {
//    QMutexLocker locker(&m_Mutex);
//    quint64 lastId = getLastIdLocal("select ID from TUserEvent order by localid desc limit 1");
//    quint64 nextId = getNextIdLocal(lastId, user.timestamp, "select count(1) from TUserEvent where ID=:ID");
//    if (nextId == 0) {
//        DEBUG_LOG("SqlWorker: Failed to create new ID in TUserEvent, last ID" << lastId);
//        return false;
//    }

//    //QDateTime ctime;
//    //ctime = QDateTime::currentDateTime();
//    //QString ss = ctime.toString(Qt::LocalDate);
//    DEBUG_LOG("SqlWorker: New user event["
//                      << formatIdString(nextId).toLocal8Bit().data() << "],[" << actionType << "],[" << actionResult
//                      << "]");

//    QSqlQuery query(db);
//    db.transaction();
//    query.prepare("insert into TUserEvent(ID,UserID,GroupID,CardSerial,CardType,ActionType,ActionResult,CreateTime) "
//                          "values(:ID,:UserID,:GroupID,:CardSerial,:CardType,:ActionType,:ActionResult,:CreateTime)");
//    query.bindValue(":ID", nextId);
//    query.bindValue(":UserID", user.id);
//    query.bindValue(":GroupID", user.account.groupId);
//    query.bindValue(":CardSerial", ConvertC2QString(user.card.serial));
//    query.bindValue(":CardType", user.card.type);
//    query.bindValue(":ActionType", actionType);
//    query.bindValue(":ActionResult", actionResult);
//    query.bindValue(":CreateTime", user.timestamp);
//    //QDateTime currentDateTime = QDateTime::currentDateTime();
//    //uint currentDateTime_t = currentDateTime.toTime_t();
//    //query.bindValue(":CreateTime", currentDateTime_t);
//    if (!query.exec()) {
//        ERROR_LOG("SqlWorker: Failed to update TUserEvent:" << ConvertQ2CString(query.lastError().text()));
//        db.rollback();
//        return false;
//    }
//    db.commit();
//    QProcess::execute("sync");
//    return true;
//}

//// 查找用户事件表
//void SqlWorker::getUserEvent_data() {
//    QMutexLocker locker(&m_Mutex);

//    QSqlQuery query(db);
//    query.exec("select id, userid, groupid, cardserial, cardtype, actiontype, actionresult, createtime "\
//                          " from TUserEvent");
//    IniParse ini(OSLA_CONFIG);
//    if(query.last()) {
//        GlobalData::id = query.value(0).toLongLong();
//        GlobalData::user_id = query.value(1).toInt();
//        GlobalData::group_id =query.value(2).toInt();

//        GlobalData::card_serial =ConvertQ2CString(query.value(3).toString());
//        GlobalData::card_type =query.value(4).toInt();
//        GlobalData::action_type =query.value(5).toInt();
//        GlobalData::action_result =query.value(6).toInt();
//        GlobalData::create_time =query.value(7).toInt();
//        GlobalData::instrument_id =ini.GetLong("Instrument.ID");

////        qDebug()<< GlobalData::id << GlobalData::user_id << GlobalData::group_id;
////        qDebug()<< GlobalData::card_serial << GlobalData::card_type << GlobalData::action_type;
////        qDebug()<< GlobalData::action_result << GlobalData::create_time << GlobalData::instrument_id;

//    }

//}
//biotech::osla::UserEventList SqlWorker::getUserEvent(uint syncTimestamp, uint count) {

//    QMutexLocker locker(&m_Mutex);
//    biotech::osla::UserEventList userEventList;

//    QSqlQuery query(db);
//    query.prepare("select id, userid, groupid, cardserial, cardtype, actiontype, actionresult, createtime "\
//                          " from TUserEvent where SyncFlag = 0 order by LocalId, CreateTime limit :MaxListSize");
//    query.bindValue(":MaxListSize", count);
//    if (!query.exec()) {
//        DEBUG_LOG("SqlWorker: Failed to select TUserEvent:" << ConvertQ2CString(query.lastError().text()));
//        query.finish();
//        return userEventList;
//    }

//    IniParse ini(OSLA_CONFIG);
//    biotech::osla::UserEvent *userEvent;

//    while (query.next()) {
//        userEvent = userEventList.add_user_event();
//        userEvent->set_id(query.value(0).toLongLong());
//        userEvent->set_user_id(query.value(1).toInt());
//        if (!query.value(2).isNull()) {
//            userEvent->set_group_id(query.value(2).toInt());
//        }
//        userEvent->set_card_serial(ConvertQ2CString(query.value(3).toString()));
//        userEvent->set_card_type(query.value(4).toInt());
//        userEvent->set_action_type(query.value(5).toInt());
//        userEvent->set_action_result(query.value(6).toInt());
//        userEvent->set_create_time(query.value(7).toInt());
//        userEvent->set_instrument_id(ini.GetLong("Instrument.ID"));
//    }
//    query.finish();

//    db.transaction();
//    for (int i = 0; i < userEventList.user_event_size(); ++i) {
//        const biotech::osla::UserEvent &inst = userEventList.user_event(i);
//        query.prepare("update TUserEvent "\
//                  "   set SyncTime = :SyncTime, SyncFlag = 1 "\
//                  " where id = :ID ");
//        query.bindValue(":SyncTime", syncTimestamp);
//        query.bindValue(":id", inst.id());
//        if (!query.exec()) {
//            DEBUG_LOG("SqlWorker: Failed to update TUserEvent:" << ConvertQ2CString(query.lastError().text()));
//            query.finish();
//            db.rollback();
//            userEventList.clear_user_event();
//            return userEventList;
//        }
//    }
//    db.commit();


//    QProcess::execute("sync");

//    return userEventList;
//}

//quint64 SqlWorker::getUserEventByUser(const User &user) {
//    QMutexLocker locker(&m_Mutex);
//    QSqlQuery query(db);
//    query.prepare("select ID from TUserEvent where UserID=:UserID and CreateTime=:CreateTime");
//    query.bindValue(":UserID", user.id);
//    query.bindValue(":CreateTime", user.timestamp);
//    if (!query.exec()) {
//        DEBUG_LOG("SqlWorker: Failed to select from TUserEvent:" << ConvertQ2CString(query.lastError().text()));
//        query.finish();
//        return 0;
//    }
//    if (!query.first()) {
//        DEBUG_LOG("SqlWorker: No TUserEvent record");
//        query.finish();
//        return 0;
//    }
//    quint64 id = query.value(0).toLongLong();
//    query.finish();
//    return id;
//}

//bool SqlWorker::updateSystemEvent(const biotech::osla::SystemEventList &systemEventList) {
//    QMutexLocker locker(&m_Mutex);
//    DEBUG_LOG("Parse Server Command : [" << __FUNCTION__);
//    QSqlQuery query(db);

//    db.transaction();
//    for (int i = 0; i < systemEventList.system_event_size(); i++) {
//        query.prepare("replace into TSystemEvent(ID,EventType,EventContent,CreateTime,SyncTime,SyncFlag) "
//                              "values(:ID,:EventType,:EventContent,:CreateTime,:SyncTime,:SyncFlag)");
//        query.bindValue(":ID", (quint64) systemEventList.system_event(i).id());
//        query.bindValue(":EventType", (uint) systemEventList.system_event(i).event_type());
//        if (systemEventList.system_event(i).has_event_content())
//            query.bindValue(":EventContent", (uint) systemEventList.system_event(i).event_content());
//        else
//            query.bindValue(":EventContent", QVariant(QVariant::Int));
//        query.bindValue(":CreateTime", (uint) systemEventList.system_event(i).create_time());
//        query.bindValue(":SyncTime", QDateTime::currentDateTime().toTime_t());
//        query.bindValue(":SyncFlag", 2);
//        if (!query.exec()) {
//            DEBUG_LOG("SqlWorker: Failed to update TSystemEvent:"
//                              << ConvertQ2CString(query.lastError().text()));
//            db.rollback();
//            return false;
//        }
//    }
//    db.commit();
//    DEBUG_LOG("SqlWorker: TSystemEvent updated");
//    QProcess::execute("sync");
//    return true;
//}

//bool SqlWorker::addSystemEventLocal(uint eventType, quint64 eventContent) {
//    quint64 lastId = getLastIdLocal("select ID from TSystemEvent order by localid desc limit 1");
//    uint timestamp = QDateTime::currentDateTime().toTime_t();
//    quint64 nextId = getNextIdLocal(lastId, timestamp, "select count(1) from TSession where id=:ID");
//    if (nextId == 0) {
//        DEBUG_LOG("SqlWorker: Failed to create new ID in TSystemEvent, last ID " << lastId);
//        return false;
//    }

//    DEBUG_LOG("SqlWorker: New system event data [" << formatIdString(nextId).toLocal8Bit().data()
//                                                   << "],[" << eventType << "],[" << eventContent << "]");

//    QSqlQuery query(db);
//    db.transaction();
//    query.prepare("insert into TSystemEvent(ID,EventType,EventContent,CreateTime) "
//                          "values(:ID,:EventType,:EventContent,:CreateTime)");
//    query.bindValue(":ID", nextId);
//    query.bindValue(":EventType", eventType);
//    if (eventContent != 0) {
//        query.bindValue(":EventContent", eventContent);
//    } else {
//        query.bindValue(":EventContent", QVariant(QVariant::Int));
//    }
//    query.bindValue(":CreateTime", timestamp);
//    if (!query.exec()) {
//        ERROR_LOG("SqlWorker: Failed to update TSystemEvent:" << ConvertQ2CString(query.lastError().text()));
//        db.rollback();
//        return false;
//    }
//    db.commit();
//    QProcess::execute("sync");
//    return true;
//}

//bool SqlWorker::addSystemEvent(uint eventType, quint64 eventContent) {
//    QMutexLocker locker(&m_Mutex);
//    return addSystemEventLocal(eventType, eventContent);
//}

//bool SqlWorker::addSystemEvent(const biotech::osla::SystemEvent &systemEvent) {
//    QMutexLocker locker(&m_Mutex);
//    QSqlQuery query(db);
//    db.transaction();
//    query.prepare("replace into TSystemEvent(ID,EventType,EventContent,CreateTime) "
//                          "values(:ID,:EventType,:EventContent,:CreateTime)");
//    query.bindValue(":ID", (quint64) systemEvent.id());
//    query.bindValue(":EventType", (uint) systemEvent.event_type());
//    if (systemEvent.has_event_content()) {
//        query.bindValue(":EventContent", (quint64) systemEvent.event_content());
//    } else {
//        query.bindValue(":EventContent", QVariant(QVariant::Int));
//    }
//    query.bindValue(":CreateTime", (uint) systemEvent.create_time());
//    if (!query.exec()) {
//        ERROR_LOG("SqlWorker: Failed to update TSystemEvent:" << ConvertQ2CString(query.lastError().text()));
//        db.rollback();
//        return false;
//    }
//    db.commit();
//    QProcess::execute("sync");
//    return true;
//}

//biotech::osla::Session SqlWorker::getSession(long long id) {
//    biotech::osla::Session session;
//    QSqlQuery query(db);

//    DEBUG_LOG("id is " << id);
//    query.prepare("select * from TSession where id=:id");
//    query.bindValue(":id", (quint64) id);
//    if (!query.exec()) {
//        DEBUG_LOG("SqlWorker: Failed to select TSession:" << ConvertQ2CString(query.lastError().text()));
//        query.finish();
//        return session;
//    }

//    while (query.next()) {
//        session.set_id(query.value(0).toLongLong());
//        session.set_start_time(query.value(1).toInt());
//        session.set_end_time(query.value(2).toInt());
//        session.set_type(query.value(3).toInt());
//        session.set_user_id(query.value(4).toInt());
//        session.set_card_serial(query.value(5).toString().toLocal8Bit().data());
//        session.set_user_right(query.value(6).toInt());
//        session.set_account_id(query.value(7).toInt());
//        session.set_group_id(query.value(8).toInt());
//        if (!query.value(9).isNull()) {
//            session.set_booking_id(query.value(9).toInt());
//        }
//        session.set_penalty_flag(query.value(10).toBool());
//        IniParse ini(OSLA_CONFIG);
//        session.set_instrument_id(ini.GetLong("Instrument.ID"));
//        break;
//    }

//    query.finish();
//    QProcess::execute("sync");

//    return session;
//}

//void SqlWorker::cleanOldData(int sec)
//{
//    QSqlQuery query(db);

//    QString sql;

//    int currentTime = QDateTime::currentDateTime().toTime_t();

//    if (sec > currentTime)
//    {
//        return;
//    }

//    int beforeTime = currentTime - sec;

//    //delete old TUserEvent
//    sql = QString("delete from TUserEvent where (SyncTime < %1) and (SyncTime <> 0)").arg(beforeTime);
//    query.exec(sql);

//    //delete old TBookingEntrustEvent
//    sql = QString("delete from TBookingEntrustEvent where (SyncTime < %1) and (SyncTime <> 0)").arg(beforeTime);
//    query.exec(sql);

//    //delete old TDeviceCaptureEvent
//    sql = QString("delete from TDeviceCaptureEvent where (SyncTime < %1) and (SyncTime <> 0)").arg(beforeTime);
//    query.exec(sql);

//    //delete old TFtpEvent
//    sql = QString("delete from TFtpEvent where (SyncTime < %1) and (SyncTime <> 0)").arg(beforeTime);
//    query.exec(sql);

//    //delete old TSystemEvent
//    sql = QString("delete from TSystemEvent where (SyncTime < %1) and (SyncTime <> 0)").arg(beforeTime);
//    query.exec(sql);

//    //delete old TSession
//    sql = QString("delete from TSession where (SyncTime < %1) and (SyncTime <> 0)").arg(beforeTime);
//    query.exec(sql);

//    //delete old TBooking
//    sql = QString("delete from TBooking where EndTime < %1").arg(beforeTime);
//    query.exec(sql);
//}

//biotech::osla::SessionList SqlWorker::getSession(uint syncTimestamp, uint count) {
//    QMutexLocker locker(&m_Mutex);
//    biotech::osla::SessionList sessionList;

//    QSqlQuery query(db);

//    query.prepare("select ID, StartTime, EndTime, Type, UserID, "\
//                  "       CardSerial, UserRight, LongTimeUserRight, AccountID, GroupID, "\
//                  "       BookingID ,PenaltyFlag "\
//                  "  from TSession where SyncFlag = 0 order by id limit :MaxListSize");
//    query.bindValue(":MaxListSize", count);
//    if (!query.exec()) {
//        DEBUG_LOG("SqlWorker: Failed to select TSession:" << ConvertQ2CString(query.lastError().text()));
//        query.finish();
//        return sessionList;
//    }

//    IniParse ini(OSLA_CONFIG);
//    biotech::osla::Session *session;
//    while (query.next()) {
//        session = sessionList.add_session();
//        session->set_id(query.value(0).toLongLong());
//        DEBUG_LOG("id : " << query.value(0).toLongLong());
//        session->set_start_time(query.value(1).toInt());
//        DEBUG_LOG("start time : " << query.value(1).toLongLong());
//        session->set_end_time(query.value(2).toInt());
//        DEBUG_LOG("end time : " << query.value(2).toLongLong());
//        session->set_type(query.value(3).toInt());
//        session->set_user_id(query.value(4).toInt());
//        session->set_card_serial(query.value(5).toString().toLocal8Bit().data());
//        session->set_user_right(query.value(6).toInt());
//        session->set_long_time_right(query.value(7).toInt());
//        session->set_account_id(query.value(8).toInt());
//        session->set_group_id(query.value(9).toInt());
//        if (!query.value(10).isNull()) {
//            session->set_booking_id(query.value(10).toInt());
//        }
//        session->set_penalty_flag(query.value(11).toBool());
//        session->set_instrument_id(ini.GetLong("Instrument.ID"));
//    }
//    query.finish();

//    db.transaction();
//    for (int i = 0; i < sessionList.session_size(); i++) {
//        const ::biotech::osla::Session &s = sessionList.session(i);
//        query.prepare("update TSession set SyncTime=:SyncTime, SyncFlag = 1 where Id = :ID");
//        query.bindValue(":SyncTime", syncTimestamp);
//        query.bindValue(":ID", s.id());
//        if (!query.exec()) {
//            ERROR_LOG("SqlWorker: Failed to update TSession:" << ConvertQ2CString(query.lastError().text()));
//            db.rollback();
//            query.finish();
//            sessionList.clear_session();
//            return sessionList;
//        }
//    }
//    db.commit();

//    query.finish();
//    QProcess::execute("sync");
//    return sessionList;
//}

//biotech::osla::Session SqlWorker::getLastSessionUncompleted() {
//    QMutexLocker locker(&m_Mutex);
//    biotech::osla::Session session;
//    session.set_id(0);
//    QSqlQuery query(db);
//    if (!query.exec("select ID, StartTime, EndTime, Type, UserID, CardSerial, "
//                            "       UserRight, LongTimeUserRight, AccountID, GroupID, BookingID, PenaltyFlag, "
//                            "       CompleteFlag"
//                            "  from TSession order by StartTime desc limit 1")) {
//        ERROR_LOG("SqlWorker: Failed to select from TSession:" << ConvertQ2CString(query.lastError().text()));
//        query.finish();
//        return session;
//    }

//    if (query.first()) {
//        if (!query.value(12).toBool()) {
//            session.set_id(query.value(0).toLongLong());
//            session.set_start_time(query.value(1).toInt());
//            session.set_end_time(query.value(2).toInt());
//            session.set_type(query.value(3).toInt());
//            session.set_user_id(query.value(4).toInt());
//            session.set_card_serial(query.value(5).toString().toLocal8Bit().data());
//            session.set_user_right(query.value(6).toInt());
//            session.set_long_time_right(query.value(7).toInt());
//            session.set_account_id(query.value(8).toInt());
//            session.set_group_id(query.value(9).toInt());
//            if (!query.value(10).isNull()) {
//                session.set_booking_id(query.value(10).toInt());
//            }
//            session.set_penalty_flag(query.value(11).toBool());
//        }
//    }
//    query.finish();
//    return session;
//}

//bool SqlWorker::addSession(biotech::osla::Session &session, bool isCompleted) {
//    QMutexLocker locker(&m_Mutex);
//    DEBUG_LOG("");
//    if (session.id() == 0) {
//        quint64 lastId = getLastIdLocal("select ID from TSession order by ID desc limit 1");
//        quint64 nextId = getNextIdLocal(lastId, session.start_time(), "select count(1) from TSession where id = :ID");
//        if (nextId == 0) {
//            DEBUG_LOG("SqlWorker: Failed to create new ID in TSession, last ID" << lastId);
//            return false;
//        }
//        session.set_id(nextId);
//    }
//    DEBUG_LOG("");

//    QSqlQuery query(db);
//    db.transaction();
//    query.prepare("replace into TSession(ID,StartTime,EndTime,Type,UserID,CardSerial,"
//                          "              UserRight,LongTimeUserRight,AccountID,GroupID,BookingID,PenaltyFlag,CompleteFlag, SyncFlag) "
//                          "values(:ID,:StartTime,:EndTime,:Type,:UserID,:CardSerial,"
//                          "       :UserRight,:LongTimeUserRight,:AccountID,:GroupID,:BookingID,:PenaltyFlag,:CompleteFlag, :SyncFlag)");
//    query.bindValue(":ID", (quint64) session.id());
//    query.bindValue(":StartTime", (uint) session.start_time());
//    query.bindValue(":EndTime", (uint) session.end_time());
//    query.bindValue(":Type", (uint) session.type());
//    query.bindValue(":UserID", (uint) session.user_id());
//    query.bindValue(":CardSerial", ConvertC2QString(session.card_serial()));
//    query.bindValue(":UserRight", (uint) session.user_right());
//    query.bindValue(":LongTimeUserRight", (uint) session.long_time_right());
//    query.bindValue(":AccountID", (uint) session.account_id());
//    query.bindValue(":GroupID", (uint) session.group_id());
//    if (session.has_booking_id()) {
//        query.bindValue(":BookingID", (uint) session.booking_id());
//    } else {
//        query.bindValue(":BookingID", QVariant(QVariant::Int));
//    }
//    query.bindValue(":PenaltyFlag", (uint) session.penalty_flag());
//    query.bindValue(":CompleteFlag", (uint) isCompleted);
//    /// 如果未完成，标记不需要同步，如果以完成，标记需要同步
//    query.bindValue(":SyncFlag", (isCompleted == false) ? 2 : 0);

//    DEBUG_LOG("");
//    if (!query.exec()) {
//        ERROR_LOG("SqlWorker: Failed to update TSession:" << ConvertQ2CString(query.lastError().text()));
//        db.rollback();
//        return false;
//    }
//    DEBUG_LOG("");
//    QProcess::execute("sync");
//    db.commit();
//    return true;
//}

//int SqlWorker::endUncompletedSession() {
//    QMutexLocker locker(&m_Mutex);
//    QSqlQuery query(db);
//    db.transaction();
//    if (!query.exec("update TSession set CompleteFlag=1 where CompleteFlag=0")) {
//        ERROR_LOG("SqlWorker: Failed to update TSession:" << ConvertQ2CString(query.lastError().text()));
//        db.rollback();
//        return -1;
//    }
//    db.commit();
//    QProcess::execute("sync");
//    return query.numRowsAffected();
//}

//bool SqlWorker::addFtpEvent(const biotech::osla::FtpEvent &ftpEvent) {
//    QMutexLocker locker(&m_Mutex);
//    DEBUG_LOG("call this function[" << __FUNCTION__ << "]");

//    auto insertToDB = [&]() -> bool {
//        auto iter = ftpEvent.file().begin();
//        if (ftpEvent.file().end() == iter) {
//            return false;
//        }

//        QSqlQuery query(db);
//        query.prepare(
//                "insert into TFtpEvent(EventId, UserId, InstrumentId, GroupId, ActivedAccount, FileName, FilePath, ConsumeTime, FileSize, CreateTime, Percentage)"
//                        "values(:EventId, :UserId, :InstrumentId, :GroupId, :ActivedAccount, :FileName, :FilePath, :ConsumeTime, :FileSize, :CreateTime, :Percentage)");
//        query.bindValue(":EventId", ftpEvent.id());
//        query.bindValue(":UserId", ftpEvent.user_id());
//        query.bindValue(":InstrumentId", ftpEvent.instrument_id());
//        query.bindValue(":GroupId", ftpEvent.group_id());
//        query.bindValue(":ActivedAccount", ftpEvent.actived_account());
//        query.bindValue(":FileName", QVariant(iter->file_name().c_str()));
//        query.bindValue(":FilePath", QVariant(iter->path().c_str()));
//        query.bindValue(":ConsumeTime", iter->consume_time());
//        query.bindValue(":FileSize", iter->file_size());
//        query.bindValue(":CreateTime", QVariant(iter->create_time().c_str()));
//        query.bindValue(":Percentage", iter->percentage());

//        if (!query.exec()) {
//            ERROR_LOG("SqlWorker: Failed to insert TFtpEvent:" << ConvertQ2CString(query.lastError().text()));
//            return false;
//        }

//        return true;
//    };

//    auto updateToDB = [&]() -> bool {
//        auto iter = ftpEvent.file().begin();
//        if (ftpEvent.file().end() == iter) {
//            return false;
//        }

//        QSqlQuery query(db);
//        query.prepare("update TFtpEvent "\
//                      "   Set ConsumeTime=:ConsumeTime, "\
//                      "       Percentage=:Percentage, "\
//                      "       SyncTime=0, "\
//                      "       SyncFlag=0 "\
//                      " where eventId = :EventId "\
//                      "   and UserId = :UserId "\
//                      "   and InstrumentId = :InstrumentId "\
//                      "   and GroupId = :GroupId "\
//                      "   and ActivedAccount = :ActivedAccount"\
//                      "   and FileName = :FileName"\
//                      "   and FilePath = :FilePath");
//        query.bindValue(":ConsumeTime", iter->consume_time());
//        query.bindValue(":Percentage", iter->percentage());
//        query.bindValue(":EventId", ftpEvent.id());
//        query.bindValue(":UserId", ftpEvent.user_id());
//        query.bindValue(":InstrumentId", ftpEvent.instrument_id());
//        query.bindValue(":GroupId", ftpEvent.group_id());
//        query.bindValue(":ActivedAccount", ftpEvent.actived_account());
//        query.bindValue(":FileName", QString(iter->file_name().c_str()));
//        query.bindValue(":FilePath", QString(iter->path().c_str()));

//        if (!query.exec()) {
//            ERROR_LOG("SqlWorker: Failed to Update TFtpEvent:" << ConvertQ2CString(query.lastError().text()));
//            return false;
//        }

//        return true;
//    };

//    auto selectToDB = [&]() -> bool {
//        auto iter = ftpEvent.file().begin();
//        if (ftpEvent.file().end() == iter) {
//            return false;
//        }

//        QSqlQuery query(db);
//        query.prepare("select count(1) "
//                              "  from TFtpEvent "
//                              " where eventId = :EventId "
//                              "   and UserId = :UserId "
//                              "   and InstrumentId = :InstrumentId "
//                              "   and GroupId = :GroupId "
//                              "   and ActivedAccount = :ActivedAccount"
//                              "   and FileName = :FileName"
//                              "   and FilePath = :FilePath");
//        query.bindValue(":EventId", ftpEvent.id());
//        query.bindValue(":UserId", ftpEvent.user_id());
//        query.bindValue(":InstrumentId", ftpEvent.instrument_id());
//        query.bindValue(":GroupId", ftpEvent.group_id());
//        query.bindValue(":ActivedAccount", ftpEvent.actived_account());
//        query.bindValue(":FileName", QString(iter->file_name().c_str()));
//        query.bindValue(":FilePath", QString(iter->path().c_str()));

//        if (!query.exec()) {
//            DEBUG_LOG("SqlWorker: Failed to select from TFtpEvent :"
//                              << ConvertQ2CString(query.lastError().text()));
//        }

//        if (!query.first()) {
//            DEBUG_LOG("SqlWorker: No checkin record");
//            query.finish();
//            return false;
//        }
//        quint64 num = query.value(0).toLongLong();

//        query.finish();
//        DEBUG_LOG("query count form tftpevent, num is " << num);
//        return (num > 0 ? true : false);
//    };

//    bool status = selectToDB();
//    db.transaction();
//    if (true == status) {
//        status = updateToDB();
//    } else {
//        status = insertToDB();
//    }
//    if (false == status) {
//        db.rollback();
//    } else {
//        db.commit();
//    }

//    QProcess::execute("sync");
//    return status;
//}

//biotech::osla::FtpEventList SqlWorker::getFtpEvent(uint syncTimestamp, uint count) {
//    QMutexLocker locker(&m_Mutex);
//    biotech::osla::FtpEventList ftpEventList;

//    QSqlQuery query(db);
//    query.prepare("select EventId, UserId, InstrumentId, "\
//                  "       GroupId, ActivedAccount, FileName, FilePath, "\
//                  "       ConsumeTime, FileSize, CreateTime, Percentage, ID"\
//                  "  from TFtpEvent "\
//                  " where SyncFlag = 0 "\
//                  " order by ID limit :MaxListSize");
//    query.bindValue(":MaxListSize", count);
//    if (!query.exec()) {
//        DEBUG_LOG("SqlWorker: Failed to select TFtpEvent:" << ConvertQ2CString(query.lastError().text()));
//        query.finish();
//        return ftpEventList;
//    }

//    while (query.next()) {
//        biotech::osla::FtpEvent *ftpEvent = ftpEventList.add_ftp_event();
//        ftpEvent->set_id(query.value(0).toLongLong());
//        ftpEvent->set_user_id(query.value(1).toInt());
//        ftpEvent->set_instrument_id(query.value(2).toInt());
//        ftpEvent->set_group_id(query.value(3).toInt());
//        ftpEvent->set_actived_account(query.value(4).toInt());
//        ftpEvent->set_localid(query.value(11).toInt());

//        biotech::osla::FtpFileUpload *ffu = ftpEvent->add_file();
//        ffu->set_file_name(query.value(5).toString().toStdString().data());
//        ffu->set_path(query.value(6).toString().toLocal8Bit().data());
//        ffu->set_status(biotech::osla::FTP_FILE_UPLOAD_SUCCESS);
//        ffu->set_consume_time(query.value(7).toInt());

//        //ffu->set_file_size(query.value(8).toInt());
//        //DEBUG_LOG("file size :-->:" << ffu->file_size());
//        ffu->set_file_size(query.value(8).toLongLong());
//        DEBUG_LOG("file size :-->:" << ffu->file_size());
//        ffu->set_create_time(query.value(9).toString().toLocal8Bit().data());
//        ffu->set_percentage(query.value(10).toInt());
//        ffu->set_localid(query.value(11).toInt());

//        DEBUG_LOG("file name : " << ffu->file_name().c_str() << ", percentage : " << ffu->percentage());
//    }
//    query.finish();

//    db.transaction();
//    for (int i = 0; i < ftpEventList.ftp_event_size(); ++i) {
//        biotech::osla::FtpEvent ftpEvent = ftpEventList.ftp_event(i);
//        query.prepare("update TFtpEvent set SyncTime = :SyncTime, SyncFlag = 1 where ID = :ID");
//        query.bindValue(":SyncTime", syncTimestamp);
//        query.bindValue(":ID", ftpEvent.localid());
//        if (!query.exec()) {
//            ERROR_LOG("SqlWorker: Failed to update TFtpEvent:" << ConvertQ2CString(query.lastError().text()));
//            db.rollback();
//            query.finish();
//            ftpEventList.clear_ftp_event();
//            return ftpEventList;
//        }
//    }
//    db.commit();
//    query.finish();

//    QProcess::execute("sync");
//    return ftpEventList;
//}

//// 查找系统事件表
//biotech::osla::SystemEventList SqlWorker::getSystemEvent(uint syncTimestamp, uint count) {
//    QMutexLocker locker(&m_Mutex);
//    biotech::osla::SystemEventList systemEventList;

//    DEBUG_LOG("");
//    QSqlQuery query(db);
//    query.prepare("select id, EventType, EventContent, CreateTime, SyncTime from TSystemEvent where SyncFlag = 0 "
//                          " order by localid, CreateTime limit :MaxListSize");
//    query.bindValue(":MaxListSize", count);
//    if (!query.exec()) {
//        DEBUG_LOG("SqlWorker: Failed to select TSystemEvent:" << ConvertQ2CString(query.lastError().text()));
//        query.finish();
//        return systemEventList;
//    }

//    biotech::osla::SystemEvent *systemEvent;
//    IniParse ini(OSLA_CONFIG);
//    while (query.next()) {
//        systemEvent = systemEventList.add_system_event();
//        systemEvent->set_id(query.value(0).toLongLong());
//        systemEvent->set_event_type(query.value(1).toInt());
//        systemEvent->set_event_content(query.value(2).toLongLong());
//        systemEvent->set_create_time(query.value(3).toInt());
//        systemEvent->set_instrument_id(ini.GetLong("Instrument.ID"));
//    }
//    DEBUG_LOG("size is :" << systemEventList.system_event_size() << ", param count is " << count);
//    query.finish();

//    db.transaction();
//    for (int i = 0; i < systemEventList.system_event_size(); ++i) {
//        const biotech::osla::SystemEvent inst = systemEventList.system_event(i);
//        query.prepare("update TSystemEvent set SyncTime = :SyncTime, SyncFlag = 1 where id = :ID");
//        query.bindValue(":SyncTime", syncTimestamp);
//        query.bindValue(":ID", inst.id());
//        if (!query.exec()) {
//            DEBUG_LOG("SqlWorker: Failed to update TSystemEvent:" << ConvertQ2CString(query.lastError().text()));
//            db.rollback();
//            query.finish();
//            systemEventList.clear_system_event();
//            return systemEventList;
//        }
//    }
//    db.commit();

//    query.finish();
//    DEBUG_LOG("");
//    QProcess::execute("sync");
//    return systemEventList;
//}


//bool SqlWorker::setSyncFlag(QString tableName, long long id, int val) {
//    QMutexLocker locker(&m_Mutex);
//    QSqlQuery query(db);
//    db.transaction();
//    //only update SyncFlag==1,modified by li ,2018-6-27
//    QString format = QString("update %1 set SyncFlag=%2, SyncTime=%3 Where id=%4 and SyncFlag=1");
//    QString info = format.arg(tableName).arg(val).arg(time(NULL)).arg(id);
//    DEBUG_LOG("update sync flag [table:" << ConvertQ2CString(tableName) << ", id:" << id << ", val:" << val);
//    if (!query.exec(info)) {
//        ERROR_LOG("SqlWorker: Failed to update" << ConvertQ2CString(tableName) << ":"
//                                                << ConvertQ2CString(query.lastError().text()));
//        db.rollback();
//        return false;
//    }
//    db.commit();
//    DEBUG_LOG("SqlWorker:" << tableName.toLocal8Bit().data() << " updated success");
//    QProcess::execute("sync");
//    return true;
//}

//bool SqlWorker::setSyncFlag(long long sessionId, uint serialCard, int val) {
//    QMutexLocker locker(&m_Mutex);
//    QSqlQuery query(db);
//    db.transaction();
//    //only update when SyncFlag==1,modified by li ,2018-6-27
//    QString format = QString(
//            "update TUserInTraining set SyncFlag=%1, SyncTime=%2 Where SessionID=%3 and CardSerial=%4 and SyncFlag=1");
//    QString info = format.arg(val).arg(time(NULL)).arg(sessionId).arg(serialCard);
//    DEBUG_LOG("set sync flag sql is : " << ConvertQ2CString(info));
//    if (!query.exec(info)) {
//        ERROR_LOG("SqlWorker: Failed to update TUserInTraining:" << ConvertQ2CString(query.lastError().text()));
//        db.rollback();
//        return false;
//    }
//    db.commit();

//    DEBUG_LOG("SqlWorker: TUserInTraining updated success");
//    QProcess::execute("sync");
//    return true;
//}

//QVector<int> SqlWorker::unsyncedRecordCount() {
//    QMutexLocker locker(&m_Mutex);
//    QVector<int> count(6);
//    count.fill(-1);
//    QSqlQuery query(db);

//    auto sqlFun = [&](QString &sql, QString &info, int index) -> bool {
//        if (!query.exec(sql)) {
//            ERROR_LOG("SqlWorker: Failed to select from " << info.toLocal8Bit().data() << ":"
//                                                          << ConvertQ2CString(query.lastError().text()));
//            query.finish();
//            return false;
//        }
//        if (query.first()) {
//            count[index] = query.value(0).toInt();
//        }
//        query.finish();
//        return true;
//    };

//    struct _TempNode_ {
//        QString sql;
//        QString info;
//        int index;
//    };
//    _TempNode_ arr[] = {
//            {"select count(*) from TSession where SyncFlag in (0, 1)",             "TSession",             0}, /// 会话
//            {"select count(*) from TUserInTraining where SyncFlag in (0, 1)",      "TUserInTraining",      1}, /// 用户培训信息
//            {"select count(*) from TUserEvent where SyncFlag in (0, 1)",           "TUserEvent",           2}, /// 用户事件
//            {"select count(*) from TSystemEvent where SyncFlag in (0, 1)",         "TSystemEvent",         3}, /// 系统事件
//            {"select count(*) from TFtpEvent where SyncFlag in (0, 1)",            "TFtpEvent",            4}, /// FTP事件
//            {"select count(*) from TBookingEntrustEvent where SyncFlag in (0, 1)", "TBookingEntrustEvent", 5}, /// 委托事件
//            //{"select count(*) from TDeviceCaptureEvent where SyncFlag in (0, 1)",  "TDeviceCaptureEvent",  6}, /// 设备采集事件
//    };

//    for (int i = 0; i < (int) (sizeof(arr) / sizeof(arr[0])); ++i) {
//        sqlFun(arr[i].sql, arr[i].info, arr[i].index);
//    }

//    return count;
//}

//bool SqlWorker::deleteUnsyncedRecords() {
//    QMutexLocker locker(&m_Mutex);
//    QSqlQuery query(db);
//    auto delFun = [&](QString sql) -> bool {
//        if (!query.exec(sql)) {
//            ERROR_LOG("SqlWorker: Failed to [" << ConvertQ2CString(sql) << "]"
//                                               << ConvertQ2CString(query.lastError().text()));
//            return false;
//        }
//        return true;
//    };
//    db.transaction();
//    delFun(tr("delete from TSession where SyncFlag in (0, 1)"));
//    delFun(tr("delete from TUserInTraining where SyncFlag in (0, 1)"));
//    delFun(tr("delete from TUserEvent where SyncFlag in (0, 1)"));
//    delFun(tr("delete from TSystemEvent where SyncFlag in (0, 1)"));
//    delFun(tr("delete from TFtpEvent where SyncFlag in (0, 1)"));
//    delFun(tr("delete from TBookingEntrustEvent where SyncFlag in (0, 1)"));
//    delFun(tr("delete from TDeviceCaptureEvent where SyncFlag in (0, 1)"));
//    db.commit();
//    QProcess::execute("sync");

//    return true;
//}

//bool SqlWorker::exportRecordEvent(QString path) {
//    QMutexLocker locker(&m_Mutex);
//    QSqlQuery query(db);

//    auto sqlFun = [&](QString &sql, QString &info, int index) -> bool {
//        if (!query.exec(sql)) {
//            ERROR_LOG("SqlWorker: Failed to select from " << info.toLocal8Bit().data() << ":"
//                                                          << ConvertQ2CString(query.lastError().text()));
//            query.finish();
//            return false;
//        }

//        std::ofstream out;
//        out.open(std::string(path.toStdString() + "/" + info.toStdString() + ".csv"));
//        if (false == out.good()) {
//            return false;
//        }

//        while (query.next()) {
//            for (int i = 0; i < index; ++i) {
//                if (0 != i) {
//                    out << ", ";
//                }
//                out << query.value(i).toString().toStdString();
//            }
//            out << std::endl;
//        }
//        query.finish();
//        out.close();

//        return true;
//    };

//    struct _TempNode_ {
//        QString sql;
//        QString info;
//        int index;
//    };
//    _TempNode_ arr[] = {
//            {"select ID, StartTime, EndTime, Type, UserID, CardSerial, UserRight, AccountID, GroupID, BookingID, PenaltyFlag, CompleteFlag "
//                     "  from TSession where SyncFlag=0",                 "TSession",        12}, /// 会话
//            {"select SessionID, CardSerial, UserID, CheckInTime, CompleteFlag "
//                     "  from TUserInTraining where SyncFlag=0",          "TUserInTraining", 5}, /// 用户培训信息
//            {"select ID, UserId, GroupId, CardSerial, CardType, ActionType, ActionResult, CreateTime "
//                     "  from TUserEvent where SyncFlag=0 order by id",   "TUserEvent",      8}, /// 用户事件
//            {"select ID, EventType, EventContent, CreateTime, SyncTime "
//                     "  from TSystemEvent where SyncFlag=0 order by id", "TSystemEvent",    5}, /// 系统事件
//            {"select EventId, UserId, InstrumentId, GroupId, ActivedAccount, FileName, FilePath, ConsumeTime, FileSize, CreateTime "
//                     " from TFtpEvent where SyncFlag=0 order by ID",     "TFtpEvent",       10}, /// FTP事件
//    };

//    for (int i = 0; i < (int) (sizeof(arr) / sizeof(arr[0])); ++i) {
//        if (false == sqlFun(arr[i].sql, arr[i].info, arr[i].index)) {
//            break;
//        }
//    }

//    return false;
//}

//quint64 SqlWorker::getLastId(QString sql) {
//    QMutexLocker locker(&m_Mutex);
//    return getLastIdLocal(sql);
//}

//quint64 SqlWorker::getLastIdLocal(QString sql) {
//    QSqlQuery query(db);
//    if (!query.exec(sql)) {
//        ERROR_LOG("SqlWorker: Failed to [" << sql.toLocal8Bit().data() << "] :"
//                                           << ConvertQ2CString(query.lastError().text()));
//        query.finish();
//        return 0;
//    }
//    if (!query.first()) {
//        DEBUG_LOG("SqlWorker: No record For [" << sql.toLocal8Bit().data() << "]");
//        query.finish();
//        return 0;
//    }
//    quint64 id = query.value(0).toLongLong();
//    query.finish();
//    return id;
//}

//quint64 SqlWorker::getNextId(quint64 lastId, uint timestamp, QString sql) {
//    QMutexLocker locker(&m_Mutex);
//    return getNextIdLocal(lastId, timestamp, sql);
//}

//quint64 SqlWorker::getNextIdLocal(quint64 lastId, uint timestamp, QString sql) {
//    auto selectCountFun = [&](quint64 id, QString sql) -> bool {
//        QSqlQuery query(db);
//        query.prepare(sql);
//        query.bindValue(":ID", id);

//        if (!query.exec()) {
//            ERROR_LOG("SqlWorker: Failed to select from TEvent([" << sql.toLocal8Bit().data() << "] ):"
//                                                                  << ConvertQ2CString(query.lastError().text()));
//            query.finish();
//            return false;
//        }

//        if (!query.first()) {
//            DEBUG_LOG("SqlWorker: No Event [" << sql.toLocal8Bit().data() << "] record");
//            query.finish();
//            return false;
//        }

//        quint64 num = query.value(0).toLongLong();
//        query.finish();

//        return (num > 0 ? true : false);
//    };

//    quint64 currentDay = 364 - QDateTime::fromTime_t(timestamp).date().daysTo(QDate(1, 1, 1));
//    quint64 lastIdDay = (0 != lastId ? lastId >> 32 : 0);
//    quint64 startPos = 0;
//    quint64 endPos = (currentDay << 32) + ((instrumentId % 0xffff) << 16) + 0xffff;

//    DEBUG_LOG(__FUNCTION__);
//    if (currentDay == lastIdDay) {
//        startPos = lastId;
//    } else {
//        startPos = (currentDay << 32) + ((instrumentId % 0xffff) << 16);
//    }
//    DEBUG_LOG(__FUNCTION__);

//    if ((startPos & 0xffff) == 0xffff) {
//        DEBUG_LOG(__FUNCTION__);
//        return 0;
//    }

//    DEBUG_LOG(__FUNCTION__ << "start pos " << startPos << ", endpos " << endPos << ", sql length" << sql.length());
//    for (startPos = startPos + 1; sql.length() > 0 && startPos <= endPos; ++startPos) {
//        if (false == selectCountFun(startPos, sql)) {
//            DEBUG_LOG(__FUNCTION__);
//            return startPos;
//        }
//    }
//    DEBUG_LOG(__FUNCTION__);

//    if (sql.length() == 0) {
//        return startPos + 1;
//    }

//    return 0;
//}

//QString SqlWorker::formatIdString(quint64 id) {
//    QDate date = QDate(1, 1, 1).addDays((id >> 32) - 364);
//    uint instrument = (id >> 16) & 0xffff;
//    uint index = id & 0xffff;
//    return QString("%1-%2-%3").arg(date.toString("yyMMdd")).
//            arg(instrument, 4, 10, QLatin1Char('0')).arg(index, 4, 10, QLatin1Char('0'));
//}

//void SqlWorker::checkOldData()
//{
//    int currentTime_t = abs(QDateTime::currentDateTime().time().secsTo(QTime(0, 0, 0))); /// 到0点的秒数

//    //每天进行一次检查，删除60天前的数据库记录
//    if (currentTime_t  == 0)
//    {
//        //删除60天前的数据库记录
//        cleanOldData(60 * 24 * 60 * 60);
//    }
//}

//void SqlWorker::setInstrumentId(int instrumentId) {
//    SqlWorker::instrumentId = instrumentId;
//}

//bool SqlWorker::convertUnsyncRecordStatus() {
//    QMutexLocker locker(&m_Mutex);
//    DEBUG_LOG(__FUNCTION__);
//    QSqlQuery query(db);
//    QString format("update %1 set SyncFlag = 0 where SyncFlag in (1)");

//    auto sqlFun = [&](QString &tableName) -> bool {
//        QString info = format.arg(tableName);
//        DEBUG_LOG(ConvertQ2CString(info));
//        if (!query.exec(info)) {
//            ERROR_LOG("SqlWorker: Failed to Convert Status For " << ConvertQ2CString(tableName) << ":"
//                                                                 << ConvertQ2CString(query.lastError().text()));
//            return false;
//        }
//        return true;
//    };

//    db.transaction();
//    QString arr[] = {"TSession", "TUserInTraining", "TUserEvent", "TSystemEvent",
//                     "TFtpEvent", "TDeviceCaptureEvent", "TBookingEntrustEvent"};
//    for (int i = 0; i < (int) (sizeof(arr) / sizeof(arr[0])); ++i) {
//        if (false == sqlFun(arr[i])) {
//            break;
//        }
//    }
//    db.commit();

//    QProcess::execute("sync");
//    return true;
//}

//bool SqlWorker::addDeviceCaptureEvent(const biotech::osla::DeviceCaptureEvent &deviceCaptureEvent) {
//    QMutexLocker locker(&m_Mutex);

//    DEBUG_LOG("call this function[" << __FUNCTION__ << "]");

//    auto insertToDB = [&]() -> bool {
//        QSqlQuery query(db);
//        query.prepare("insert into TDeviceCaptureEvent(DeviceName, DeviceValue, createTimer)"\
//                      "values(:DeviceName, :DeviceValue, :CreateTimer)");
//        query.bindValue(":DeviceName", QVariant(deviceCaptureEvent.devicename().c_str()));
//        query.bindValue(":DeviceValue", QVariant(deviceCaptureEvent.devicevalue().c_str()));
//        query.bindValue(":CreateTimer", QVariant((qlonglong) time(nullptr)));

//        if (!query.exec()) {
//            ERROR_LOG("SqlWorker: Failed to insert TFtpEvent:" << ConvertQ2CString(query.lastError().text()));
//            return false;
//        }

//        return true;
//    };

//    db.transaction();
//    bool status = insertToDB();
//    if (false == status) {
//        db.rollback();
//    } else {
//        db.commit();
//    }

//    QProcess::execute("sync");
//    QProcess::execute("sync");
//    return status;
//}

//biotech::osla::DeviceCaptureEventList SqlWorker::getDeviceCaptureEvent(uint syncTimestamp, uint count) {
//    QMutexLocker locker(&m_Mutex);
//    DEBUG_LOG(__FUNCTION__);
//    biotech::osla::DeviceCaptureEventList deviceCaptureEventList;
//    QSqlQuery query(db);
//    db.transaction();
//    query.prepare("select ID, DeviceName, DeviceValue, createTimer "\
//                  "  from TDeviceCaptureEvent "\
//                  " where SyncFlag = 0 "\
//                  " order by ID limit :MaxListSize");
//    query.bindValue(":MaxListSize", count);
//    if (!query.exec()) {
//        DEBUG_LOG("SqlWorker: Failed to select TDeviceCaptureEvent:" << ConvertQ2CString(query.lastError().text()));
//        query.finish();
//        return deviceCaptureEventList;
//    }

//    while (query.next()) {
//        biotech::osla::DeviceCaptureEvent *deviceCaptureEvent = deviceCaptureEventList.add_device_capture_event();
//        deviceCaptureEvent->set_id(query.value(0).toLongLong());
//        deviceCaptureEvent->set_instrumentid(instrumentId);
//        deviceCaptureEvent->set_devicename(query.value(1).toString().toStdString().data());
//        deviceCaptureEvent->set_devicevalue(query.value(2).toString().toStdString().data());
//        deviceCaptureEvent->set_capturetime(query.value(3).toInt());
//    }
//    query.finish();

//    for (int i = 0; i < deviceCaptureEventList.device_capture_event_size(); ++i) {
//        const biotech::osla::DeviceCaptureEvent &deviceCaptureEvent = deviceCaptureEventList.device_capture_event(i);
//        query.prepare("update TDeviceCaptureEvent set SyncTime = :SyncTime, SyncFlag = 1 where ID = :ID");
//        query.bindValue(":SyncTime", (qlonglong) time(nullptr));
//        query.bindValue(":ID", deviceCaptureEvent.id());
//        if (!query.exec()) {
//            ERROR_LOG("SqlWorker: Failed to update TDeviceCaptureEvent:" << ConvertQ2CString(query.lastError().text()));
//            db.rollback();
//            query.finish();
//            deviceCaptureEventList.clear_device_capture_event();
//            return deviceCaptureEventList;
//        }
//    }
//    db.commit();

//    DEBUG_LOG(__FUNCTION__);
//    QProcess::execute("sync");
//    return deviceCaptureEventList;
//}

//bool SqlWorker::updateEntrust(const biotech::osla::EntrustList &entrustList) {
//    QMutexLocker locker(&m_Mutex);
//    DEBUG_LOG("Parse Server Command : [" << __FUNCTION__ << "]");
//    QSqlQuery query(db);

//    db.transaction();
//    for (int i = 0; i < entrustList.entrust_size(); i++) {
//        const biotech::osla::Entrust &entrust = entrustList.entrust(i);
//        int status = entrust.status();
//        DEBUG_LOG("entrust status is : " << status);

//        if (EntrustStatus::TERMINAL == status) {
//            query.prepare("delete from TBookingEntrust where ID=:ID");
//            query.bindValue(":ID", entrust.id());
//            if (!query.exec()) {
//                ERROR_LOG("SqlWorker: Failed to update TBookingEntrust:" << ConvertQ2CString(query.lastError().text()));
//                db.rollback();
//                return false;
//            }
//            DEBUG_LOG("SqlWorker: TBookingEntrust " << entrust.id() << " deleted");
//        } else if (EntrustStatus::DONT_DO == status || EntrustStatus::DONT_OVER == status ||
//                   EntrustStatus::HAVA_OVER == status) {

//            query.prepare("replace into TBookingEntrust(ID, UserID, Title, Status, IsInExperiment) "\
//                          "values(:ID, :UserID, :Title, :Status, :IsInExperiment)");
////            query.prepare("replace into TBookingEntrust(ID, UserID, Title, Status) "\
////                          "values(:ID, :UserID, :Title, :Status,)");
//            DEBUG_LOG("entrust id : " << entrust.id());
//            DEBUG_LOG("entrust userid : " << entrust.user_id());
//            DEBUG_LOG("entrust status: " << status);
//            DEBUG_LOG("entrust title:" << entrust.title());
//            DEBUG_LOG("entrust is doing:" << entrust.is_doing());

//            query.bindValue(":ID", entrust.id());
//            query.bindValue(":UserID", entrust.user_id());
//            query.bindValue(":Title", QString(entrust.title().c_str()));
//            query.bindValue(":Status", status);
//            query.bindValue(":IsInExperiment", entrust.is_doing());

//            if (!query.exec()) {
//                ERROR_LOG("SqlWorker: Failed to update TBookingEntrust:" << ConvertQ2CString(query.lastError().text()));
//                db.rollback();
//                return false;
//            }
//        }
//    }
//    db.commit();
//    QProcess::execute("sync");
//    DEBUG_LOG("SqlWorker: TBookingEntrust updated");
//    return true;
//}

//bool SqlWorker::updateEntrustStatus(const biotech::osla::Entrust &entrust) {
//    QMutexLocker locker(&m_Mutex);
//    QSqlQuery query(db);
//    db.transaction();
//    query.prepare("update TBookingEntrust "\
//                  "   set Status = :Status, "\
//                  "       IsInExperiment = :IsInExperiment "\
//                  " where ID = :ID");
//    query.bindValue(":Status", entrust.status());
//    query.bindValue(":ID", entrust.id());
//    query.bindValue(":IsInExperiment", entrust.is_doing());
//    if (!query.exec()) {
//        ERROR_LOG("SqlWorker: Failed to update TBookingEntrust:" << ConvertQ2CString(query.lastError().text()));
//        db.rollback();
//        return false;
//    }
//    QProcess::execute("sync");
//    db.commit();
//    return true;
//}

//bool SqlWorker::addEntrustEvent(const biotech::osla::EntrustEvent &entrustEvent) {
//    QMutexLocker locker(&m_Mutex);
//    DEBUG_LOG("Parse Server Command : [" << __FUNCTION__ << "]");
//    QSqlQuery query(db);

//    db.transaction();

//    query.prepare("Insert into TBookingEntrustEvent(EntrustID, Status, IsInExperiment, SessionId, CreateTime) "\
//                  "values(:EntrustID, :Status, :IsInExperiment, :SessionId, :CreateTime)");
//    query.bindValue(":EntrustID", entrustEvent.entrust_id());
//    query.bindValue(":Status", entrustEvent.status());
//    query.bindValue(":IsInExperiment", entrustEvent.is_doing());
//    query.bindValue(":SessionId", entrustEvent.session_id());
//    query.bindValue(":CreateTime", QVariant((qlonglong) time(nullptr)));
//    if (!query.exec()) {
//        ERROR_LOG("SqlWorker: Failed to update TBookingEntrustEvent:" << ConvertQ2CString(query.lastError().text()));
//        db.rollback();
//        return false;
//    }
//    db.commit();
//    DEBUG_LOG("SqlWorker: TBookingEntrust updated");
//    QProcess::execute("sync");
//    return true;
//}


//biotech::osla::EntrustEventList SqlWorker::getEntrustEvent(uint syncTimestamp, uint count) {
//    QMutexLocker locker(&m_Mutex);
//    biotech::osla::EntrustEventList entrustEventList;

//    QSqlQuery query(db);
//    query.prepare("select id, EntrustID, Status, IsInExperiment, SessionId "\
//                  "  from TBookingEntrustEvent "\
//                  " where SyncFlag = 0 "\
//                  " order by ID limit :MaxListSize");
//    query.bindValue(":MaxListSize", count);
//    if (!query.exec()) {
//        DEBUG_LOG("SqlWorker: Failed to select TBookingEntrustEvent:" << ConvertQ2CString(query.lastError().text()));
//        query.finish();
//        return entrustEventList;
//    }

//    biotech::osla::EntrustEvent *entrustEvent;
//    while (query.next()) {
//        entrustEvent = entrustEventList.add_entrust_event();
//        entrustEvent->set_id(query.value(0).toLongLong());
//        entrustEvent->set_entrust_id(query.value(1).toInt());
//        entrustEvent->set_status(query.value(2).toInt());
//        entrustEvent->set_is_doing(query.value(3).toInt());
//        entrustEvent->set_session_id(query.value(4).toLongLong());
//        biotech::osla::Session *session = entrustEvent->mutable_sessioninfo();
//        biotech::osla::Session s = getSession(entrustEvent->session_id());
//        session->CopyFrom(s);
//    }
//    query.finish();

//    db.transaction();
//    for (int i = 0; i < entrustEventList.entrust_event_size(); ++i) {
//        const biotech::osla::EntrustEvent &inst = entrustEventList.entrust_event(i);
//        query.prepare("update TBookingEntrustEvent set SyncTime = :SyncTime, SyncFlag = 1 where ID = :ID");
//        query.bindValue(":SyncTime", syncTimestamp);
//        query.bindValue(":ID", inst.id());
//        if (!query.exec()) {
//            DEBUG_LOG("SqlWorker: Failed to update TUserEvent:" << ConvertQ2CString(query.lastError().text()));
//            query.finish();
//            db.rollback();
//            entrustEventList.clear_entrust_event();
//            return entrustEventList;
//        }
//    }
//    db.commit();
//    QProcess::execute("sync");

//    return entrustEventList;
//}

//bool SqlWorker::delSyncFlag(QString tableName, long long id) {
//    QMutexLocker locker(&m_Mutex);
//    QSqlQuery query(db);
//    db.transaction();
//    QString format = QString("delete from %1 Where id=%2");
//    QString info = format.arg(tableName).arg(id);
//    DEBUG_LOG("delete sync flag [table:" << ConvertQ2CString(tableName) << ", id:" << id << "]");
//    if (!query.exec(info)) {
//        ERROR_LOG("SqlWorker: Failed to delete " << ConvertQ2CString(tableName) << ":"
//                                                 << ConvertQ2CString(query.lastError().text()));
//        db.rollback();
//        return false;
//    }
//    db.commit();
//    DEBUG_LOG("SqlWorker:" << tableName.toLocal8Bit().data() << " updated success");

//    QProcess::execute("sync");
//    return true;
//}

//biotech::osla::EntrustList SqlWorker::getEntrustList(int begin, int size, std::list<int> status, int isEx) {
//    QMutexLocker locker(&m_Mutex);

//    biotech::osla::EntrustList entrustList;

//    QString statusInfo;

//    auto iter = status.begin();
//    if (iter != status.end()) {
//        statusInfo += QString::number(*iter, 10);
//        iter++;
//    }

//    for (; iter != status.end(); ++iter) {
//        statusInfo += "," + QString::number(*iter, 10);
//    }

//    QString sql = QString("select id, Title, Status from TBookingEntrust where status in (%1)").arg(statusInfo);
//    if (0 == isEx) {
//        sql += QString("   and IsInExperiment = 0 ");
//    } else if (1 == isEx) {
//        sql += QString("   and IsInExperiment = 1 ");
//    }
//    sql += QString(" order by id desc limit %1, %2").arg(begin).arg(size);

//    DEBUG_LOG(__FUNCTION__ << "sql is [" << ConvertQ2CString(sql) << "]");
//    QSqlQuery query(db);
//    query.prepare(sql);
//    if (!query.exec()) {
//        DEBUG_LOG("SqlWorker: Failed to select EntrustList:" << ConvertQ2CString(query.lastError().text()));
//        query.finish();
//        return entrustList;
//    }

//    biotech::osla::Entrust *entrust;
//    while (query.next()) {
//        entrust = entrustList.add_entrust();
//        entrust->set_id(query.value(0).toLongLong());
//        QByteArray byte = query.value(1).toString().toUtf8();
//        entrust->set_title(byte.data());
//        entrust->set_status(query.value(2).toInt());
//    }
//    query.finish();

//    return entrustList;
//}

//int SqlWorker::countValidEntrustList(std::list<int> status, int inEx) {
//    QMutexLocker locker(&m_Mutex);

//    QString statusInfo;
//    auto iter = status.begin();
//    if (iter != status.end()) {
//        statusInfo += QString::number(*iter, 10);
//        iter++;
//    }

//    for (; iter != status.end(); ++iter) {
//        statusInfo += "," + QString::number(*iter, 10);
//    }

//    QSqlQuery query(db);
//    QString info = QString("select count(*) from TBookingEntrust where status in (%1)").arg(statusInfo);
//    if (0 == inEx) {
//        info += QString(" and IsInExperiment = 0 ");
//    } else if (1 == inEx) {
//        info += QString(" and IsInExperiment = 1 ");
//    }

//    DEBUG_LOG(__FUNCTION__ << "sql is [" << ConvertQ2CString(info) << "]");
//    if (!query.exec(info)) {
//        ERROR_LOG("SqlWorker: Failed to select from TBookingEntrust");
//        query.finish();
//        return 0;
//    }

//    int val = 0;
//    if (query.first()) {
//        val = query.value(0).toInt();
//    }
//    query.finish();
//    return val;
//}

//bool SqlWorker::getUserIsInPenaltyStop(User &user, int now) {
//    QMutexLocker locker(&m_Mutex);

//    user.inPenaltyStopStart = 0;
//    user.inPenaltyStopEnd = 0;
//    user.isInPenaltyStop = false;

//    QSqlQuery query(db);
//    query.prepare("select penaltyStopStart, penaltyStopEnd from TUser where id = :id");
//    query.bindValue(":id", user.id);

//    if (!query.exec()) {
//        DEBUG_LOG("SqlWorker: Failed to select from TUser for penaltyStop :"
//                          << ConvertQ2CString(query.lastError().text()));
//        query.finish();
//        return false;
//    }

//    DEBUG_LOG("");
//    if (query.next()) {
//        DEBUG_LOG("start :" << user.inPenaltyStopStart);
//        DEBUG_LOG("end:" << user.inPenaltyStopEnd);
//        user.inPenaltyStopStart = query.value(0).toInt();
//        user.inPenaltyStopEnd = query.value(1).toInt();
//        time_t n = time(NULL);
//        DEBUG_LOG("now:" << n);
//        if (user.inPenaltyStopStart < n && user.inPenaltyStopEnd > n) {
//            user.isInPenaltyStop = true;
//        }
//    }

//    query.finish();
//    return true;
//}
