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


#include "publicdefine.h"

int PublicDefine::ReconnectInterval = 15; // 自动重连间隔（秒）
int PublicDefine::MaxRoundTripTime = 20; // 允许的最大网络延迟（秒）
int PublicDefine::MinHeartbeatCount = 3; // 判断网络可靠所需最小连续心跳数
int PublicDefine::OfflineTimeOut = 90; // 判断离线的超时时间（秒）
int PublicDefine::SyncTimeOut = 15; // 同步超时时间（秒）
int PublicDefine::ForceRecheckInterval = 3600; // 强制重新检查数据库间隔（秒）
int PublicDefine::MaxSyncTryCount = 3; // 最大同步尝试次数
int PublicDefine::MaxListSize = 10;

int PublicDefine::getReconnectInterval() {
    return ReconnectInterval;
}

int PublicDefine::getMaxRoundTripTime() {
    return MaxRoundTripTime;
}

int PublicDefine::getMinHeartbeatCount() {
    return MinHeartbeatCount;
}

int PublicDefine::getOfflineTimeOut() {
    return OfflineTimeOut;
}

int PublicDefine::getSyncTimeOut() {
    return SyncTimeOut;
}

int PublicDefine::getForceRecheckInterval() {
    return ForceRecheckInterval;
}

int PublicDefine::getMaxSyncTryCount() {
    return MaxSyncTryCount;
}

int PublicDefine::getMaxListSize() {
    return MaxListSize;
}
// 单条消息允许包含最大记录数，超过此值需分多条消息发送
