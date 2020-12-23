/******************************************************************************************
*-----First Created. time: 16-November-2020	4:35  P.m.
* @File name    :log.cpp
* @Platform     :imx6ull * openwrt19.07.2 * linux 4.1.15 * Qt5.5.1 * arm-openwrt-linux-gnueabi-
* @Author       :MacianYuan
* @E-mail       :macian@foxmail.com
* @Project      :边缘网关控制器（楼宇供暖控制系统）
*******************************************************************************************
* @Description  :日志系统
* @Attention    :
* @Log          :
*****************16-November-2020(MacianYuan):
*****************
* @Copyright© @MacianYuan Anhui Xinjie Technology Co.,Ltd.  2010-2020  All rights reserved.
******************************************************************************************/


#include "tools/log.h"

#include <log4cplus/ndc.h>
#include <log4cplus/logger.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/loggingmacros.h>

using namespace log4cplus;
using namespace log4cplus::helpers;

bool LoggerControl::init(std::string fileName, int size, int num) {
    /* 实例化 _append 对象*/
    SharedAppenderPtr _append(new RollingFileAppender(fileName, size, num));
    _append->setName("QT");
    /* 实例化 Layout 对象,将布局对象加入_append*/
    std::string pattern("%D{%m/%d/%y %H:%M:%S} : %m [%F:%L]%n");
    _append->setLayout(std::auto_ptr<Layout>(new PatternLayout(pattern)));
    /*将appender对象附加到日志记录器*/
    Logger::getRoot().addAppender(_append);
    return true;
}

Logger LoggerControl::getRoot() {
    static Logger root = Logger::getRoot();
    return root;
}

LoggerControl& LoggerControl::getInstance() {
    static LoggerControl info;
    return info;
}
