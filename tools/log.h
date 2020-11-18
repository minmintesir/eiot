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

#ifndef OSLA_NEW_LOG_H
#define OSLA_NEW_LOG_H

#include <string>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

using namespace log4cplus;

#define DEBUG_LOG(p) LOG4CPLUS_DEBUG(LoggerControl::getInstance().getRoot(), __FUNCTION__ << " " << p)
#define INFO_LOG(p)  LOG4CPLUS_INFO(LoggerControl::getInstance().getRoot(), __FUNCTION__ << " " << p)
#define WARN_LOG(p)  LOG4CPLUS_WARN(LoggerControl::getInstance().getRoot(), __FUNCTION__ << " " << p)
#define ERROR_LOG(p) LOG4CPLUS_ERROR(LoggerControl::getInstance().getRoot(), __FUNCTION__ << " " << p)

class LoggerControl {
public:
    static LoggerControl &getInstance();
    bool init(std::string fileName, int size, int num);
    Logger getRoot();

private:
    LoggerControl() {};
    ~LoggerControl() {};
};


#endif //OSLA_NEW_LOG_H
