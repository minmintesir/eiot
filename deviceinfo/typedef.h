/******************************************************************************************
*-----First Created. time: 16-November-2020	4:35  P.m.
* @File name    :main.cpp
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

#ifndef TYPEDEF_H
#define TYPEDEF_H

typedef struct AccountInfo {
    uint id;
    uint status;
    uint groupId;
    int deposit;
    int credit;
} Account;

typedef struct CardInfo {
    std::string serial;
    uint type;
    uint status; // 0-有效卡，1-未生效卡，2-过期卡（仅对临时卡有效）
} Card;

typedef struct UserInfo {
    uint id;
    uint status;
    QString firstName;
    QString lastName;
    QString fullName;
    QString phone;
    bool isInBlacklist;
    uint right;
    uint longTimeRight;
    uint rightExpireTime;
    Account account;
    Card card;
    uint timestamp;
    bool isInPenaltyStop;
    int inPenaltyStopStart;
    int inPenaltyStopEnd;
} User;


struct ConditionNode {
    bool status;
    QString info;
    QString soundPath;
    int errorCode;
};

#endif // TYPEDEF_H
